#ifndef GLUT_FUNCS_H
#define GLUT_FUNCS_H

#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>  // for the format

#include "delfem2/camera.h"

#if defined(__APPLE__) && defined(__MACH__)
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

// end of header
// --------------------------------------------

/*
static void DrawSphere(double r, double x, double y, double z)
{
  ::glMatrixMode(GL_MODELVIEW);
  ::glPushMatrix();
  ::glTranslated(+x,+y,+z);
  ::glutSolidSphere(r,32, 32);
  ::glPopMatrix();
}
 */

static void RenderBitmapString(float x, float y, void *font,const char *string)
{
  ::glRasterPos2f(x, y);
  for(unsigned int i=0;string[i]!='\0';i++){
    ::glutBitmapCharacter(font, string[i]);
  }
}


static void DrawMessage_LeftTop(const char* msg, int iline=0)
{
  int* font=(int*)GLUT_BITMAP_8_BY_13;
  ////
  glPushAttrib(GL_TRANSFORM_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POINT_BIT);
  ::glDisable(GL_LIGHTING);
  ::glDisable(GL_TEXTURE_2D);
  ::glDisable(GL_DEPTH_TEST);
  GLint viewport[4]; ::glGetIntegerv(GL_VIEWPORT,viewport);
  const int win_w = viewport[2];
  const int win_h = viewport[3];
  ////
  ::glMatrixMode(GL_PROJECTION);
  ::glPushMatrix();
  ::glLoadIdentity();
  ::gluOrtho2D(0, win_w, 0, win_h);
  ::glMatrixMode(GL_MODELVIEW);
  ::glPushMatrix();
  ::glLoadIdentity();
  ////
  ::glScalef(1, -1, 1);
  ::glTranslatef(0, -(float)win_h, 0);
//  ::glColor3d(0.0, 0.0, 0.0);
  RenderBitmapString(10.0f,18.0f+iline*12.0f, (void*)font, msg);
  ////
  ::glMatrixMode(GL_PROJECTION);
  ::glPopMatrix();
  ::glMatrixMode(GL_MODELVIEW);
  ::glPopMatrix();
  ::glPopAttrib();
}



static void ShowFPS(){
  ::glDisable(GL_LIGHTING);
//  static char s_fps[32];
  static std::string s_fps;
  {
    static int frame, timebase;
    int time;
    frame++;
    time=glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 500) {
      std::stringstream ss;
      ss << "FPS:";
      ss << std::fixed << std::setprecision(2);
      ss << frame*1000.0/(time-timebase);
      s_fps = ss.str();
//      s_fps = std::string("FPS:")+std::to_string(frame*1000.0/(time-timebase));
//      sprintf(s_fps,"FPS:%4.2f",frame*1000.0/(time-timebase));
      timebase = time;
      frame = 0;
    }
  }
//  DrawMessage_LeftTop(s_fps);
  DrawMessage_LeftTop(s_fps.c_str());
}



/**
 * @brief class for each GLUT window
 */
class CNav3D_GLUT
{
public:  
  void glutMouse(int button, int state, int x, int y){
    GLint viewport[4];
    ::glGetIntegerv(GL_VIEWPORT,viewport);
    imodifier = glutGetModifiers();
    /*
    if( imodifier == 0 ){
      if( button == 1 && state == GLUT_DOWN ){
        imodifier = GLUT_ACTIVE_ALT;
      }
      if( button == 2 && state == GLUT_DOWN ){
        imodifier = GLUT_ACTIVE_CTRL;
      }
    }
     */
    if( state == GLUT_UP ){ this->ibutton = -1; }
    else{                   this->ibutton = button; }
    const int win_w = viewport[2];
    const int win_h = viewport[3];
    mouse_x = (2.0*x-win_w)/win_w;
    mouse_y = (win_h-2.0*y)/win_h;
    if( state == GLUT_DOWN ){
      mouse_x_down = mouse_x;
      mouse_y_down = mouse_y;
    }
    ::glutPostRedisplay();
  }
  void glutMotion( int x, int y ){
    GLint viewport[4];
    ::glGetIntegerv(GL_VIEWPORT,viewport);
    const int win_w = viewport[2];
    const int win_h = viewport[3];
    const double mov_end_x = (2.0*x-win_w)/win_w;
    const double mov_end_y = (win_h-2.0*y)/win_h;
    dx = mov_end_x - mouse_x;
    dy = mov_end_y - mouse_y;
    {
      if(      imodifier == GLUT_ACTIVE_ALT   ){
        camera.Rot_Camera(dx, dy);
      }
      else if( imodifier == GLUT_ACTIVE_SHIFT ){
        camera.Pan_Camera(dx, dy);
      }
    }
    mouse_x = mov_end_x;
    mouse_y = mov_end_y;
    ::glutPostRedisplay();
  }
  void glutSpecial(int Key, int x, int y)
  {
    switch(Key)
    {
      case GLUT_KEY_PAGE_UP:
        camera.Scale(1.03);
        break;
      case GLUT_KEY_PAGE_DOWN:
        camera.Scale(1.0/1.03);
        break;
      case GLUT_KEY_F1:
        camera.is_pars = !camera.is_pars;
        break;
      case GLUT_KEY_F2:
        camera.fovy *= 1.05;
        break;
      case GLUT_KEY_F3:
        camera.fovy /= 1.05;
        break;
      case GLUT_KEY_LEFT:
        break;
      case GLUT_KEY_RIGHT:
        break;
      case GLUT_KEY_UP:
        break;
      case GLUT_KEY_DOWN:
        break;
      case GLUT_KEY_HOME :
        break;
      case GLUT_KEY_END :
        break;
      default:
        break;
    }
    ::glutPostRedisplay();
  }
  void PosMouse2D(float& x, float& y){
//    float mMV[16], mP[16]; this->Matrix_MVP(mMV, mP, window);
    float mMV[16]; glGetFloatv(GL_MODELVIEW_MATRIX, mMV);
    float mP[16]; glGetFloatv(GL_PROJECTION_MATRIX, mP);
    const float sp0[3] = {(float)mouse_x, (float)mouse_y,0.0};
    float src_pick[3];
    delfem2::screenUnProjection(src_pick,
                       sp0, mMV,mP);
    x = src_pick[0];
    y = src_pick[1];
  }
  void PosMove2D(float& x0, float& y0,
                 float& x1, float& y1) const
  {
    float mMV[16]; glGetFloatv(GL_MODELVIEW_MATRIX, mMV);
    float mP[16]; glGetFloatv(GL_PROJECTION_MATRIX, mP);
    {
      const float sp0[3] = {(float)(mouse_x-dx), (float)(mouse_y-dy),0.0};
      float src0[3];
      delfem2::screenUnProjection(src0,
                         sp0, mMV,mP);
      x0 = src0[0]; y0 = src0[1];
    }
    {
      const float sp1[3] = {(float)mouse_x, (float)mouse_y,0.0};
      float src1[3];
      delfem2::screenUnProjection(src1,
                         sp1, mMV,mP);
      x1 = src1[0]; y1 = src1[1];
    }
  }
  void SetGL_Camera()
  {
    GLint viewport[4]; ::glGetIntegerv(GL_VIEWPORT,viewport);
    const int win_w = viewport[2];
    const int win_h = viewport[3];
    {
      ::glMatrixMode(GL_PROJECTION);
      ::glLoadIdentity();
      float mP[16];
      double depth = camera.view_height/(camera.scale*tan(0.5*camera.fovy*3.1415/180.0));
      camera.Affine4f_Projection(mP, (double)win_w/win_h, depth);
      ::glMultMatrixf(mP);
    }
    {
      ::glMatrixMode(GL_MODELVIEW);
      ::glLoadIdentity();
      float mMV[16];
      camera.Affine4f_ModelView(mMV);
      ::glMultMatrixf(mMV);
    }
  }
public:
  int iwin;
  int imodifier;
  int ibutton;
  delfem2::CCamera<double> camera;
  double mouse_x, mouse_y;
  double dx;
  double dy;
  double mouse_x_down, mouse_y_down;
};



#endif
