/*
 * Copyright (c) 2019 Nobuyuki Umetani
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */


#ifndef DFM2_RIG_V3Q_H
#define DFM2_RIG_V3Q_H

#include "delfem2/dfm2_inline.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "delfem2/gizmo_geo3.h"

namespace delfem2 {

/**
 * @brief Set 3D affine matrix that transfrom from intial position from the deformed poisition for each bones.
 * @details this funcition is for rigging without the class "CRigBone"
 */
DFM2_INLINE void SetMat4AffineBone_FromJointRelativeRotation(
    std::vector<double>& aMat4AffineBone,
    const double trans_root[3],
    const std::vector<double>& aQuatRelativeRot,
    const std::vector<int>& aIndBoneParent,
    const std::vector<double>& aJntPos0);

// ----------------------------------------------
// CRigBone

/**
 *@brief articulated rigid body for character rigging
 *@details this class support rig in GlTF and BioVision BVH
 */
class CRigBone
{
public:
    CRigBone(){
      for(int i=0;i<16;++i){ invBindMat[i]=0.0; }
      invBindMat[ 0] = 1.0;
      invBindMat[ 5] = 1.0;
      invBindMat[10] = 1.0;
      invBindMat[15] = 1.0;
      //
      scale = 1;
      quatRelativeRot[0] = 1;
      quatRelativeRot[1] = 0;
      quatRelativeRot[2] = 0;
      quatRelativeRot[3] = 0;
      transRelative[0] = 0;
      transRelative[1] = 0;
      transRelative[2] = 0;
      ibone_parent = -1;
    }
  delfem2::CVec3d Pos() const{
    return delfem2::CVec3d(affmat3Global[3],affmat3Global[7],affmat3Global[11]);
  }
  void DeformSkin(double pos2[3],
                  const double pos0[3]) const;
  void SetRotationBryant(double rx, double ry, double rz);
  void SetTranslation(double tx, double ty, double tz);
  /*
  int PickHandler(const delfem2::CVec3d& org,
                  const delfem2::CVec3d& dir,
                  double rad_handlr,
                  double tol) const;
   */
  void AffineJoint(const double a[16]) const;
public:
  std::string name; // initialized and stay constant
  
  /** 
   * @details Inverse of Affine matrix to send the skin to the bone reference config. The joint position of this bone will be mapped to the origin
   */
  double invBindMat[16];
  
  int ibone_parent; // initialized and stay constant
  
  double transRelative[3]; // position of the joint position from parent joint
  
  double scale; // scale
  
  /**
   * @brief rotation of the joint from parent joint (quaternion w,x,y,z).
   * @details this value will be changed  when pose is edited
   */
  double quatRelativeRot[4];

  /**
   * @brief affine matrix  to send bone from the origin to the deformed pose
   * @details this value will be set when the pose is edited using the function
   * "void UpdateBoneRotTrans(std::vector<CRigBone>& aBone)"
   */
  double affmat3Global[16];
};

/**
 * @brief set "CRgidiBone.affmat3Global" based on "CRigidBone.quadRelativeRot"
 */
DFM2_INLINE void UpdateBoneRotTrans(std::vector<CRigBone>& aBone);

DFM2_INLINE void PickBone(
    int& ibone_selected,
    int& ielem_selected,
    const std::vector<CRigBone>& aBone,
    const delfem2::CVec3d& src,
    const delfem2::CVec3d& dir,
    double rad_hndlr,
    double tol);


// ----------------------------------

DFM2_INLINE void Skinning_LBS_LocalWeight(
    double* aXYZ,
    const double* aXYZ0,
    unsigned int nXYZ,
    const unsigned int* aTri,
    unsigned int nTri,
    const std::vector<CRigBone>& aBone,
    const double* aRigWeight,
    const unsigned int* aRigJoint);

/**
 * @params aW rigging weight [np, nbone]
 */
DFM2_INLINE void Skinning_LBS(
    std::vector<double>& aXYZ1,
    const std::vector<double>& aXYZ0,
    const std::vector<CRigBone>& aBone,
    const std::vector<double>& aW);


// --------------------------------------

DFM2_INLINE void Smpl2Rig(
    std::vector<CRigBone>& aBone,
    const std::vector<int>& aIndBoneParent,
    const std::vector<double>& aXYZ0,
    const std::vector<double>& aJntRgrs);

// --------------------------------------

DFM2_INLINE void Rig_SkinReferncePositionsBoneWeighted(
    std::vector<double>& aRefPosAff,
    const std::vector<CRigBone> aBone1,
    const std::vector<double>& aXYZ0,
    const std::vector<double>& aW);

// ------------------------------------



class CTarget
{
public:
  unsigned int ib;
  CVec3d pos;
public:
  void WdW
  (std::vector<double>& aW,
   std::vector<double>& adW,
   const std::vector<CRigBone>& aBone,
   std::vector<double>& aL) const; // [ [nb, 3],  [ndim(3), nBone, ndim(4)] ]
};

DFM2_INLINE void Rig_SensitivityBoneTransform(double* aL, // [ ndim(3), nBone, ndim(4) ]
                                  unsigned int ib_s,
                                  unsigned int idim_s,
                                  const std::vector<CRigBone> aBone1);

DFM2_INLINE void Rig_SensitivityBoneTransform_Eigen(
    std::vector<double>& Lx, // [ [nBone, ndim(3)], [nBone, ndim(4)] ]
    std::vector<double>& Ly, // [ [nBone, ndim(3)], [nBone, ndim(4)] ]
    std::vector<double>& Lz, // [ [nBone, ndim(3)], [nBone, ndim(4)] ]
    unsigned int ib_s,
    double idim_s,
    bool is_rot,
    const std::vector<CRigBone> aBone1);

DFM2_INLINE void Rig_WdW_Target_Eigen(
    std::vector<double>& aW,
    std::vector<double>& adW,
    const std::vector<CRigBone>& aBone,
    const CTarget& target,
    const std::vector<double>& Lx,  // [ nsns, nBone*4 ]
    const std::vector<double>& Ly,  // [ nsns, nBone*4 ]
    const std::vector<double>& Lz); // [ nsns, nBone*4 ]

DFM2_INLINE void Rig_SensitivitySkin_BoneRotation_Eigen(
    std::vector<double>& dSkinX,
    std::vector<double>& dSkinY,
    std::vector<double>& dSkinZ,
    const std::vector<CRigBone>& aBone1,
    const std::vector<double>& aXYZ0,
    const std::vector<double>& aW,
    const std::vector<double>& Lx, // [ nBone*3, nBone*4 ]
    const std::vector<double>& Ly, // [ nBone*3, nBone*4 ]
    const std::vector<double>& Lz); // [ nBone*3, nBone*4 ]

DFM2_INLINE void Rig_SensitivitySkin_BoneRotation(
    std::vector<double>& aSns, // np*ndim(3) * nb*3
    const std::vector<CRigBone> aBone1,
    const std::vector<double>& aXYZ0,
    const std::vector<double>& aW,
    const std::vector<double>& aL);


// ------------------------------------

class CChannel_BioVisionHierarchy
{
public:
  CChannel_BioVisionHierarchy(int ib, int ia, bool br){
    this->ibone = ib;
    this->iaxis = ia;
    this->isrot = br;
  }
public:
  int ibone;
  int iaxis;
  bool isrot;
};

DFM2_INLINE void Read_BioVisionHierarchy(
    std::vector<CRigBone>& aBone,
    std::vector<CChannel_BioVisionHierarchy>& aChannelInfo,
    int& nframe,
    std::vector<double>& aChannelValue,
    const std::string& path_bvh);

/**
 * @brief set value to CRigBone.rot (bone rotation from parent bone)
 */
DFM2_INLINE void SetPose_BioVisionHierarchy(
    std::vector<CRigBone>& aBone,
    const std::vector<CChannel_BioVisionHierarchy>& aChannelInfo,
    const double *aVal);


/**
 * @brief gizmo for rig
 */
template <typename REAL>
class CGizmo_Rig {
public:
  enum MODE_EDIT { ROT, TRNSL };
public:
  void Pick(const float src[3], const float dir[3],
            const std::vector<CRigBone>& aBone)
  {
    if( mode_edit == ROT ){
      const CVec3d s0(src), d0(dir);
      if( ipicked_bone != -1 ){
        gizmo_rot.Pick(true, src, dir, 0.03);
        if( gizmo_rot.ielem_picked != -1 ){
          return;
        }
      }
      ipicked_bone = -1;
      for(unsigned int ib=0;ib<aBone.size();++ib){
        CVec3d p0 = aBone[ib].Pos();
        CVec3d p1 = nearest_Line_Point(p0, s0, d0);
        double len01 = (p0-p1).Length();
        if( len01 < 0.03 ){
          ipicked_bone = ib;
          gizmo_rot.quat[0] = aBone[ipicked_bone].quatRelativeRot[0];
          gizmo_rot.quat[1] = aBone[ipicked_bone].quatRelativeRot[1];
          gizmo_rot.quat[2] = aBone[ipicked_bone].quatRelativeRot[2];
          gizmo_rot.quat[3] = aBone[ipicked_bone].quatRelativeRot[3];
        }
      }
    }
    else if( mode_edit == TRNSL ){
      gizmo_trnsl.pos[0] = aBone[0].transRelative[0];
      gizmo_trnsl.pos[1] = aBone[0].transRelative[1];
      gizmo_trnsl.pos[2] = aBone[0].transRelative[2];
      gizmo_trnsl.Pick(true, src, dir, 0.03 );
    }
  }
  bool Drag(std::vector<CRigBone>& aBone,
            const float src0[3], const float src1[3], const float dir[3])
  {
    if( mode_edit == ROT ){
      if( ipicked_bone == -1 ){ return false; }
      assert( ipicked_bone >= 0 && ipicked_bone < (int)aBone.size() );
      gizmo_rot.pos = aBone[ipicked_bone].Pos().Float();
      gizmo_rot.Drag(src0, src1, dir);
      {
        const int ibp = aBone[ipicked_bone].ibone_parent;
        CMat3d m3;
        if( ibp == -1 ){ m3.SetIdentity(); }
        else{ m3.SetMat4(aBone[ibp].affmat3Global); }
        CQuatd qp; m3.GetQuat_RotMatrix(qp.q);
        CQuatd qg = CQuatf(gizmo_rot.quat).Double();
        CQuatd qj = qp.Conjugate()*qg;
        qj.CopyTo(aBone[ipicked_bone].quatRelativeRot);
      }
      UpdateBoneRotTrans(aBone);
      return true;
    }
    else if( mode_edit == TRNSL ){
      gizmo_trnsl.Drag(src0, src1, dir);
      aBone[0].transRelative[0] = gizmo_trnsl.pos.x();
      aBone[0].transRelative[1] = gizmo_trnsl.pos.y();
      aBone[0].transRelative[2] = gizmo_trnsl.pos.z();
      UpdateBoneRotTrans(aBone);
      return true;
    }
    return false;
  }
  // --------
  void SetSize(double size){
    gizmo_rot.size = size;
    gizmo_trnsl.size = size;
  }
  void SetMode(MODE_EDIT mode){ mode_edit = mode; }
public:
  MODE_EDIT mode_edit = ROT; // 0:rot, 1:trnsl
  int ipicked_bone = -1;
  CGizmo_Transl<REAL> gizmo_trnsl;
  CGizmo_Rotation<REAL> gizmo_rot;
};

} // namespace delfem2

#ifdef DFM2_HEADER_ONLY
#  include "delfem2/rig_geo3.cpp"
#endif

#endif // #define DFM2_RIG_V3Q_H

