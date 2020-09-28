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
#include "LCIOEventHandler.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Factories.h"

#include "IO/LCReader.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/SimTrackerHit.h"
#include "EVENT/MCParticle.h"

#include "TSystem.h"
#include "TGMsgBox.h"

// C/C++ include files
#include <stdexcept>
#include <climits>

using namespace std;
using namespace lcio;
using namespace dd4hep;
using namespace EVENT;
using namespace IMPL;

auto _fill(const SimTrackerHit* hit, DDEveHit* target) -> const void*   {
  const double* p = hit->getPosition();
  target->x = p[0];
  target->y = p[1];
  target->z = p[2];
  target->deposit = hit->getEDep();
  return hit;
}
auto _fill(const SimCalorimeterHit* hit, DDEveHit* target) -> const void*   {
  const float* p = hit->getPosition();
  target->x = p[0];
  target->y = p[1];
  target->z = p[2];
  target->deposit = hit->getEnergy();
  return hit;
}

static auto _convertHitFunc(const LCObject* source, DDEveHit* target) -> const void*  {
  const auto* t = dynamic_cast<const SimTrackerHit*>(source);
  if (t && _fill(t,target)) return t;
  const auto* c = dynamic_cast<const SimCalorimeterHit*>(source);
  if (c && _fill(c,target)) return c;
  return nullptr;
}
static auto _convertParticleFunc(const LCObject* source, DDEveParticle* target) -> const void*  {
  if ( source && target )  {}
  return nullptr;
}

static auto _create(const char*) -> void*  {
  EventHandler* eh = new LCIOEventHandler();
  return eh;
}
using namespace dd4hep::detail;
DECLARE_CONSTRUCTOR(DDEve_LCIOEventHandler,_create)

/// Standard constructor
LCIOEventHandler::LCIOEventHandler() : EventHandler(), m_lcReader(nullptr), m_event(nullptr) {
  m_lcReader = LCFactory::getInstance()->createLCReader() ;
}

/// Default destructor
LCIOEventHandler::~LCIOEventHandler()   {
  delete m_lcReader;
}

/// Access the number of events on the current input data source (-1 if no data source connected)
auto LCIOEventHandler::numEvents() const -> long   {
  if ( hasFile() )  {
    return m_lcReader->getNumberOfEvents();
  }
  return -1;
}

/// Access to the collection type by name
auto LCIOEventHandler::collectionType(const std::string& /* collection */) const -> EventHandler::CollectionType {
  return CALO_HIT_COLLECTION;
#if 0
  if ( cl == cl_calo ) return CALO_HIT_COLLECTION;
  else if ( cl == cl_tracker ) return TRACKER_HIT_COLLECTION;
  else if ( cl == cl_particles ) return PARTICLE_COLLECTION;
  else return NO_COLLECTION;
#endif
}

/// Call functor on hit collection
auto LCIOEventHandler::collectionLoop(const std::string& collection, DDEveHitActor& actor) -> size_t   {
  auto ibr = m_branches.find(collection);
  if ( ibr != m_branches.end() )   {
    LCCollection* c = (*ibr).second;
    if ( c )  {
      DDEveHit hit;
      int n = c->getNumberOfElements();
      actor.setSize(n);
      for(int i=0; i<n; ++i)  {
        LCObject* ptr = c->getElementAt(i);
        if ( _convertHitFunc(ptr,&hit) )    {
          actor(hit);
        }
      }
      return n;
    }
  }
  return 0;
}

/// Loop over collection and extract particle data
auto LCIOEventHandler::collectionLoop(const std::string& collection, DDEveParticleActor& actor) -> size_t    {
  auto ibr = m_branches.find(collection);
  if ( ibr != m_branches.end() )   {
    LCCollection* c = (*ibr).second;
    if ( c )  {
      DDEveParticle part;
      int n = c->getNumberOfElements();
      actor.setSize(n);
      for(int i=0; i<n; ++i)  {
        LCObject* ptr = c->getElementAt(i);
        if ( _convertParticleFunc(ptr,&part) )    {
          actor(part);
        }
      }
      return n;
    }
  }
  return 0;
}

/// Open new data file
auto LCIOEventHandler::Open(const std::string&, const std::string& name) -> bool   {
  if ( m_hasFile ) m_lcReader->close();
  m_hasFile = false;
  m_hasEvent = false;
  m_event = nullptr;
  m_branches.clear();
  m_lcReader->open(name);
  m_hasFile = true;
  return true;
}

/// Load the next event
auto LCIOEventHandler::NextEvent() -> bool   {
  m_data.clear();
  m_hasEvent = false;
  m_branches.clear();
  m_data.clear();
  if ( hasFile() )  {
    m_event = m_lcReader->readNextEvent();
    if ( m_event )   {
      typedef std::vector<std::string> _S;
      const _S* collnames = m_event->getCollectionNames();
      for(const auto & collname : *collnames) {
        LCCollection* c = m_event->getCollection(collname);
        m_data[c->getTypeName()].push_back(make_pair(collname.c_str(),c->getNumberOfElements()));
        m_branches[collname] = c;
      }
      m_hasEvent = true;
      return true;
    }
    throw runtime_error("+++ EventHandler::readEvent: Failed to read event");
  }
  throw runtime_error("+++ EventHandler::readEvent: No file open!");
}

/// Load the previous event
auto LCIOEventHandler::PreviousEvent() -> bool   {
  throw runtime_error("+++ This version of the LCIO reader can only access files sequentially!\n"
                      "+++ Access to the previous event is not supported.");
}

/// Goto a specified event in the file
auto LCIOEventHandler::GotoEvent(long /* event_number */) -> bool   {
  throw runtime_error("+++ This version of the LCIO reader can only access files sequentially!\n"
                      "+++ Random access is not supported.");
}
