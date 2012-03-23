// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Detector.h"
#include "DDTPCModule.h"

namespace DD4hep {

  struct GearTPC : public Geometry::DetElement {
    GearTPC(const Geometry::Ref_t& e);

    double getInnerRadius() const;
    double getOuterRadius() const;
    double getEndPlateThickness() const;
    void   listDetElements() const;

    /** The maximum drift length in the TPC in mm.
     */
    double getMaxDriftLength() const;

    /** Returns number of modules on endplate (default 0 or 1, see compact xml).
     */
    int getNModules(int endplate) const;

    /** Returns the module with given id from endplate 0 or 1.
     */
    const DDTPCModule& getModule(int ID, int endplate) const;

    /** Returns nearest module to given coordinates (3D) on endplate (default 0 or 1, see compact xml).
     */
    const DDTPCModule& getNearestModule(double c0, double c1, int endplate) const;
  };
}
