// $Id:$
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
    Ref_t                   gas;
    Ref_t                   endplate;
    int                     nmodules;
    double                  driftlength;
  };
}
