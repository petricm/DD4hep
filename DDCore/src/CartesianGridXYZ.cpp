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
#include "DD4hep/CartesianGridXYZ.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DDSegmentation/CartesianGridXYZ.h"

// C/C++ include files

using namespace std;
using namespace dd4hep;

/// determine the position based on the cell ID
auto CartesianGridXYZ::position(const CellID& id) const -> Position   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
auto CartesianGridXYZ::cellID(const Position& local,
                                        const Position& global,
                                        const VolumeID& volID) const -> dd4hep::CellID
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in X
auto CartesianGridXYZ::gridSizeX() const -> double {
  return access()->implementation->gridSizeX();
}

/// access the grid size in Y
auto CartesianGridXYZ::gridSizeY() const -> double {
  return access()->implementation->gridSizeY();
}

/// access the grid size in Z
auto CartesianGridXYZ::gridSizeZ() const -> double {
  return access()->implementation->gridSizeZ();
}

/// set the grid size in X
void CartesianGridXYZ::setGridSizeX(double cellSize) const   {
  access()->implementation->setGridSizeX(cellSize);
}

/// set the grid size in Y
void CartesianGridXYZ::setGridSizeY(double cellSize) const   {
  access()->implementation->setGridSizeY(cellSize);
}

/// set the grid size in Z
void CartesianGridXYZ::setGridSizeZ(double cellSize) const   {
  access()->implementation->setGridSizeZ(cellSize);
}

/// access the coordinate offset in X
auto CartesianGridXYZ::offsetX() const -> double {
  return access()->implementation->offsetX();
}

/// access the coordinate offset in Y
auto CartesianGridXYZ::offsetY() const -> double {
  return access()->implementation->offsetY();
}

/// access the coordinate offset in Z
auto CartesianGridXYZ::offsetZ() const -> double {
  return access()->implementation->offsetZ();
}

/// set the coordinate offset in X
void CartesianGridXYZ::setOffsetX(double offset) const   {
  access()->implementation->setOffsetX(offset);
}

/// set the coordinate offset in Y
void CartesianGridXYZ::setOffsetY(double offset) const   {
  access()->implementation->setOffsetY(offset);
}

/// set the coordinate offset in Z
void CartesianGridXYZ::setOffsetZ(double offset) const   {
  access()->implementation->setOffsetZ(offset);
}

/// access the field name used for X
auto CartesianGridXYZ::fieldNameX() const -> const string& {
  return access()->implementation->fieldNameX();
}

/// access the field name used for Y
auto CartesianGridXYZ::fieldNameY() const -> const string& {
  return access()->implementation->fieldNameY();
}

/// access the field name used for Z
auto CartesianGridXYZ::fieldNameZ() const -> const string& {
  return access()->implementation->fieldNameZ();
}

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in x
    -# size in y
    -# size in z
*/
auto CartesianGridXYZ::cellDimensions(const CellID& id) const -> vector<double>  {
  return access()->implementation->cellDimensions(id);
}
