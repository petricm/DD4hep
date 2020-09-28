/*
 * CylindricalSegmentation.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CylindricalSegmentation.h"

namespace dd4hep::DDSegmentation {

    /// default constructor using an encoding string
    CylindricalSegmentation::CylindricalSegmentation(const std::string& cellEncoding) :
      Segmentation(cellEncoding) {
    }

    /// Default constructor used by derived classes passing an existing decoder
    CylindricalSegmentation::CylindricalSegmentation(const BitFieldCoder* decode) :
      Segmentation(decode) {
    }

    /// destructor
    CylindricalSegmentation::~CylindricalSegmentation() = default;

  } /* namespace dd4hep */
