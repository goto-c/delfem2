/*
 * Copyright (c) 2019 Nobuyuki Umetani
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <vector>
#include <string>
#include <cassert>
#include <cstdlib>
#include "delfem2/mshmisc.h"
#include "delfem2/mshio.h"
#include "delfem2/dtri3_v3dtri.h"

#include <GLFW/glfw3.h>
#include "delfem2/opengl/v3q_glold.h"
#include "delfem2/opengl/glfw/viewer_glfw.h"

namespace dfm2 = delfem2;

// -----------------------------

void myGlutDisplay
 (const std::vector<dfm2::CDynPntSur>& aPo,
  const std::vector<dfm2::CDynTri>& aTri,
  const std::vector<dfm2::CVec3d>& aVec3)
{
  GLboolean is_lighting = ::glIsEnabled(GL_LIGHTING);
  ::glEnable(GL_LIGHTING);
  GLboolean is_texture  = ::glIsEnabled(GL_TEXTURE_2D);
  ::glDisable(GL_TEXTURE_2D);    
  {
    //    float gray[4] = {0.3,0.3,0.3,1};
    float gray[4] = {0.9f,0.9f,0.9f,1.f};    
    ::glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, gray);
    float shine[4] = {0,0,0,0};
    ::glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, shine);
    ::glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127.0);
    //    ::glColor3d(1,1,1);
  }
  
  ::glDisable(GL_LIGHTING);
  ::glColor3d(1,1,1);
  ::glBegin(GL_TRIANGLES);
  for(const auto & tri : aTri){
    const unsigned int i1 = tri.v[0];
    const unsigned int i2 = tri.v[1];
    const unsigned int i3 = tri.v[2];
    dfm2::opengl::myGlVertex(aVec3[i1]);
    dfm2::opengl::myGlVertex(aVec3[i2]);
    dfm2::opengl::myGlVertex(aVec3[i3]);
  }
  ::glEnd();        
  
  ::glColor3d(0,0,0);
  ::glBegin(GL_LINES);
  for(const auto & tri : aTri){
    const unsigned int i1 = tri.v[0];
    const unsigned int i2 = tri.v[1];
    const unsigned int i3 = tri.v[2];
    dfm2::opengl::myGlVertex(aVec3[i1]);
    dfm2::opengl::myGlVertex(aVec3[i2]);
    dfm2::opengl::myGlVertex(aVec3[i2]);
    dfm2::opengl::myGlVertex(aVec3[i3]);
    dfm2::opengl::myGlVertex(aVec3[i3]);
    dfm2::opengl::myGlVertex(aVec3[i1]);
  }
  ::glEnd();
  // ----------------
  if( is_lighting ){ ::glEnable( GL_LIGHTING); }
  else{              ::glDisable(GL_LIGHTING); }
  if(  is_texture  ){ ::glEnable(GL_TEXTURE_2D); }
}

int main(int argc,char* argv[])
{
  std::vector<dfm2::CDynPntSur> aPo;
  std::vector<dfm2::CDynTri> aTri;
  std::vector<dfm2::CVec3d> aVec3;
  {
    std::vector<double> aXYZ0;
    std::vector<unsigned int> aTri0;
    delfem2::Read_Ply(std::string(PATH_INPUT_DIR)+"/arm_16k.ply",
                      aXYZ0,aTri0);
    dfm2::Normalize_Points3(aXYZ0,2.0);
    const unsigned int np = aXYZ0.size()/3;
    aPo.resize(np);
    aVec3.resize(np);
    for(unsigned int ipo=0;ipo<aPo.size();ipo++){
      aVec3[ipo].p[0] = aXYZ0[ipo*3+0];
      aVec3[ipo].p[1] = aXYZ0[ipo*3+1];
      aVec3[ipo].p[2] = aXYZ0[ipo*3+2];
    }
    InitializeMesh(aPo, aTri,
                   aTri0.data(),aTri0.size()/3,aVec3.size());
    AssertDTri(aTri);
    AssertMeshDTri(aPo, aTri);
    AssertMeshDTri2(aPo, aTri, aVec3);
  }
  // -----------
  delfem2::opengl::CViewer_GLFW viewer;
  viewer.Init_oldGL();
  viewer.nav.camera.view_height = 1.5;
  while (!glfwWindowShouldClose(viewer.window))
  {
    for(unsigned int i=0;i<10;i++){
      auto itri0 = (unsigned int)((rand()/(RAND_MAX+1.0))*aTri.size());
      assert( itri0 < aTri.size() );
      CollapseEdge_MeshDTri(itri0, 0, aPo, aTri);
      AssertDTri(aTri);
      AssertMeshDTri(aPo, aTri);
      if( aTri.size() <= 100 ) break;
    }
    viewer.DrawBegin_oldGL();
    myGlutDisplay(aPo,aTri,aVec3);
    viewer.DrawEnd_oldGL();
  }
  glfwDestroyWindow(viewer.window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
