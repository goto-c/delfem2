#include <iostream>
#include <sstream>
#include <math.h>
#include <complex>
#include <set>
#include <deque>
#include <set>
#include <stack>
#include "delfem2/mat3.h"
#include "delfem2/cad3d.h"

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

// ----------
#include <GLFW/glfw3.h>
#include "delfem2/opengl/glfw_viewer.hpp"
#include "delfem2/opengl/gl2_funcs.h"
#include "delfem2/opengl/gl2_v23.h"

namespace dfm2 = delfem2;

// ------------------------------

/*

void CCad3D_Face::DrawFace() const
{
  dfm2::opengl::DrawMeshTri3D_FaceNorm(aXYZ, aTri, aNorm);
}


void CCad3D::DrawFace_LeftRight() const
{
  ::glEnable(GL_LIGHTING);
  dfm2::opengl::myGlColorDiffuse(color_face);
  dfm2::opengl::DrawMeshTri3D_FaceNorm(aXYZ, aTri, aNorm);
}

void CCad3D::DrawFace_RightSelected(bool is_edge) const
{
  {
    float specular[4] ={ 0.797357f, 0.723991f, 0.208006f, 1.0f};
    float shine =83.2f ;
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shine);
  }
  
  ::glEnable(GL_LIGHTING);
  for(unsigned int iface=0;iface<aFace.size();++iface){
    if( iface == iface_picked ){
      dfm2::opengl::myGlColorDiffuse(color_face_selected);
    }
    else{
      dfm2::opengl::myGlColorDiffuse(color_face);
    }
    aFace[iface].DrawFace();
    if( is_edge ){ aFace[iface].DrawEdge(); }
  }
}

void CCad3D::DrawVtxEdgeHandler(double view_height) const
{

  for(unsigned int icp=0;icp<aVertex.size();++icp){
    aVertex[icp].Draw(icp==ivtx_picked,ielem_vtx_picked, view_height);
  }
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//  glEnable(GL_MULTISAMPLE);
  
  dfm2::opengl::myGlColorDiffuse(dfm2::CColor::Blue());
  for(int ie=0;ie<(int)aEdge.size();++ie){
    bool is_loop0 = std::find(aIE_picked.begin(), aIE_picked.end(), std::make_pair(ie,true ) ) != aIE_picked.end();
    bool is_loop1 = std::find(aIE_picked.begin(), aIE_picked.end(), std::make_pair(ie,false) ) != aIE_picked.end();
    bool is_loop = is_loop0 || is_loop1;
    aEdge[ie].DrawLine(is_loop, view_height);
  }
  if( iedge_picked != -1 ){
    aEdge[iedge_picked].DrawHandler(ielem_edge_picked, view_height);
  }

  ::glDisable(GL_LIGHTING);
  ::glDisable(GL_CULL_FACE);
  if( plane_inorm >= 0 && plane_inorm < 3 ){
    dfm2::opengl::myGlColorDiffuse(dfm2::CColor(0.0, 0.0, 1.0, 0.5));
    CVector3 plane_ex = CVector3::Axis((plane_inorm+1)%3);
    CVector3 plane_ey = CVector3::Axis((plane_inorm+2)%3);
    CVector3 p0 = plane_org-plane_sizeX*plane_ex-plane_sizeY*plane_ey;
    CVector3 p1 = plane_org+plane_sizeX*plane_ex-plane_sizeY*plane_ey;
    CVector3 p2 = plane_org+plane_sizeX*plane_ex+plane_sizeY*plane_ey;
    CVector3 p3 = plane_org-plane_sizeX*plane_ex+plane_sizeY*plane_ey;
    ::glBegin(GL_QUADS);
    dfm2::opengl::myGlVertex(p0);
    dfm2::opengl::myGlVertex(p1);
    dfm2::opengl::myGlVertex(p2);
    dfm2::opengl::myGlVertex(p3);
    ::glEnd();
    ////
    ::glLineWidth(5);
    if( ielem_edge_picked == 1 ){ dfm2::opengl::myGlColorDiffuse(dfm2::CColor(1.0, 0.0, 0.0, 0.9)); }
    else{                         dfm2::opengl::myGlColorDiffuse(dfm2::CColor(0.0, 0.0, 1.0, 0.9)); }
    dfm2::opengl::DrawCylinder(p0, p1, view_height*0.01);
    dfm2::opengl::DrawCylinder(p1, p2, view_height*0.01);
    dfm2::opengl::DrawCylinder(p2, p3, view_height*0.01);
    dfm2::opengl::DrawCylinder(p3, p0, view_height*0.01);
  }
  
  {
    ::glDisable(GL_DEPTH_TEST);
    ::glMatrixMode(GL_PROJECTION);
    ::glPushMatrix();
    ::glLoadIdentity();
    ::glMatrixMode(GL_MODELVIEW);
    ::glPushMatrix();
    ::glLoadIdentity();
    //////
    ::glDisable(GL_LIGHTING);
    ::glColor3d(1,0,0);
    ::glLineWidth(3);
    ::glBegin(GL_LINE_STRIP);
    for(unsigned int ist=0;ist<aStroke.size();++ist){
      dfm2::opengl::myGlVertex(aStroke[ist]);
    }
    ::glEnd();
    //
    ::glMatrixMode(GL_PROJECTION);
    ::glPopMatrix();
    ::glMatrixMode(GL_MODELVIEW);
    ::glPopMatrix();
    ::glEnable(GL_DEPTH_TEST);
  }
  
  glDisable(GL_LINE_SMOOTH);
  
}



void CCad3D_Face::DrawBackFace() const
{
  ::glEnable(GL_LIGHTING);
  dfm2::opengl::myGlColorDiffuse(dfm2::CColor::Gray(0.9));
  dfm2::opengl::DrawMeshTri3D_FaceNorm_XYsym(aXYZ, aTri);
}

void CCad3D_Face::DrawEdge() const
{
  ::glLineWidth(1);
  dfm2::opengl::DrawMeshTri3D_Edge(aXYZ, aTri);
}


void CCad3D_Vertex::Draw(bool is_selected, int ielem, double view_height) const
{
  ::glDisable(GL_LIGHTING);
  if( is_selected ){
    double s = view_height*0.3;
    ::glDisable(GL_LIGHTING);
    if( !isConst[0] ){
      ::glColor3d(1, 0, 0);
      dfm2::opengl::DrawArrow(pos, CVector3(+s, 0, 0));
      dfm2::opengl::DrawArrow(pos, CVector3(-s, 0, 0));
    }
    if( !isConst[1] ){
      ::glColor3d(0, 1, 0);
      dfm2::opengl::DrawArrow(pos, CVector3(0, +s, 0));
      dfm2::opengl::DrawArrow(pos, CVector3(0, -s, 0));
    }
    if( !isConst[2] ){
      ::glColor3d(0, 0, 1);
      dfm2::opengl::DrawArrow(pos, CVector3(0, 0, +s));
      dfm2::opengl::DrawArrow(pos, CVector3(0, 0, -s));
    }
  }
  else{
    ::glDisable(GL_LIGHTING);
    ::glColor3d(0,0,1);
    ::glMatrixMode(GL_MODELVIEW);
    ::glPushMatrix();
    dfm2::opengl::myGlTranslate(pos);
    ::glDisable(GL_CULL_FACE);
    ::glScaled(view_height*0.03, view_height*0.03, view_height*0.03);
    dfm2::opengl::DrawSphere(32, 32);
    ::glPopMatrix();
  }
  ////
  dfm2::opengl::myGlColorDiffuse(dfm2::CColor::Red());
//  DrawArrow(pos, +norm*view_height*0.1);
//  ::DrawArrow(pos, -norm*0.15);
}

void CCad3D_Edge::DrawLine(bool is_picked, double view_height) const
{
  ::glDisable(GL_LIGHTING);
  if( is_picked ){
    ::glColor3d(1,1,0);
  }
  else if( is_sim ){
    ::glColor3d(0,0,0);
  }
  else{
    ::glColor3d(0.8,0.8,0.8);
  }
  ::glLineWidth(2);
  ::glBegin(GL_LINE_STRIP);
  for(unsigned int ip=0;ip<aP.size();++ip){ dfm2::opengl::myGlVertex(aP[ip]); }
  ::glEnd();
}

void CCad3D_Edge::DrawHandler(int ielem_picked, double view_height) const
{
  ::glColor3d(0,1,0);
  dfm2::opengl::DrawCylinder(p0, q0, view_height*0.01);
  dfm2::opengl::DrawCylinder(p1, q1, view_height*0.01);
  {
    if( ielem_picked == 2 ){ ::glColor3d(1,0.0,0); }
    else{                    ::glColor3d(0,0.9,0); }
    ::glMatrixMode(GL_MODELVIEW);
    ::glPushMatrix();
    dfm2::opengl::myGlTranslate(q0);
    ::glScaled(view_height*0.02, view_height*0.02, view_height*0.02);
    dfm2::opengl::DrawSphere(32, 32);
    ::glPopMatrix();
  }
  {
    if( ielem_picked == 3 ){ ::glColor3d(1,0.0,0); }
    else{                    ::glColor3d(0,0.9,0); }
    ::glMatrixMode(GL_MODELVIEW);
    ::glPushMatrix();
    dfm2::opengl::myGlTranslate(q1);
    ::glScaled(view_height*0.02, view_height*0.02, view_height*0.02);
    dfm2::opengl::DrawSphere(32, 32);
    ::glPopMatrix();
  }
  ::glEnd();
}
*/

// ------------------------------

int main(int argc,char* argv[])
{
  class CCAD3DViewer : public delfem2::opengl::CViewer_GLFW {
  public:
    CCAD3DViewer(){
      cad.Initialize_Sphere();
      nav.camera.view_height = 1.5;
      nav.camera.camera_rot_mode = delfem2::CAMERA_ROT_YTOP;
      cad.imode_edit = CCad3D::EDIT_MOVE;
    }
    void Draw(){
      this->DrawBegin_oldGL();
//      cad.DrawFace_RightSelected(false);
//      cad.DrawVtxEdgeHandler(nav.camera.view_height);
      this->DrawEnd_oldGL();
    }
    virtual void mouse_press(const float src[3], const float dir[3]){
      const CVector3 src_pick(src), dir_pick(dir);
      float mMV[16], mPrj[16]; nav.Matrix_MVP(mMV, mPrj, this->window);
      cad.MouseDown(src_pick, dir_pick,
                    CVector2(nav.mouse_x,nav.mouse_y),
                    mMV,mPrj,
                    nav.camera.view_height);
    }
    virtual void mouse_drag(const float src0[3], const float src1[3], const float dir[3]){
      CVector2 sp0(nav.mouse_x-nav.dx, nav.mouse_y-nav.dy);
      CVector2 sp1(nav.mouse_x, nav.mouse_y);
      const CVector3 src_pick(src1), dir_pick(dir);
      float mMV[16], mPrj[16]; nav.Matrix_MVP(mMV, mPrj, this->window);
      cad.MouseMotion(src_pick,dir_pick, sp0,sp1, mMV, mPrj);
    }
  public:
    CCad3D cad;
  };
  // -------------
  CCAD3DViewer viewer;
  viewer.Init_oldGL();
  delfem2::opengl::setSomeLighting();
  while(!glfwWindowShouldClose(viewer.window)){
    viewer.Draw();
  }
  glfwDestroyWindow(viewer.window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}


