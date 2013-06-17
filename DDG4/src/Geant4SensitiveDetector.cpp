// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDG4/Geant4SensitiveDetector.h"
#include "DDG4/Geant4Converter.h"
#include "DDG4/Geant4Hits.h"
#include "DD4hep/Printout.h"
#include "DD4hep/LCDD.h"

#include "G4Step.hh"
#include "G4PVPlacement.hh"

#include "TGeoNode.h"

#define DEBUG 0


// C/C++ include files
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Constructor. The detector element is identified by the name
Geant4SensitiveDetector::Geant4SensitiveDetector(const string& name, LCDD& lcdd)
  : G4VSensitiveDetector(name), m_lcdd(lcdd), m_detector(), m_sensitive(), m_readout(), m_hce(0)
{
  m_sensitive = lcdd.sensitiveDetector(name);
  m_detector  = lcdd.detector(name);
  m_readout   = m_sensitive.readout();
}

/// Standard destructor
Geant4SensitiveDetector::~Geant4SensitiveDetector() {
}

/// Initialize the sensitive detector for the usage of a single hit collection
bool Geant4SensitiveDetector::defineCollection(const string& coll_name)   {
  if ( coll_name.empty() ) {
    throw runtime_error("Geant4SensitiveDetector: No collection defined for "+name()+
			" of type "+string(m_sensitive.type()));
  }
  collectionName.insert(coll_name);
  return true;
}

/// Access HitCollection container names
const string& Geant4SensitiveDetector::hitCollectionName(int which) const      { 
  size_t w = which;
  if ( w >= collectionName.size() ) {
    throw runtime_error("The collection name index for subdetector "+name()+" is out of range!");
  }
  return collectionName[which];
}

/// Create single hits collection
Geant4SensitiveDetector::HitCollection* Geant4SensitiveDetector::createCollection(const string& coll_name)  const  {
  return new G4THitsCollection<Geant4Hit>(GetName(), coll_name);
}
namespace DD4hep { namespace Simulation {
template <> Geant4CalorimeterHit* 
Geant4SensitiveDetector::find<Geant4CalorimeterHit>(const HitCollection* c,const HitCompare<Geant4CalorimeterHit>& cmp)   {
  typedef vector<Geant4CalorimeterHit*> _V;
  const _V* v = (const _V*)c->GetVector();
  for(_V::const_iterator i=v->begin(); i !=v->end(); ++i) 
    if ( cmp(*i) ) return *i;
  return 0;
}
}}

/// Method invoked at the begining of each event. 
void Geant4SensitiveDetector::Initialize(G4HCofThisEvent* HCE) {
  int count = 0;
  m_hce = HCE;
  for(G4CollectionNameVector::const_iterator i=collectionName.begin(); i!=collectionName.end();++i,++count) {
    G4VHitsCollection* c = createCollection(*i);
    m_hce->AddHitsCollection(GetCollectionID(count),c);
  }
}

/// Method invoked at the end of each event. 
void Geant4SensitiveDetector::EndOfEvent(G4HCofThisEvent* /* HCE */) {
  m_hce = 0;
  // Eventuall print event summary
}

/// Method for generating hit(s) using the information of G4Step object.
G4bool Geant4SensitiveDetector::ProcessHits(G4Step* step,G4TouchableHistory* hist) {
  double ene_cut = m_sensitive.energyCutoff();
  if ( step->GetTotalEnergyDeposit() > ene_cut ) {
    if ( !Geant4Hit::isGeantino(step->GetTrack()) )   {
#if DEBUG
      dumpStep(step, hist);
#endif
      return buildHits(step,hist);
    }
  }
  return false;
}

/// Retrieve the hits collection associated with this detector by its collection identifier
Geant4SensitiveDetector::HitCollection* Geant4SensitiveDetector::collectionByID(int id)    {
  HitCollection* hc = (HitCollection*)m_hce->GetHC(id);
  if ( hc ) return hc;
  throw runtime_error("The collection index for subdetector "+name()+" is wrong!");
}

/// Retrieve the hits collection associated with this detector by its serial number
Geant4SensitiveDetector::HitCollection* Geant4SensitiveDetector::collection(int which)    {
  size_t w = which;
  if ( w < collectionName.size() ) {
    HitCollection* hc = (HitCollection*)m_hce->GetHC(GetCollectionID(which));
    if ( hc ) return hc;
    throw runtime_error("The collection index for subdetector "+name()+" is wrong!");
  }
  throw runtime_error("The collection name index for subdetector "+name()+" is out of range!");
}

/// Method is invoked if the event abortion is occured.
void Geant4SensitiveDetector::clear() {
}

/// Dump Step information (careful: very verbose)
void Geant4SensitiveDetector::dumpStep(G4Step* st, G4TouchableHistory* /* history */) {
  Geant4StepHandler step(st);
  Geant4Mapping& cnv = Geant4Mapping::instance();
  //Geant4Converter::G4GeometryInfo& data = cnv.data();

  Position pos1 = step.prePos();
  Position pos2 = step.postPos();
  Momentum mom = step.postMom();

  printout(INFO,"G4Step","  Track:%08ld Pos:(%8f %8f %8f) -> (%f %f %f)  Mom:%7.0f %7.0f %7.0f",
   	   long(step.track), pos1.X(), pos1.Y(), pos1.Z(), pos2.X(), pos2.Y(), pos2.Z(), mom.X(), mom.Y(), mom.Z());
  printout(INFO,"G4Step","                pre-Vol: %s  Status:%s",
   	   step.preVolume()->GetName().c_str(), step.preStepStatus());
  printout(INFO,"G4Step","                post-Vol:%s  Status:%s",
   	   step.postVolume()->GetName().c_str(), step.postStepStatus());
  
  const G4VPhysicalVolume* pv = step.volume(step.post);
  
  typedef Geant4Converter::PlacementMap Places;
  const Places& places = cnv.data().g4Placements;

  for(Places::const_iterator i=places.begin(); i!=places.end();++i) {
    const G4PVPlacement* pl = (*i).second;
    const G4VPhysicalVolume* qv = pl;
    
    if ( qv == pv ) {
      const TGeoNode* tpv = (*i).first;
      printf("           Found TGeoNode:%s!\n",tpv->GetName());
    }
  }

}
