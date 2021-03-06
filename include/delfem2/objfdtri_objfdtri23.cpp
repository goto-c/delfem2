/*
 * Copyright (c) 2019 Nobuyuki Umetani
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cstdio>
#include "delfem2/dtri2_v2dtri.h"
#include "delfem2/dtri3_v3dtri.h"
#include "delfem2/objf_geo3.h"
#include "delfem2/objfdtri_objfdtri23.h"

// -------------------------------------

namespace delfem2 {
namespace objfdtri {

static void FetchData
    (double *val_to,
     int nno, int ndim,
     const unsigned int *aIP,
     const double *val_from,
     int nstride) {
  assert(nstride >= ndim);
  for (int ino = 0; ino < nno; ++ino) {
    int ip = aIP[ino];
    for (int idim = 0; idim < ndim; ++idim) {
      val_to[ino * ndim + idim] = val_from[ip * nstride + idim];
    }
  }
}

}
}

void delfem2::PBD_TriStrain
(double* aXYZt,
 unsigned int nXYZ,
 const std::vector<delfem2::CDynTri>& aETri,
 const std::vector<CVec2d>& aVec2)
{
  for(const auto & it : aETri){
    const unsigned int aIP[3] = {
      it.v[0],
      it.v[1],
      it.v[2]};
    const double P[3][2] = {
      {aVec2[aIP[0]].x(),aVec2[aIP[0]].y()},
      {aVec2[aIP[1]].x(),aVec2[aIP[1]].y()},
      {aVec2[aIP[2]].x(),aVec2[aIP[2]].y()} };
    double p[3][3]; objfdtri::FetchData(&p[0][0], 3, 3, aIP, aXYZt, 3);
    double C[3], dCdp[3][9];  PBD_CdC_TriStrain2D3D(C, dCdp, P, p);
    double m[3] = {1,1,1};
    PBD_Update_Const3(aXYZt,
                      3, 3, m, C, &dCdp[0][0], aIP, 1.0);
  }
}

void delfem2::PBD_Bend
(double* aXYZt,
 unsigned int nXYZ,
 const std::vector<delfem2::CDynTri>& aETri,
 const std::vector<CVec2d>& aVec2,
 double ratio)
{
  for(size_t it=0;it<aETri.size();++it){
    for(int ie=0;ie<3;++ie){
      const int jt0 = aETri[it].s2[ie];
      if( jt0 == -1 ){ continue; }
      if( jt0 > (int)it ){ continue; }
      const int je0 = FindAdjEdgeIndex(aETri[it],ie,aETri);
      assert( aETri[jt0].s2[je0] == it);
      const unsigned int aIP[4] = {
        aETri[it].v[ie],
        aETri[jt0].v[je0],
        aETri[it].v[(ie+1)%3],
        aETri[it].v[(ie+2)%3] };
      const double P[4][3] = {
        {aVec2[aIP[0]].x(),aVec2[aIP[0]].y(), 0.0},
        {aVec2[aIP[1]].x(),aVec2[aIP[1]].y(), 0.0},
        {aVec2[aIP[2]].x(),aVec2[aIP[2]].y(), 0.0},
        {aVec2[aIP[3]].x(),aVec2[aIP[3]].y(), 0.0} };
      double p[4][3]; objfdtri::FetchData(&p[0][0], 4, 3, aIP, aXYZt, 3);
      double C[3], dCdp[3][12];
      PBD_CdC_QuadBend(C, dCdp,
                       P, p);
      double m[4] = {1,1,1,1};
      PBD_Update_Const3(aXYZt,
                        4,3, m, C, &dCdp[0][0], aIP, ratio);
    }
  }
}
