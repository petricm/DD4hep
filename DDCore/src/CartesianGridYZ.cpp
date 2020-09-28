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
#include "DD4hep/CartesianGridYZ.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DDSegmentation/CartesianGridYZ.h"

// C/C++ include files

using namespace std;
using namespace dd4hep;

/// determine the position based on the cell ID
auto CartesianGridYZ::position(const CellID& id) const -> Position   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
auto CartesianGridYZ::cellID(const Position& local,
                                       const Position& global,
                                       const VolumeID& volID) const -> dd4hep::CellID
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in Y
auto CartesianGridYZ::gridSizeY() const -> double {
  return access()->implementation->gridSizeY();
}

/// access the grid size in Z
auto CartesianGridYZ::gridSizeZ() const -> double {
  return access()->implementation->gridSizeZ();
}

/// set the grid size in Y
void CartesianGridYZ::setGridSizeY(double cellSize) const   {
  access()->implementation->setGridSizeY(cellSize);
}

/// set the grid size in Z
void CartesianGridYZ::setGridSizeZ(double cellSize) const   {
  access()->implementation->setGridSizeZ(cellSize);
}

/// access the coordinate offset in Y
auto CartesianGridYZ::offsetY() const -> double {
  return access()->implementation->offsetY();
}

/// access the coordinate offset in Z
auto CartesianGridYZ::offsetZ() const -> double {
  return access()->implementation->offsetZ();
}

/// set the coordinate offset in Y
void CartesianGridYZ::setOffsetY(double offset) const   {
  access()->implementation->setOffsetY(offset);
}

/// set the coordinate offset in Z
void CartesianGridYZ::setOffsetZ(double offset) const   {
  access()->implementation->setOffsetZ(offset);
}

/// access the field name used for Y
auto CartesianGridYZ::fieldNameY() const -> const string& {
  return access()->implementation->fieldNameY();
}

/// access the field name used for Z
auto CartesianGridYZ::fieldNameZ() const -> const string& {
  return access()->implementation->fieldNameZ();
}

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in y
    -# size in z
*/
auto CartesianGridYZ::cellDimensions(const CellID& id) const -> vector<double>  {
  return access()->implementation->cellDimensions(id);
}
