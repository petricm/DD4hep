// $Id: TPCData.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include <vector>
#include "DD4hep/Detector.h"

namespace DD4hep {

  struct TPCData : public Geometry::DetElement::Object {
    typedef Geometry::Ref_t Ref_t;
    Ref_t                   outerWall;
    Ref_t                   innerWall;
    Ref_t                   gasVolume;
    Ref_t                   cathode;
    //positive endplate
    Ref_t endplate;
    //negative endplate
    Ref_t endplate2;
  };
}
