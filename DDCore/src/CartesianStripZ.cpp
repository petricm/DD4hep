//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank, D.Blyth
//
//==========================================================================

// Framework include files
#include "DD4hep/CartesianStripZ.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DDSegmentation/CartesianStripZ.h"

using namespace std;
using namespace dd4hep;

/// determine the position based on the cell ID
auto CartesianStripZ::position(const CellID& id) const -> Position {
    return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
auto CartesianStripZ::cellID(const Position& local, const Position& global,
                                       const VolumeID& volID) const -> dd4hep::CellID {
    return access()->implementation->cellID(local, global, volID);
}

/// access the strip size in Z
auto CartesianStripZ::stripSizeZ() const -> double { return access()->implementation->stripSizeZ(); }

/// set the strip size in Z
void CartesianStripZ::setStripSizeZ(double cellSize) const {
    access()->implementation->setStripSizeZ(cellSize);
}

/// access the coordinate offset in Z
auto CartesianStripZ::offsetZ() const -> double { return access()->implementation->offsetZ(); }

/// set the coordinate offset in Z
void CartesianStripZ::setOffsetZ(double offset) const { access()->implementation->setOffsetZ(offset); }

/// access the field name used for Z
auto CartesianStripZ::fieldNameZ() const -> const string& { return access()->implementation->fieldNameZ(); }

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in x
    -# size in y
*/
auto CartesianStripZ::cellDimensions(const CellID& id) const -> vector<double> {
    return access()->implementation->cellDimensions(id);
}
