//==========================================================================
//  AIDA Detector description implementation 
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

// Framework include files
#include "DD4hep/MultiSegmentation.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DDSegmentation/MultiSegmentation.h"

// C/C++ include files

using namespace std;
using namespace dd4hep;

/// access the field name used to discriminate sub-segmentations
auto MultiSegmentation::discriminatorName() const -> const std::string&   {
  return access()->implementation->discriminatorName();
}

/// Discriminating bitfield entry
auto MultiSegmentation::discriminator() const -> const BitFieldElement*  {
  return access()->implementation->discriminator();
}

/// The underlying sub-segementations
auto
MultiSegmentation::subSegmentations()  const -> const MultiSegmentation::Segmentations&   {
  return access()->implementation->subSegmentations();
}

/// determine the position based on the cell ID
auto MultiSegmentation::position(const CellID& id) const -> Position   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
auto MultiSegmentation::cellID(const Position& local,
                                   const Position& global,
                                   const VolumeID& volID) const -> dd4hep::CellID
{
  return access()->implementation->cellID(local, global, volID);
}

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in x
    -# size in y
*/
auto MultiSegmentation::cellDimensions(const CellID& id) const -> vector<double>  {
  return access()->implementation->cellDimensions(id);
}
