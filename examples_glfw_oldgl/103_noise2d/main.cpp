/*
 * Copyright (c) 2019 Nobuyuki Umetani
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <limits>
#include <assert.h>
#include <vector>
#include "delfem2/noise.h"
#include "delfem2/camera.h"

#include <GLFW/glfw3.h>
#include "delfem2/opengl/glfw_viewer.hpp"
#include "delfem2/opengl/gl2_funcs.h"
#include "delfem2/opengl/gl2_color.h"

namespace dfm2 = delfem2;

// ---------------------

std::vector<int> aP;
std::vector<double> aGrad;
int nH, nW;
std::vector<double> aV;

// ---------------------

void ComputePerlin(){
  
  aP.resize(256);
  for(int i=0;i<256;++i){ aP[i]=i; }
  Shuffle(aP);
  aP.resize(512);
  for(int i=0;i<256;++i){ aP[256+i]=i; }
  
  aGrad.clear();
  
  aGrad.push_back(-1); aGrad.push_back(-1);
  aGrad.push_back(-1); aGrad.push_back(+1);
  aGrad.push_back(+1); aGrad.push_back(-1);
  aGrad.push_back(+1); aGrad.push_back(+1);
  
  
  nH = 256;
  nW = 256;
  aV.resize(nH*nW);
  int nrep = 8;
  for(int ih=0;ih<nH;++ih){
    for(int iw=0;iw<nW;++iw){
      double x = (double)iw/(nW)*nrep;
      double y = (double)ih/(nH)*nrep;
      aV[ih*nW+iw] = noise_perlin_2d_oct(x,y,nrep, 4, 0.9, aGrad,aP);
    }
  }
}

int main(int argc,char* argv[])
{
  CViewer_GLFW viewer;
  viewer.Init_GLold();
  
  while (!glfwWindowShouldClose(viewer.window))
  {
    {
      static int iframe = 0;
      if( iframe == 0 ){
        ComputePerlin();
      }
      iframe = (iframe + 1) % 30;
    }
    
    viewer.DrawBegin_Glold();
    
    ::glColor3d(1,1,1);
    ::glBegin(GL_LINE_LOOP);
    ::glVertex2d(-0.5, -0.5);
    ::glVertex2d(+0.5, -0.5);
    ::glVertex2d(+0.5, +0.5);
    ::glVertex2d(-0.5, +0.5);
    ::glEnd();
    
    std::vector<std::pair<double, dfm2::CColor> > colorMap;
    makeHeatMap_BlueCyanGreenYellowRed(colorMap, -0.5, +0.5);
      //  makeHeatMap_BlueGrayRed(colorMap, -0.8, +0.8);
    ::glBegin(GL_QUADS);
    for(int jh=0;jh<nH-1;++jh){
      for(int jw=0;jw<nW-1;++jw){
        int i00 = (jh+0)*nW+(jw+0);
        int i10 = (jh+0)*nW+(jw+1);
        int i11 = (jh+1)*nW+(jw+1);
        int i01 = (jh+1)*nW+(jw+0);
        double v00 = aV[i00];
        double v10 = aV[i10];
        double v11 = aV[i11];
        double v01 = aV[i01];
        double x0 = -0.5+1.0/(nW-1)*(jw+0);
        double x1 = -0.5+1.0/(nW-1)*(jw+1);
        double y0 = -0.5+1.0/(nH-1)*(jh+0);
        double y1 = -0.5+1.0/(nH-1)*(jh+1);
        dfm2::opengl::heatmap(v00, colorMap); ::glVertex2d(x0,y0);
        dfm2::opengl::heatmap(v10, colorMap); ::glVertex2d(x1,y0);
        dfm2::opengl::heatmap(v11, colorMap); ::glVertex2d(x1,y1);
        dfm2::opengl::heatmap(v01, colorMap); ::glVertex2d(x0,y1);
      }
    }
    ::glEnd();
    
    ::glPointSize(5);
    
      // yerrow: input
    ::glLineWidth(1);
    ::glPointSize(5);
    ::glColor3d(1,1,0);
    
      // magenda: last
    ::glLineWidth(1);
    ::glPointSize(5);
    ::glColor3d(1,0,1);
    
    glfwSwapBuffers(viewer.window);
    glfwPollEvents();
  }
  glfwDestroyWindow(viewer.window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
