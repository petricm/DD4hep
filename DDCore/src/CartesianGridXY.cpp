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
#include "DD4hep/CartesianGridXY.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DDSegmentation/CartesianGridXY.h"

// C/C++ include files

using namespace std;
using namespace dd4hep;

/// determine the position based on the cell ID
auto CartesianGridXY::position(const CellID& id) const -> Position   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
auto CartesianGridXY::cellID(const Position& local,
                                       const Position& global,
                                       const VolumeID& volID) const -> dd4hep::CellID
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in X
auto CartesianGridXY::gridSizeX() const -> double {
  return access()->implementation->gridSizeX();
}

/// access the grid size in Y
auto CartesianGridXY::gridSizeY() const -> double {
  return access()->implementation->gridSizeY();
}

/// set the grid size in X
void CartesianGridXY::setGridSizeX(double cellSize) const   {
  access()->implementation->setGridSizeX(cellSize);
}

/// set the grid size in Y
void CartesianGridXY::setGridSizeY(double cellSize) const   {
  access()->implementation->setGridSizeY(cellSize);
}

/// access the coordinate offset in X
auto CartesianGridXY::offsetX() const -> double {
  return access()->implementation->offsetX();
}

/// access the coordinate offset in Y
auto CartesianGridXY::offsetY() const -> double {
  return access()->implementation->offsetY();
}

/// set the coordinate offset in X
void CartesianGridXY::setOffsetX(double offset) const   {
  access()->implementation->setOffsetX(offset);
}

/// set the coordinate offset in Y
void CartesianGridXY::setOffsetY(double offset) const   {
  access()->implementation->setOffsetY(offset);
}

/// access the field name used for X
auto CartesianGridXY::fieldNameX() const -> const string& {
  return access()->implementation->fieldNameX();
}

/// access the field name used for Y
auto CartesianGridXY::fieldNameY() const -> const string& {
  return access()->implementation->fieldNameY();
}

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in x
    -# size in y
*/
auto CartesianGridXY::cellDimensions(const CellID& id) const -> vector<double>  {
  return access()->implementation->cellDimensions(id);
}
