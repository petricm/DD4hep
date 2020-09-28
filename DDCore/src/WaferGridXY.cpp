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
#include "DD4hep/WaferGridXY.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DDSegmentation/WaferGridXY.h"

// C/C++ include files

using namespace std;
using namespace dd4hep;

/// determine the position based on the cell ID
auto WaferGridXY::position(const CellID& id) const -> Position   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
auto WaferGridXY::cellID(const Position& local,
                                   const Position& global,
                                   const VolumeID& volID) const -> dd4hep::CellID
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in X
auto WaferGridXY::gridSizeX() const -> double {
  return access()->implementation->gridSizeX();
}

/// access the grid size in Y
auto WaferGridXY::gridSizeY() const -> double {
  return access()->implementation->gridSizeY();
}

/// access the coordinate offset in X
auto WaferGridXY::offsetX() const -> double {
  return access()->implementation->offsetX();
}

/// access the coordinate offset in Y
auto WaferGridXY::offsetY() const -> double {
  return access()->implementation->offsetY();
}
/// access the coordinate waferOffset for inGroup in X
auto WaferGridXY::waferOffsetX(int inGroup, int inWafer) const -> double  {
  return access()->implementation->waferOffsetX(inGroup,inWafer);
}

/// access the coordinate waferOffset for inGroup in Y
auto WaferGridXY::waferOffsetY(int inGroup, int inWafer) const -> double  {
  return access()->implementation->waferOffsetY(inGroup,inWafer);
}

/// access the field name used for X
auto WaferGridXY::fieldNameX() const -> const string& {
  return access()->implementation->fieldNameX();
}

/// access the field name used for Y
auto WaferGridXY::fieldNameY() const -> const string& {
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
auto WaferGridXY::cellDimensions(const CellID& id) const -> vector<double>  {
  return access()->implementation->cellDimensions(id);
}
