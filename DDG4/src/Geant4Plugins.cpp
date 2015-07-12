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

// Framework include files
#include "DD4hep/Plugins.inl"
#include "DDG4/Factories.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4InputAction.h"
#include "DDG4/Geant4Converter.h"

// Geant4 include files
#include "G4VSensitiveDetector.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4EquationOfMotion.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagneticField.hh"
#include "G4VProcess.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4ParticleDefinition.hh"
#include "G4VUserPhysicsList.hh"

DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4VSensitiveDetector*, (std::string,DD4hep::Geometry::LCDD*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(DD4hep::Simulation::Geant4Sensitive*, (DD4hep::Simulation::Geant4Context*,
                                                                        std::string,DD4hep::Geometry::DetElement*,
                                                                        DD4hep::Geometry::LCDD*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(DD4hep::Simulation::Geant4Action*, (DD4hep::Simulation::Geant4Context*,std::string))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4MagIntegratorStepper*, (G4EquationOfMotion*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4MagIntegratorStepper*, (G4Mag_EqRhs*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4Mag_EqRhs*, (G4MagneticField*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4VProcess*, ())
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4VPhysicsConstructor*, ())
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4ParticleDefinition*, ())
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4VUserPhysicsList*, (DD4hep::Simulation::Geant4PhysicsListActionSequence*,int))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(DD4hep::Simulation::Geant4EventReader*, (std::string))
