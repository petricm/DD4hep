// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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

#ifndef DDG4_DEFS_H
#define DDG4_DEFS_H

// Framework include files
#include "DD4hep/LCDD.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    using DD4hep::Position;
    using Geometry::LCDD;
    using Geometry::Readout;
    using Geometry::DetElement;

    // Forward declarations;
    typedef Position Direction;
    typedef Position Momentum;

    class Geant4StepHandler;
    class Geant4Hit;
    class Geant4TrackerHit;
    class Geant4CalorimeterHit;
    class Geant4SensitiveDetector;
    template <class T> class Geant4GenericSD;

  }  // End namespace Simulation
}  // End namespace DD4hep
#endif  // DDG4_DEFS_H
