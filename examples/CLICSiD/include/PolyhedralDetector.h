/*
 * PolyhedralDetector.h
 *
 *  Created on: Apr 3, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef POLYHEDRALDETECTOR_H_
#define POLYHEDRALDETECTOR_H_

#include "DD4hep/Detector.h"

namespace DD4hep {
  namespace Geometry {
    class PolyhedralDetector : virtual public DetElement {
    public:
      PolyhedralDetector(const DetElement& e) : DetElement(e) {}
      virtual ~PolyhedralDetector() {}

      int    getNSides() const { return getPolyhedra()->GetNedges(); }
      double getRMin() const { return getPolyhedra()->GetRmin(0); }
      double getRMax() const { return getPolyhedra()->GetRmax(0); }
      double getZMin() const { return getPolyhedra()->GetZ(0); }
      double getZMax() const { return getPolyhedra()->GetZ(1); }

    protected:
      PolyhedraRegular getPolyhedra() const { return PolyhedraRegular(volume().solid()); }
    };
  } /* namespace Geometry */
} /* namespace DD4hep */
#endif /* POLYHEDRALDETECTOR_H_ */
