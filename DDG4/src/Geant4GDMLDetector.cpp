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
#include "DDG4/Geant4GDMLDetector.h"

// C/C++ include files
#include <iostream>
#include <utility>


//#define GEANT4_HAS_GDML

#ifdef GEANT4_HAS_GDML
#include "G4GDMLParser.hh"
#endif

using namespace std;
using namespace dd4hep;

dd4hep::sim::Geant4GDMLDetector::Geant4GDMLDetector(std::string  gdmlFile)
  : m_fileName(std::move(gdmlFile)), m_world(nullptr) {
}

auto dd4hep::sim::Geant4GDMLDetector::Construct() -> G4VPhysicalVolume* {
#ifdef GEANT4_HAS_GDML
  G4GDMLParser parser;
  parser.Read( m_fileName );
  m_world = parser.GetWorldVolume();
#endif
  return m_world;
}
