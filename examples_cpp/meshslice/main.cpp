#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__) && (__MACH__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "delfem2/msh.h"
#include "delfem2/mshio.h"
#include "delfem2/mshtopo.h"
#include "delfem2/vec3.h"

#include "delfem2/gl_funcs.h"
#include "delfem2/gl_color.h"
#include "delfem2/glut_funcs.h"


void IndexElement_OverlapLevels_MeshTri3D
(std::vector< std::vector<unsigned int> >& aCST,
 ////
 const std::vector<double>& aH,
 const CVector3& norm,
 const CVector3& centerOfGravity,
 const std::vector<double>& aXYZ,
 const std::vector<unsigned int>& aTri)
{
  const unsigned int ntri = aTri.size()/3;
  const unsigned int nH = aH.size();
  aCST.resize(nH);
  for(unsigned int itri=0;itri<ntri;itri++){
    double ah[3];
    for(unsigned int inotri=0;inotri<3;inotri++){
      unsigned int ino0 = aTri[itri*3+inotri];
      CVector3 p0( aXYZ[ino0*3+0], aXYZ[ino0*3+1], aXYZ[ino0*3+2] );
      ah[inotri] = Dot(p0-centerOfGravity,norm);
    }
    for(unsigned int ih=0;ih<nH;ih++){
      unsigned int icnt = 0;
      for(unsigned int inotri=0;inotri<3;inotri++){
        if( ah[inotri]-aH[ih] > 0 ){ icnt++; }
      }
      ////
      if( icnt == 1 || icnt == 2 ){
        aCST[ih].push_back(itri);
      }
    }
  }
}

void CrossSections_Levels_MeshTri3D
(std::vector<unsigned int>& aCS_H,
 std::vector< std::vector<unsigned int> >& aCS_IndTri,
 std::vector< std::vector<CVector3> >& aCS_LoopXYZ,
 const std::vector<double>& aHeight,
 const CVector3& norm,
 const CVector3& origin,
 const std::vector<double>& aXYZ,
 const std::vector<unsigned int>& aTri,
 const std::vector<int>& aTriSur)
{
  const unsigned int ntri = (unsigned int)aTri.size()/3;
  const unsigned int nH = (unsigned int)aHeight.size();
  ////
  std::vector< std::vector<unsigned int> > aCST;
  IndexElement_OverlapLevels_MeshTri3D(aCST,
                                  aHeight,norm,origin,
                                  aXYZ,aTri);
  /////
  aCS_H.clear();
  aCS_IndTri.clear();
  aCS_LoopXYZ.clear();
  
  std::vector<int> Tri2Seg;
  Tri2Seg.resize(ntri,-1);
  for(unsigned int ih=0;ih<nH;ih++){ // h loop
    for(unsigned int isg=0;isg<aCST[ih].size();isg++){
      unsigned int itri = aCST[ih][isg];
      Tri2Seg[itri] = isg;
    }
    ////
    std::vector<int> aFlgSeg;
    aFlgSeg.resize(aCST[ih].size(),0);
    unsigned int iseg_ker = 0;
    for(;;){ // cs loop
      for(;iseg_ker<aCST[ih].size();iseg_ker++){
        if( aFlgSeg[iseg_ker] == 0 ){ break; }
      }
      if( iseg_ker == aCST[ih].size() ) break;
      const unsigned int ihcs = (unsigned int)aCS_H.size();
      aCS_H.resize( ihcs+1 );
      aCS_IndTri.resize( ihcs+1 );
      aCS_LoopXYZ.resize( ihcs+1 );
      aCS_H[ihcs] = ih;
      aCS_LoopXYZ[ihcs].clear();
      aCS_IndTri[ihcs].clear();
      /////
      unsigned int iseg_next = iseg_ker;
      bool is_open = false;
      for(;;){ // seg in cs loop
        assert( aFlgSeg[iseg_next] == 0 );
        int jtri0 = aCST[ih][iseg_next];
        aFlgSeg[iseg_next] = 1;
        aCS_IndTri[ihcs].push_back(jtri0);
        ////
        unsigned int iflg = 0;
        CVector3 aP[3];
        double aH[3];
        for(unsigned int inotri=0;inotri<3;inotri++){
          unsigned int jno0 = aTri[jtri0*3+inotri];
          aP[inotri] = CVector3( aXYZ[jno0*3+0], aXYZ[jno0*3+1], aXYZ[jno0*3+2] );
          aH[inotri] = Dot(aP[inotri]-origin,norm)-aHeight[ih];
          if( aH[inotri] < 0 ){ continue; }
          if( inotri == 0 ){ iflg += 1; }
          if( inotri == 1 ){ iflg += 2; }
          if( inotri == 2 ){ iflg += 4; }
        }
        CVector3 v1, v2;
        unsigned int iedge_next;
        if( iflg == 1 ){ // 0
          v1 = -aH[1]/(aH[0]-aH[1])*aP[0] + aH[0]/(aH[0]-aH[1])*aP[1];
          v2 = -aH[2]/(aH[0]-aH[2])*aP[0] + aH[0]/(aH[0]-aH[2])*aP[2];
          iedge_next = 1;
        }
        if( iflg == 2 ){ // 1
          v1 = -aH[2]/(aH[1]-aH[2])*aP[1] + aH[1]/(aH[1]-aH[2])*aP[2];
          v2 = -aH[0]/(aH[1]-aH[0])*aP[1] + aH[1]/(aH[1]-aH[0])*aP[0];
          iedge_next = 2;
        }
        if( iflg == 4 ){ // 2
          v1 = -aH[0]/(aH[2]-aH[0])*aP[2] + aH[2]/(aH[2]-aH[0])*aP[0];
          v2 = -aH[1]/(aH[2]-aH[1])*aP[2] + aH[2]/(aH[2]-aH[1])*aP[1];
          iedge_next = 0;
        }
        if( iflg == 3 ){ // 01
          v1 = -aH[1]/(aH[2]-aH[1])*aP[2] + aH[2]/(aH[2]-aH[1])*aP[1];
          v2 = -aH[0]/(aH[2]-aH[0])*aP[2] + aH[2]/(aH[2]-aH[0])*aP[0];
          iedge_next = 1;
        }
        if( iflg == 5 ){ // 02
          v1 = -aH[0]/(aH[1]-aH[0])*aP[1] + aH[1]/(aH[1]-aH[0])*aP[0];
          v2 = -aH[2]/(aH[1]-aH[2])*aP[1] + aH[1]/(aH[1]-aH[2])*aP[2];
          iedge_next = 0;
        }
        if( iflg == 6 ){ // 12
          v1 = -aH[2]/(aH[0]-aH[2])*aP[0] + aH[0]/(aH[0]-aH[2])*aP[2];
          v2 = -aH[1]/(aH[0]-aH[1])*aP[0] + aH[0]/(aH[0]-aH[1])*aP[1];
          iedge_next = 2;
        }
        aCS_LoopXYZ[ihcs].push_back(v1);
        ////////////////
        int itri_next1 = aTriSur[jtri0*6+iedge_next*2+0];
        if( itri_next1 == -1 ){
          is_open = true;
          break;
        }
        int iseg_next1 = Tri2Seg[itri_next1];
        assert( iseg_next1 != -1 );
        if( iseg_next1 == iseg_ker ) break;
        iseg_next = iseg_next1;
      }
      if( is_open ){
        std::cout << "open : " << ih << " " << std::endl;
        const unsigned int nhcs = aCS_H.size();
        aCS_H.resize( nhcs-1 );
        aCS_LoopXYZ.resize( aCS_H.size() );
        aCS_IndTri.resize( aCS_H.size() );
        /////
        iseg_next = iseg_ker;
        for(;;){ // seg in cs loop
          int jtri0 = aCST[ih][iseg_next];
          unsigned int iflg = 0;
          CVector3 aP[3];
          double aH[3];
          for(unsigned int inotri=0;inotri<3;inotri++){
            unsigned int jno0 = aTri[jtri0*3+inotri];
            aP[inotri] = CVector3( aXYZ[jno0*3+0], aXYZ[jno0*3+1], aXYZ[jno0*3+2] );
            aH[inotri] = Dot(aP[inotri]-origin,norm)-aHeight[ih];
            if( aH[inotri] < 0 ){ continue; }
            if( inotri == 0 ){ iflg += 1; }
            if( inotri == 1 ){ iflg += 2; }
            if( inotri == 2 ){ iflg += 4; }
          }
          unsigned int iedge_next;
          if( iflg == 1 ){ iedge_next = 2; } // 0
          if( iflg == 2 ){ iedge_next = 0; } // 1
          if( iflg == 4 ){ iedge_next = 1; } // 2
          if( iflg == 3 ){ iedge_next = 0; } // 01
          if( iflg == 5 ){ iedge_next = 2; } // 02
          if( iflg == 6 ){ iedge_next = 1; } // 12
          int itri_next1 = aTriSur[jtri0*6+iedge_next*2+0];
          if( itri_next1 == -1 ){ break; } // reached boundary
          const int iseg_next1 = Tri2Seg[itri_next1];
          assert( iseg_next1 != -1 );
          if( iseg_next1 == iseg_ker ) break;
          iseg_next = iseg_next1;
          ////
          assert( aFlgSeg[iseg_next] == 0 );
          jtri0 = aCST[ih][iseg_next];
          aFlgSeg[iseg_next] = 1;
        }
      }
    }
    ////
    for(unsigned int isg=0;isg<aCST[ih].size();isg++){
      unsigned int itri = aCST[ih][isg];
      Tri2Seg[itri] = -1;
    }
  }
  
}


std::vector<double> aXYZ;
std::vector<unsigned int> aTri;
std::vector< std::vector<CVector3> > aCS_LoopXYZ;

CGlutWindowManager win;



//////////////////////////////////////////////////////////////

void myGlutIdle(){
  glutPostRedisplay();
}

void myGlutResize(int w, int h)
{
  glViewport(0, 0, w, h);
  ::glMatrixMode(GL_PROJECTION);
  glutPostRedisplay();
}

void myGlutMotion( int x, int y ){
  win.glutMotion(x,y);
}

void myGlutMouse(int button, int state, int x, int y){
  win.glutMouse(button, state, x, y);
}

void myGlutSpecial(int Key, int x, int y)
{
  win.glutSpecial(Key, x, y);
}

void myGlutDisplay(void)
{
  ::glClearColor(0.2f, .7f, .7f,1.0f);
  ::glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  ::glEnable(GL_DEPTH_TEST);
  
  ::glEnable(GL_POLYGON_OFFSET_FILL );
  ::glPolygonOffset( 1.1f, 4.0f );
  
  DrawBackground();
  win.SetGL_Camera();
  
  ::glEnable(GL_LIGHTING);
  ::DrawMeshTri3D_FaceNorm(aXYZ, aTri);
  
  ::glDisable(GL_LIGHTING);
  ::glColor3d(1,0,0);
  ::glLineWidth(5);
  for(int iloop=0;iloop<aCS_LoopXYZ.size();++iloop){
    const std::vector<CVector3>& aVec = aCS_LoopXYZ[iloop];
    ::glBegin(GL_LINE_LOOP);
    for(int iv=0;iv<aVec.size();++iv){
      ::glVertex3d(aVec[iv].x,aVec[iv].y,aVec[iv].z);
    }
    ::glEnd();
  }
  
  ShowFPS();
  glutSwapBuffers();
}

void myGlutKeyboard(unsigned char Key, int x, int y)
{
  switch(Key){
    case 'q':
    case 'Q':
    case '\033':
      exit(0);  /* '\033' ? ESC ? ASCII ??? */
    case 'a':
      break;
    case ' ':
      break;
    case 'l':
      break;
  }
  
  ::glutPostRedisplay();
}

int main(int argc,char* argv[])
{
  // initialize glut
  glutInitWindowPosition(200,200);
  glutInitWindowSize(400, 300);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
  glutCreateWindow("Surface Mesh Edge Collapse");
  
  // define call back functions
  glutIdleFunc(myGlutIdle);
  glutKeyboardFunc(myGlutKeyboard);
  glutDisplayFunc(myGlutDisplay);
  glutReshapeFunc(myGlutResize);
  glutSpecialFunc(myGlutSpecial);;
  glutMotionFunc(myGlutMotion);
  glutMouseFunc(myGlutMouse);
  
  setSomeLighting();
  
  win.camera.view_height = 0.5;
  win.camera.camera_rot_mode  = CAMERA_ROT_TBALL;
  
  Read_Ply(std::string(PATH_INPUT_DIR)+"/bunny_1k.ply",
           aXYZ,aTri);
  Normalize(aXYZ);
  std::vector<int> aTriSurRel;
  makeSurroundingRelationship(aTriSurRel, aTri.data(), aTri.size()/3, MESHELEM_TRI, aXYZ.size()/3);
  
 
  std::vector<double> aHeight;
  aHeight.push_back(-0.3);
  aHeight.push_back(-0.2);
  aHeight.push_back(-0.1);
  aHeight.push_back(-0.0);
  aHeight.push_back(+0.1);
  aHeight.push_back(+0.2);
  aHeight.push_back(+0.3);
  std::vector<unsigned int> aCS_H;
  std::vector< std::vector<unsigned int> > aCS_IndTri;
  CrossSections_Levels_MeshTri3D(aCS_H, aCS_IndTri, aCS_LoopXYZ,
                             aHeight,
                             CVector3(0,1,0), CVector3(0,0,0),
                             aXYZ,aTri,aTriSurRel);
  assert(aCS_H.size()==aCS_IndTri.size());
  assert(aCS_H.size()==aCS_LoopXYZ.size());
  
  glutMainLoop();
  return 0;
}