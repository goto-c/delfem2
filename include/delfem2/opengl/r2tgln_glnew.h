/*
 * Copyright (c) 2019 Nobuyuki Umetani
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */


#ifndef DFM2_RENDER2TEX_GLNEW_H
#define DFM2_RENDER2TEX_GLNEW_H

#include "delfem2/dfm2_inline.h"
#include <stdio.h>
#include <vector>
#include "delfem2/opengl/glnew_mshcolor.h"
#include "delfem2/opengl/r2t_gl.h"

namespace delfem2{
namespace opengl{

class CRender2Tex_DrawNewGL : public CRender2Tex
{
public:
  CRender2Tex_DrawNewGL(){
    pointSize = 3;
    isDrawTex = true;
    draw_len_axis = 1.0;
  }
  // --------------
  void Draw(float mP[16], float mMV[16]) const;
  // ------------
  virtual void InitGL(); // override function
  virtual void SetDepth();
public:
  bool isDrawTex;
  double draw_len_axis;
  unsigned int pointSize;
  CShader_LineMesh shdr0;
  CShader_TriMesh_Tex shdr1;
  CShader_Points shdr2;
};
  
}
}

#ifdef DFM2_HEADER_ONLY
#  include "delfem2/opengl/r2tgln_glnew.cpp"
#endif

#endif /* depth_hpp */
