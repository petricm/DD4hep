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
#include "DDG4/Geant4SensDetAction.h"

// Forward declarations
class G4ParticleDefinition;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep::sim {

    /// Geant4 sensitive detector filter base class for particle filters
    /**
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct ParticleFilter : public Geant4Filter  {
    protected:
      /// name of the particle to be filtered
      std::string m_particle;
      /// Corresponding geant4 particle definiton
      mutable G4ParticleDefinition* m_definition;
    public:
      /// Constructor.
      ParticleFilter(Geant4Context* context, const std::string& name);
      /// Standard destructor
      ~ParticleFilter() override;
      /// Safe access to the definition
      auto definition() const -> const G4ParticleDefinition*;
      /// Check if a track is of the same type
      auto isSameType(const G4Track* track)  const -> bool;
      /// Check if the particle is a geantino
      auto isGeantino(const G4Track* track) const -> bool;
    };

    /// Geant4 sensitive detector filter implementing a particle rejector
    /**
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct ParticleRejectFilter : public ParticleFilter  {
      /// Constructor.
      ParticleRejectFilter(Geant4Context* c, const std::string& n);
      /// Standard destructor
      ~ParticleRejectFilter() override;
      /// Filter action. Return true if hits should be processed
      auto operator()(const G4Step* step) const -> bool  final;
    };

    /// Geant4 sensitive detector filter implementing a particle selector
    /**
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct ParticleSelectFilter : public ParticleFilter  {
      /// Constructor.
      ParticleSelectFilter(Geant4Context* c, const std::string& n);
      /// Standard destructor
      ~ParticleSelectFilter() override;
      /// Filter action. Return true if hits should be processed
      auto operator()(const G4Step* step) const -> bool  final;
    };

    /// Geant4 sensitive detector filter implementing a Geantino rejector
    /**
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct GeantinoRejectFilter : public ParticleFilter  {
      /// Constructor.
      GeantinoRejectFilter(Geant4Context* c, const std::string& n);
      /// Standard destructor
      ~GeantinoRejectFilter() override;
      /// Filter action. Return true if hits should be processed
      auto operator()(const G4Step* step) const -> bool  final;
    };

    /// Geant4 sensitive detector filter implementing an energy cut.
    /**
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct EnergyDepositMinimumCut : public Geant4Filter  {
      /// Energy cut value
      double m_energyCut;
    public:
      /// Constructor.
      EnergyDepositMinimumCut(Geant4Context* c, const std::string& n);
      /// Standard destructor
      ~EnergyDepositMinimumCut() override;
      /// Filter action. Return true if hits should be processed
      auto operator()(const G4Step* step) const -> bool  final;
    };
  }

/// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDG4/Factories.h"

// Geant4 include files
#include "G4ParticleTable.hh"
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4Track.hh"
#include "G4Step.hh"

using namespace dd4hep::sim;
using namespace dd4hep;
using namespace std;

//DECLARE_GEANT4ACTION()
DECLARE_GEANT4ACTION(GeantinoRejectFilter)
DECLARE_GEANT4ACTION(ParticleRejectFilter)
DECLARE_GEANT4ACTION(ParticleSelectFilter)
DECLARE_GEANT4ACTION(EnergyDepositMinimumCut)

/// Constructor.
ParticleFilter::ParticleFilter(Geant4Context* ctxt, const std::string& nam)
: Geant4Filter(ctxt,nam), m_definition(nullptr)
{
  declareProperty("particle",m_particle);
  InstanceCount::increment(this);
}

/// Standard destructor
ParticleFilter::~ParticleFilter()   {
  InstanceCount::decrement(this);
}

/// Safe access to the definition
auto ParticleFilter::definition() const -> const G4ParticleDefinition*  {
  if ( m_definition ) return m_definition;
  m_definition = G4ParticleTable::GetParticleTable()->FindParticle(m_particle);
  if ( nullptr == m_definition )  {
    throw runtime_error("Invalid particle name:'"+m_particle+"' [Not-in-particle-table]");
  }
  return m_definition;
}

/// Check if a track is of the same type
auto ParticleFilter::isSameType(const G4Track* track)  const -> bool   {
  G4ParticleDefinition* def = track->GetDefinition();
  return definition() == def;
}

/// Check if the particle is a geantino
auto ParticleFilter::isGeantino(const G4Track* track) const -> bool   {
  if ( track ) {
    G4ParticleDefinition* def = track->GetDefinition();
    if ( def == G4ChargedGeantino::Definition() )
      return true;
    if ( def == G4Geantino::Definition() ) {
      return true;
    }
  }
  return false;
}

/// Constructor.
GeantinoRejectFilter::GeantinoRejectFilter(Geant4Context* c, const std::string& n)
  : ParticleFilter(c,n) {
  InstanceCount::increment(this);
}

/// Standard destructor
GeantinoRejectFilter::~GeantinoRejectFilter() {
  InstanceCount::decrement(this);
}

/// Filter action. Return true if hits should be processed
auto GeantinoRejectFilter::operator()(const G4Step* step) const -> bool   {
  return !isGeantino(step->GetTrack());
}

/// Constructor.
ParticleRejectFilter::ParticleRejectFilter(Geant4Context* c, const std::string& n)
  : ParticleFilter(c,n) {
  InstanceCount::increment(this);
}

/// Standard destructor
ParticleRejectFilter::~ParticleRejectFilter() {
  InstanceCount::decrement(this);
}

/// Filter action. Return true if hits should be processed
auto ParticleRejectFilter::operator()(const G4Step* step) const -> bool   {
  return !isSameType(step->GetTrack());
}

/// Constructor.
ParticleSelectFilter::ParticleSelectFilter(Geant4Context* c, const std::string& n)
  : ParticleFilter(c,n) {
  InstanceCount::increment(this);
}

/// Standard destructor
ParticleSelectFilter::~ParticleSelectFilter() {
  InstanceCount::decrement(this);
}

/// Filter action. Return true if hits should be processed
auto ParticleSelectFilter::operator()(const G4Step* step) const -> bool   {
  return isSameType(step->GetTrack());
}

/// Constructor.
EnergyDepositMinimumCut::EnergyDepositMinimumCut(Geant4Context* c, const std::string& n)
  : Geant4Filter(c,n) {
  InstanceCount::increment(this);
  declareProperty("Cut",m_energyCut=0.0);
}

/// Standard destructor
EnergyDepositMinimumCut::~EnergyDepositMinimumCut() {
  InstanceCount::decrement(this);
}

/// Filter action. Return true if hits should be processed
auto EnergyDepositMinimumCut::operator()(const G4Step* step) const -> bool  {
  return step->GetTotalEnergyDeposit() > m_energyCut;
}

