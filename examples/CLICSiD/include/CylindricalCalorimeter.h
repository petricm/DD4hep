/*
 * CylindricalCalorimeter.h
 *
 *  Created on: Apr 17, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef CYLINDRICALCALORIMETER_H_
#define CYLINDRICALCALORIMETER_H_

#include "Calorimeter.h"
#include "CylindricalDetector.h"

namespace DD4hep {
  namespace Geometry {

    class CylindricalCalorimeter : public Calorimeter, public CylindricalDetector {
    public:
      CylindricalCalorimeter(const DetElement& e) : DetElement(e), Calorimeter(e), CylindricalDetector(e) {}
      virtual ~CylindricalCalorimeter() {}
    };

  } /* namespace Geometry */
} /* namespace DD4hep */
#endif /* CYLINDRICALCALORIMETER_H_ */
