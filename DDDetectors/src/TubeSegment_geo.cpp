// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
//
// Specialized generic detector constructor
//
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element( LCDD& lcdd, xml_h e, Ref_t sens ) {
  xml_det_t  x_det( e );
  xml_comp_t x_tube = x_det.tubs();
  xml_dim_t  pos    = x_det.position();
  xml_dim_t  rot    = x_det.rotation();
  string     name   = x_det.nameStr();
  Tube       tub( x_tube.rmin(), x_tube.rmax(), x_tube.zhalf() );
  Volume     vol( name, tub, lcdd.material( x_det.materialStr() ) );

  vol.setVisAttributes( lcdd, x_det.visStr() );
  vol.setLimitSet( lcdd, x_det.limitsStr() );
  vol.setRegion( lcdd, x_det.regionStr() );
  if ( x_det.isSensitive() ) {
    SensitiveDetector sd     = sens;
    xml_dim_t         sd_typ = x_det.child( _U( sensitive ) );
    vol.setSensitiveDetector( sens );
    sd.setType( sd_typ.typeStr() );
  }

  DetElement   sdet( name, x_det.id() );
  Volume       mother = lcdd.pickMotherVolume( sdet );
  PlacedVolume phv    = mother.placeVolume(
      vol, Transform3D( RotationZYX( rot.z(), rot.y(), rot.x() ), Position( -pos.x(), -pos.y(), pos.z() ) ) );
  if ( x_det.hasAttr( _U( id ) ) ) {
    phv.addPhysVolID( "system", x_det.id() );
  }
  sdet.setPlacement( phv );
  return sdet;
}

DECLARE_DETELEMENT( DD4hep_TubeSegment, create_element )
DECLARE_DEPRECATED_DETELEMENT( TubeSegment, create_element )
