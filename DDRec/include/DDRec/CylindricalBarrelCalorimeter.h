/*
 * CylindricalBarrelCalorimeter.h
 *
 *  Created on: Apr 17, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef CYLINDRICALBARRELCALORIMETER_H_
#define CYLINDRICALBARRELCALORIMETER_H_

#include "DDRec/BarrelDetector.h"
#include "DDRec/CylindricalCalorimeter.h"

namespace DD4hep {
  namespace DDRec {

    class CylindricalBarrelCalorimeter : public CylindricalCalorimeter, public BarrelDetector {
    public:
      CylindricalBarrelCalorimeter(const DetElement& e) : DetElement(e), CylindricalCalorimeter(e), BarrelDetector(e) {}
      virtual ~CylindricalBarrelCalorimeter() {}
    };

  } /* namespace DDRec */
} /* namespace DD4hep */
#endif /* CYLINDRICALBARRELCALORIMETER_H_ */
