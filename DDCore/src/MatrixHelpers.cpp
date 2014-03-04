// $Id: Handle.cpp 575 2013-05-17 20:41:52Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "MatrixHelpers.h"

// ROOT includes
#include "TGeoMatrix.h"

TGeoIdentity* DD4hep::Geometry::identityTransform() {
  return gGeoIdentity;
}

TGeoTranslation* DD4hep::Geometry::_translation(const Position& pos) {
  return new TGeoTranslation("", pos.X(), pos.Y(), pos.Z());
}

TGeoRotation* DD4hep::Geometry::_rotationZYX(const RotationZYX& rot) {
  return new TGeoRotation("", rot.Phi() * RAD_2_DEGREE, rot.Theta() * RAD_2_DEGREE, rot.Psi() * RAD_2_DEGREE);
}

TGeoRotation* DD4hep::Geometry::_rotation3D(const Rotation3D& rot3D) {
  EulerAngles rot(rot3D);
  return new TGeoRotation("", rot.Phi() * RAD_2_DEGREE, rot.Theta() * RAD_2_DEGREE, rot.Psi() * RAD_2_DEGREE);
}

TGeoHMatrix* DD4hep::Geometry::_transform(const Transform3D& trans) {
  double t[3];
  RotationZYX rot;
  Position pos;
  trans.GetDecomposition(rot, pos);
  TGeoHMatrix *tr = new TGeoHMatrix();
  tr->RotateZ(rot.Phi() * RAD_2_DEGREE);
  tr->RotateY(rot.Theta() * RAD_2_DEGREE);
  tr->RotateX(rot.Psi() * RAD_2_DEGREE);
  pos.GetCoordinates(t);
  tr->SetDx(t[0]);
  tr->SetDy(t[1]);
  tr->SetDz(t[2]);
  return tr;
}

TGeoHMatrix* DD4hep::Geometry::_transform(const Position& pos, const RotationZYX& rot) {
  double t[3];
  TGeoHMatrix *tr = new TGeoHMatrix();
  tr->RotateZ(rot.Phi() * RAD_2_DEGREE);
  tr->RotateY(rot.Theta() * RAD_2_DEGREE);
  tr->RotateX(rot.Psi() * RAD_2_DEGREE);
  pos.GetCoordinates(t);
  tr->SetDx(t[0]);
  tr->SetDy(t[1]);
  tr->SetDz(t[2]);
  return tr;
}
