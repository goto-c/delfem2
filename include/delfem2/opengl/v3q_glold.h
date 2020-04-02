/*
 * Copyright (c) 2019 Nobuyuki Umetani
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * @discussion It might be nice to separate dependency of v2 from v3 and quat
 * because 2D application does not use v3 and quat
 */

#ifndef DFM2_V3Q_GLOLD_H
#define DFM2_V3Q_GLOLD_H

#include <vector>
#include "delfem2/vec3.h"
#include "delfem2/quat.h"

namespace delfem2{
namespace opengl
{

// ------------------------------------------------------------------------------------
// vec3 starts here

void myGlVertex(const CVec3d& v);
void myGlTranslate(const CVec3d& v);
void myGlNormal(const CVec3d& n);
void myGlNormal(const CVec3d& a, const CVec3d& b, const CVec3d& c);
void myGlVertex(int i, const std::vector<CVec3d>& aV);
void myGlVertex3(unsigned int i, const std::vector<double>& vec);
void ModelTransformation(const CVec3d& dx, const CVec3d& dz, const CVec3d& origin);
void ViewTransformation(const CVec3d& dx, const CVec3d& dz, const CVec3d& origin);

// -------------------------------------

void DrawArcSolid(const CVec3d& axis,
                  const CVec3d& org,
                  double ru, // rin
                  double rv, // rout
                  double rads,
                  double rade);
void DrawArrow(const CVec3d& p0,
               const CVec3d& d,
               int ndivt=16);
void DrawCircleArrow(const CVec3d& org, CVec3d axis, double offset);
void DrawCylinder(const CVec3d& p0,
                  const CVec3d& p1,
                  double r);
void DrawCylinderWire(const CVec3d& p0,
                      const CVec3d& p1,
                      double r);
void DrawSingleQuad_FaceNorm(const CVec3d& p0,
                             const CVec3d& p1,
                             const CVec3d& p2,
                             const CVec3d& p3);
void DrawSingleQuad_Edge(const CVec3d& p0,
                         const CVec3d& p1,
                         const CVec3d& p2,
                         const CVec3d& p3);
void DrawSingleHex_Edge(const CVec3d& p0,
                        const CVec3d& p1,
                        const CVec3d& p2,
                        const CVec3d& p3,
                        const CVec3d& p4,
                        const CVec3d& p5,
                        const CVec3d& p6,
                        const CVec3d& p7);
void drawPolyLine3D(const std::vector<CVec3d>& aP);

void DrawCircleWire(const CVec3d& axis,
                    const CVec3d& org,
                    double r);
void DrawCircleSolid(const CVec3d& axis,
                     const CVec3d& org,
                     double r);
void DrawGrid2D(int ndivx, int ndivy,
                const CVec3d& ex, const CVec3d& ey, const CVec3d& org);
void DrawGridOutside(int ndivx, int ndivy, int ndivz,
                     double elen,
                     const CVec3d& org);
  
void DrawAxisHandler(double s, const CVec3d& p);

// ------------
// mesh from here
void DrawPoint3D(const std::vector<CVec3d>& aPoint);
void DrawMeshQuad_Face(const std::vector<CVec3d>& aPoint,
                       const std::vector<unsigned int>& aQuad);
void DrawMeshTri_Edge(const std::vector<CVec3d>& aP,
                      const std::vector<unsigned int>& aTri);
void DrawTriMeshNorm(const std::vector<CVec3d>& aP,
                     const std::vector<int>& aTri);
void DrawMeshTri_Edge(const std::vector<CVec3d>& aP,
                      const std::vector<unsigned int>& aTri);
void DrawQuad3D_Edge(const std::vector<CVec3d>& aPoint,
                     const std::vector<unsigned int>& aQuad);

// CVec3
// --------------------------------------------------------------------
// CMat4

void DrawHandlerRotation_Mat4
 (const double Mat[16],
  double size, int ielem_picked);

// CMat4
// --------------------------------------------------------------------
// CQuaternion

void DrawHandlerRotation_PosQuat
 (const CVec3d& pos, const double quat[4],
  double size, int ielem_picked);


void Draw_QuaternionsCoordinateAxes(
    const std::vector<double>& aXYZ1,
    const std::vector<double>& aQuat,
    double l);


  
} // end namespace opengl
} // end namespace delfem2

#endif