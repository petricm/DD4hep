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
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4GeometryInfo.h"
#include "DDG4/Geant4DetectorConstruction.h"

#include "G4VUserDetectorConstruction.hh"
#include "G4SDManager.hh"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

/// Helper: Assign sensitive detector to logical volume
void Geant4DetectorConstructionContext::setSensitiveDetector(G4LogicalVolume* vol, G4VSensitiveDetector* sd)   {
  //detector->SetSensitiveDetector(vol,sd);
  G4SDManager::GetSDMpointer()->AddNewDetector(sd);
  vol->SetSensitiveDetector(sd);
}

/// Standard Constructor
Geant4DetectorConstruction::Geant4DetectorConstruction(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt,nam)
{
}

/// Default destructor
Geant4DetectorConstruction::~Geant4DetectorConstruction()  = default;

/// Geometry construction callback. Called at "Construct()"
void Geant4DetectorConstruction::constructGeo(Geant4DetectorConstructionContext* )   {
}

/// Electromagnetic field construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstruction::constructField(Geant4DetectorConstructionContext* )   {
}

/// Sensitive detector construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstruction::constructSensitives(Geant4DetectorConstructionContext* )   {
}


/// Standard Constructor
Geant4DetectorConstructionSequence::Geant4DetectorConstructionSequence(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt,nam)
{
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4DetectorConstructionSequence::~Geant4DetectorConstructionSequence()   {
  m_actors(&Geant4DetectorConstruction::release);  
  InstanceCount::decrement(this);
}

/// Set or update client context
void Geant4DetectorConstructionSequence::updateContext(Geant4Context* ctxt)   {
  m_context = ctxt;
  m_actors(&Geant4DetectorConstruction::updateContext,ctxt);  
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4DetectorConstructionSequence::adopt(Geant4DetectorConstruction* action)   {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  except("Geant4RunActionSequence","++ Attempt to add an invalid actor!");
}

/// Access an actor by name
auto Geant4DetectorConstructionSequence::get(const std::string& nam)  const -> Geant4DetectorConstruction*   {
  for(auto* i : m_actors)  {
    if ( i->name() == nam )  {
      return i;
    }
  }
  except("Geant4RunActionSequence","++ Attempt to access invalid actor %s!",nam.c_str());
  return nullptr;
}

/// Geometry construction callback. Called at "Construct()"
void Geant4DetectorConstructionSequence::constructGeo(Geant4DetectorConstructionContext* ctxt)   {
  m_actors(&Geant4DetectorConstruction::constructGeo, ctxt);  
}

/// Electromagnetic field construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstructionSequence::constructField(Geant4DetectorConstructionContext* ctxt)   {
  m_actors(&Geant4DetectorConstruction::constructField, ctxt);  
}

/// Sensitive detector construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstructionSequence::constructSensitives(Geant4DetectorConstructionContext* ctxt)   {
  m_actors(&Geant4DetectorConstruction::constructSensitives, ctxt);  
}

/// Access to the converted regions
auto Geant4DetectorConstructionSequence::regions() const -> const map<Region, G4Region*>&   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Regions;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::regions: Access not possible. Geometry is not yet converted!");
}
#if 0
/// Access to the converted sensitive detectors
const Geant4GeometryMaps::SensDetMap& Geant4DetectorConstructionSequence::sensitives() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4SensDets;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::sensitives: Access not possible. Geometry is not yet converted!");
}
#endif
/// Access to the converted volumes
auto Geant4DetectorConstructionSequence::volumes() const -> const map<Volume, G4LogicalVolume*>&   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Volumes;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::volumes: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted shapes
auto Geant4DetectorConstructionSequence::shapes() const -> const map<const TGeoShape*, G4VSolid*>&   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Solids;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::shapes: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted limit sets
auto Geant4DetectorConstructionSequence::limits() const -> const map<LimitSet, G4UserLimits*>&   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Limits;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::limits: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted assemblies
auto Geant4DetectorConstructionSequence::assemblies() const -> const map<PlacedVolume, Geant4AssemblyVolume*>&   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4AssemblyVolumes;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::assemblies: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted placements
auto Geant4DetectorConstructionSequence::placements() const -> const map<PlacedVolume, G4VPhysicalVolume*>&   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Placements;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::placements: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted materials
auto Geant4DetectorConstructionSequence::materials() const -> const Geant4GeometryMaps::MaterialMap&   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Materials;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::materials: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted elements
auto Geant4DetectorConstructionSequence::elements() const -> const Geant4GeometryMaps::ElementMap&   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Elements;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::elements: Access not possible. Geometry is not yet converted!");
}




