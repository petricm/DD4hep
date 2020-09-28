//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/DD4hepUnits.h"

#ifdef HAVE_GEANT4_UNITS
#define MM_2_CM 1.0
#else
#define MM_2_CM 0.1
#endif

// ROOT includes
#include "TGeoMatrix.h"

auto dd4hep::detail::matrix::_identity() -> TGeoIdentity* {
  return gGeoIdentity;
}

auto dd4hep::detail::matrix::_scale(const TGeoMatrix* matrix) -> ROOT::Math::XYZVector {
  if ( matrix->IsScale() )   {
    const Double_t* mat_scale = matrix->GetScale();
    return ROOT::Math::XYZVector(mat_scale[0],mat_scale[1],mat_scale[2]);
  }
  return ROOT::Math::XYZVector(1e0,1e0,1e0);
}

auto dd4hep::detail::matrix::_scale(const TGeoMatrix& matrix) -> ROOT::Math::XYZVector {
  return _scale(&matrix);
}

auto dd4hep::detail::matrix::_translation(const TGeoMatrix* matrix) -> dd4hep::Position {
  if ( matrix->IsTranslation() )   {
    const Double_t* trans = matrix->GetTranslation();
    return Position(trans[0]*MM_2_CM,trans[1]*MM_2_CM,trans[2]*MM_2_CM);
  }
  return Position(0e0,0e0,0e0);
}

auto dd4hep::detail::matrix::_translation(const TGeoMatrix& matrix) -> dd4hep::Position {
  return _translation(&matrix);
}

auto dd4hep::detail::matrix::_translation(const Position& pos) -> TGeoTranslation* {
  return new TGeoTranslation("", pos.X(), pos.Y(), pos.Z());
}

auto dd4hep::detail::matrix::_rotationZYX(const RotationZYX& rot) -> TGeoRotation* {
  return new TGeoRotation("", rot.Phi() * RAD_2_DEGREE, rot.Theta() * RAD_2_DEGREE, rot.Psi() * RAD_2_DEGREE);
}

auto dd4hep::detail::matrix::_rotation3D(const Rotation3D& rot3D) -> TGeoRotation* {
  EulerAngles rot(rot3D);
  return new TGeoRotation("", rot.Phi() * RAD_2_DEGREE, rot.Theta() * RAD_2_DEGREE, rot.Psi() * RAD_2_DEGREE);
}

/// Extract the rotational part of a TGeoMatrix as a Rotation3D                           \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_rotation3D(const TGeoMatrix* matrix) -> dd4hep::Rotation3D   {
  if ( matrix->IsRotation() )  {
    const Double_t* rot = matrix->GetRotationMatrix();
    return Rotation3D(rot[0],rot[1],rot[2],
                      rot[3],rot[4],rot[5],
                      rot[6],rot[7],rot[8]);
  }
  return Rotation3D(0e0,0e0,0e0,
                    0e0,0e0,0e0,
                    0e0,0e0,0e0);
}

/// Extract the rotational part of a TGeoMatrix as a Rotation3D                           \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_rotation3D(const TGeoMatrix& matrix) -> dd4hep::Rotation3D   {
  return _rotation3D(&matrix);
}

/// Set a RotationZYX object to a TGeoHMatrix            \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_transform(TGeoHMatrix& tr, const RotationZYX& rot) -> TGeoHMatrix&   {
  tr.RotateZ(rot.Phi()   * RAD_2_DEGREE);
  tr.RotateY(rot.Theta() * RAD_2_DEGREE);
  tr.RotateX(rot.Psi()   * RAD_2_DEGREE);
  return tr;
}

/// Set a Position object (translation) to a TGeoHMatrix \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_transform(TGeoHMatrix& tr, const Position& pos) -> TGeoHMatrix&   {
  double t[3];
  pos.GetCoordinates(t);
  tr.SetDx(t[0]);
  tr.SetDy(t[1]);
  tr.SetDz(t[2]);
  return tr;
}

/// Set a Rotation3D object to a TGeoHMatrix           \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_transform(TGeoHMatrix& tr, const Rotation3D& rot) -> TGeoHMatrix&   {
  Double_t* r = tr.GetRotationMatrix();
  rot.GetComponents(r);
  return tr;
}

/// Set a Transform3D object to a TGeoHMatrix            \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_transform(TGeoHMatrix& tr, const Transform3D& trans) -> TGeoHMatrix& {
  Position pos;
  RotationZYX rot;
  trans.GetDecomposition(rot, pos);
  return _transform(tr, pos, rot);
}

/// Set a Position followed by a RotationZYX to a TGeoHMatrix  \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_transform(TGeoHMatrix& tr, const Position& pos, const RotationZYX& rot) -> TGeoHMatrix& {
  return _transform(_transform(tr, rot), pos);
}

/// Convert a Position object to a TGeoTranslation         \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_transform(const Position& pos) -> TGeoHMatrix*   {
  return &_transform(*(new TGeoHMatrix()), pos);
}

/// Convert a RotationZYX object to a TGeoHMatrix          \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_transform(const RotationZYX& rot) -> TGeoHMatrix*   {
  return &_transform(*(new TGeoHMatrix()), rot);
}

/// Convert a Rotation3D object to a TGeoHMatrix           \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_transform(const Rotation3D& rot) -> TGeoHMatrix*   {
  return &_transform(*(new TGeoHMatrix()), rot);
}

/// Convert a Transform3D object to a TGeoHMatrix          \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_transform(const Transform3D& trans) -> TGeoHMatrix* {
  return &_transform(*(new TGeoHMatrix()), trans);
}

/// Convert a Position followed by a RotationZYX to a TGeoHMatrix  \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_transform(const Position& pos, const RotationZYX& rot) -> TGeoHMatrix* {
  return &_transform(*(new TGeoHMatrix()), pos, rot);
}

/// Convert a TGeoMatrix object to a generic Transform3D  \ingroup DD4HEP \ingroup DD4HEP_CORE
auto dd4hep::detail::matrix::_transform(const TGeoMatrix* matrix) -> dd4hep::Transform3D    {
  const Double_t* t = matrix->GetTranslation();
  if ( matrix->IsRotation() )  {
    const Double_t* rot = matrix->GetRotationMatrix();
    return Transform3D(rot[0],rot[1],rot[2],t[0]*MM_2_CM,
                       rot[3],rot[4],rot[5],t[1]*MM_2_CM,
                       rot[6],rot[7],rot[8],t[2]*MM_2_CM);
  }
  return Transform3D(0e0,0e0,0e0,t[0]*MM_2_CM,
                     0e0,0e0,0e0,t[1]*MM_2_CM,
                     0e0,0e0,0e0,t[2]*MM_2_CM);
}

// add another implementation that takes const reference
auto dd4hep::detail::matrix::_transform(const TGeoMatrix& matrix) -> dd4hep::Transform3D    {
  const Double_t* t = matrix.GetTranslation();
  if ( matrix.IsRotation() )  {
    const Double_t* rot = matrix.GetRotationMatrix();
    return Transform3D(rot[0],rot[1],rot[2],t[0]*MM_2_CM,
                       rot[3],rot[4],rot[5],t[1]*MM_2_CM,
                       rot[6],rot[7],rot[8],t[2]*MM_2_CM);
  }
  return Transform3D(0e0,0e0,0e0,t[0]*MM_2_CM,
                     0e0,0e0,0e0,t[1]*MM_2_CM,
                     0e0,0e0,0e0,t[2]*MM_2_CM);
}

auto dd4hep::detail::matrix::_xyzAngles(const TGeoMatrix* matrix) -> dd4hep::XYZAngles {
  return matrix->IsRotation() ? _xyzAngles(matrix->GetRotationMatrix()) : XYZAngles(0,0,0);
}

auto dd4hep::detail::matrix::_xyzAngles(const double* rot) -> dd4hep::XYZAngles {
  Double_t cosb = std::sqrt(rot[0]*rot[0] + rot[1]*rot[1]);
  if (cosb > 0.00001) {
    return XYZAngles(atan2(rot[5], rot[8]), atan2(-rot[2], cosb), atan2(rot[1], rot[0]));
  }
  return XYZAngles(atan2(-rot[7], rot[4]),atan2(-rot[2], cosb),0);
}

void dd4hep::detail::matrix::_decompose(const TGeoMatrix& trafo, Position& pos, Rotation3D& rot)  {
  _decompose(_transform(trafo), pos, rot);
}

void dd4hep::detail::matrix::_decompose(const TGeoMatrix& trafo, Position& pos, RotationZYX& rot)   {
  _decompose(_transform(trafo), pos, rot);
}

void dd4hep::detail::matrix::_decompose(const TGeoMatrix& trafo, Position& pos, XYZAngles& rot)   {
  _decompose(_transform(trafo), pos, rot);
}

void dd4hep::detail::matrix::_decompose(const Transform3D& trafo, Position& pos, Rotation3D& rot)  {
  trafo.GetDecomposition(rot, pos);  
}

void dd4hep::detail::matrix::_decompose(const Transform3D& trafo, Translation3D& pos, RotationZYX& rot)   {
  trafo.GetDecomposition(rot, pos);
}

void dd4hep::detail::matrix::_decompose(const Transform3D& trafo, Translation3D& pos, XYZAngles& rot)   {
  EulerAngles r;
  trafo.GetDecomposition(r,pos);
  rot.SetXYZ(r.Psi(),r.Theta(),r.Phi());
}

void dd4hep::detail::matrix::_decompose(const Transform3D& trafo, Position& pos, RotationZYX& rot)  {
  trafo.GetDecomposition(rot,pos);
}

void dd4hep::detail::matrix::_decompose(const Transform3D& trafo, Position& pos, XYZAngles& rot)  {
  EulerAngles r;
  trafo.GetDecomposition(r,pos);
  rot.SetXYZ(r.Psi(),r.Theta(),r.Phi());
}

/// Check matrices for equality
auto dd4hep::detail::matrix::_matrixEqual(const TGeoMatrix& left, const TGeoMatrix& right) -> int   {
  double epsilon = 1e-12;
  int result = MATRICES_EQUAL;
  const Double_t* t1 = left.GetTranslation();
  const Double_t* t2 = right.GetTranslation();
  for(int i=0; i<3; ++i)   {
    if ( std::fabs(t1[i]-t2[i]) > epsilon )  {
      result = MATRICES_DIFFER_TRANSLATION;
      break;
    }
  }
  const Double_t* r1 = left.GetRotationMatrix();
  const Double_t* r2 = right.GetRotationMatrix();
  for(int i=0; i<9; ++i)   {
    if ( std::fabs(r1[i]-r2[i]) > epsilon )  {
      result |= MATRICES_DIFFER_ROTATION;
      break;
    }
  }
  return result;
}

    
