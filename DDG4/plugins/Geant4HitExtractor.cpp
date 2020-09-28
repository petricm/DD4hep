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
#include <utility>



#include "DDG4/Geant4Data.h"

using namespace dd4hep;
using namespace dd4hep::sim;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep::DDEve  {

    /// SimulationHit definition
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     *  \ingroup DD4HEP_EVE
     */
    class SimulationHit   {
    public:
      Position position;
      float deposit;
      /// Default constructor
      SimulationHit() : deposit(0.0) {}
      /// Standard initializing constructor
      SimulationHit(Position  p, float d) : position(std::move(p)), deposit(d) {}
      /// Copy constructor
      SimulationHit(const SimulationHit& c) : position(c.position), deposit(c.deposit) {}
      /// Standard Destructor
      ~SimulationHit()  {}
      /// Assignment operator
      SimulationHit& operator=(const SimulationHit& c)  {
        if ( this != &c )  {
          position = c.position;
          deposit = c.deposit;
        }
        return *this;
      }
    };
  }

/// Hit conversion function  \ingroup DD4HEP_EVE
static void* _convertHitCollection(const char* source)  {
  typedef dd4hep::DDEve::SimulationHit SimulationHit;
  const std::vector<SimpleHit*>* c = (std::vector<SimpleHit*>*)source;
  auto* pv = new std::vector<SimulationHit>();
  if ( source && c->size() > 0 )   {
    for(auto k : *c)   {
      auto* trh = dynamic_cast<SimpleTracker::Hit*>(k);
      if ( trh )   {
        pv->emplace_back(SimulationHit(trh->position, trh->energyDeposit));
        continue;
      }
      auto* cah = dynamic_cast<SimpleCalorimeter::Hit*>(k);
      if ( cah )   {
        pv->emplace_back(SimulationHit(cah->position, cah->energyDeposit));
        continue;
      }
    }
  }
  return pv;
}

#include "DD4hep/Factories.h"
using namespace dd4hep::detail;
DECLARE_CONSTRUCTOR(DDEve_DDG4CollectionAccess,_convertHitCollection)
