/*
 * Copyright (c) 2019 Nobuyuki Umetani
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DFM2_OPENGL_GLOLD_VOX
#define DFM2_OPENGL_GLOLD_VOX

#include <stdio.h>
#include "delfem2/dfm2_inline.h"
#include "delfem2/gridcube.h"
#include "delfem2/primitive.h"
#include "delfem2/bv.h"

namespace delfem2 {
namespace opengl {

void Draw_CubeGrid(bool is_picked, int iface_picked,
                   double elen, const CVec3d &org,
                   const CCubeGrid &cube);

}
}

#ifdef DFM2_HEADER_ONLY
#  include "delfem2/opengl/gridcube_glold.cpp"
#endif

#endif /* gl_voxsdf_hpp */
