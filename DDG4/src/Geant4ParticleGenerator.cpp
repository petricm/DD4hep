// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4ParticleGenerator.h"
#include "CLHEP/Units/SystemOfUnits.h"

// Geant4 include files
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

// C/C++ include files
#include <stdexcept>
#include <cmath>

using CLHEP::MeV;
using CLHEP::GeV;
using namespace std;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4ParticleGenerator::Geant4ParticleGenerator(Geant4Context* ctxt, const string& nam)
  : Geant4GeneratorAction(ctxt, nam), m_direction(0,1,0), m_position(0,0,0), m_particle(0)
{
  InstanceCount::increment(this);
  m_needsControl = true;
  declareProperty("Particle",      m_particleName = "e-");
  declareProperty("Energy",        m_energy = 50 * CLHEP::MeV);
  declareProperty("Multiplicity",  m_multiplicity = 1);
  declareProperty("Mask",          m_mask = 0);
  declareProperty("Position",      m_position);
  declareProperty("Direction",     m_direction);
}

/// Default destructor
Geant4ParticleGenerator::~Geant4ParticleGenerator() {
  InstanceCount::decrement(this);
}

/// Particle modification. Caller presets defaults to: ( direction = m_direction, momentum = m_energy)
void Geant4ParticleGenerator::getParticleDirection(int , ROOT::Math::XYZVector& , double& ) const   {
}

/// Particle modification. Caller presets defaults to: (multiplicity=m_multiplicity)
void Geant4ParticleGenerator::getParticleMultiplicity(int& ) const   {
}

/// Particle modification. Caller presets defaults to: (multiplicity=m_multiplicity)
void Geant4ParticleGenerator::getVertexPosition(ROOT::Math::XYZVector& ) const   {
}

/// Callback to generate primary particles
void Geant4ParticleGenerator::operator()(G4Event*) {
  typedef Geant4Particle Particle;

  if (0 == m_particle || m_particle->GetParticleName() != m_particleName.c_str()) {
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    m_particle = particleTable->FindParticle(m_particleName);
    if (0 == m_particle) {
      throw runtime_error("Geant4ParticleGenerator: Bad particle type:"+m_particleName+"!");
    }
  }
  Geant4Event& evt = context()->event();
  Geant4PrimaryEvent* prim = evt.extension<Geant4PrimaryEvent>();
  Geant4PrimaryInteraction* inter = new Geant4PrimaryInteraction();
  prim->add(m_mask, inter);

  Geant4Vertex* vtx = new Geant4Vertex();
  int multiplicity = m_multiplicity;
  ROOT::Math::XYZVector unit_direction, direction, position = m_position;
  getVertexPosition(position);
  getParticleMultiplicity(multiplicity);
  vtx->mask = m_mask;
  vtx->x = position.X();
  vtx->y = position.Y();
  vtx->z = position.Z();
  inter->vertices.insert(make_pair(inter->vertices.size(),vtx));
  for(int i=0; i<m_multiplicity; ++i)   {
    double momentum = m_energy;
    Particle* p = new Particle();
    direction = m_direction;
    getParticleDirection(i, direction, momentum);
    unit_direction = direction.unit();
    p->id         = inter->nextPID();
    p->status    |= G4PARTICLE_GEN_STABLE;
    p->mask       = m_mask;
    p->pdgID      = m_particle->GetPDGEncoding();

    p->psx        = unit_direction.X()*momentum;
    p->psy        = unit_direction.Y()*momentum;
    p->psz        = unit_direction.Z()*momentum;
    p->mass       = m_particle->GetPDGMass();
    p->vsx        = vtx->x;
    p->vsy        = vtx->y;
    p->vsz        = vtx->z;
    p->vex        = vtx->x;
    p->vey        = vtx->y;
    p->vez        = vtx->z;
    inter->particles.insert(make_pair(p->id,p));
    vtx->out.insert(p->id);
    printout(INFO,name(),"Particle [%d] %s %.3f GeV direction:(%6.3f %6.3f %6.3f)",
             p->id, m_particleName.c_str(), momentum/CLHEP::GeV, 
	     direction.X(), direction.Y(), direction.Z());

  }
}
