/*
 * NoSegmentation.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/NoSegmentation.h"

namespace dd4hep::DDSegmentation {

    NoSegmentation::NoSegmentation(const std::string& cellEncoding) :
      Segmentation(cellEncoding)
    {
      _type = "NoSegmentation";
      _description = "None Segmentation";
    }

    NoSegmentation::NoSegmentation(const BitFieldCoder* decode) : Segmentation(decode)
    { 
      _type = "NoSegmentation";
      _description = "None Segmentation";
    }

    NoSegmentation::~NoSegmentation()
    = default;

    auto NoSegmentation::position(const CellID& /*cID*/) const -> Vector3D
    {
      Vector3D cellPosition{0,0,0};
      return cellPosition;
    }

    auto NoSegmentation::cellID(const Vector3D& /*localPosition*/, const Vector3D& /* globalPosition */, const VolumeID& vID) const -> CellID {
      return vID;
    }

REGISTER_SEGMENTATION(NoSegmentation)

  } /* namespace dd4hep */
