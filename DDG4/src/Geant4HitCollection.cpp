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
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4Data.h"
#include "G4Allocator.hh"

using namespace dd4hep;
using namespace dd4hep::sim;

G4ThreadLocal G4Allocator<Geant4HitWrapper>* HitWrapperAllocator = nullptr;

Geant4HitWrapper::InvalidHit::~InvalidHit() = default;

/// Initializing Constructor
Geant4HitWrapper::HitManipulator::HitManipulator(const ComponentCast& c, const ComponentCast& v)
  : cast(c), vec_type(v) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4HitWrapper::HitManipulator::~HitManipulator() {
  InstanceCount::decrement(this);
}

/// Default destructor
Geant4HitWrapper::~Geant4HitWrapper() {
  if (m_data.first && m_data.second) {
    (*m_data.second->cast.destroy)(m_data.first);
    m_data.first = 0;
  }
}

/// Geant4 required object allocator
auto Geant4HitWrapper::operator new(size_t) -> void* {
  if ( HitWrapperAllocator )
    return HitWrapperAllocator->MallocSingle();
  HitWrapperAllocator = new G4Allocator<Geant4HitWrapper>;
  return HitWrapperAllocator->MallocSingle();
}

/// Geat4 required object destroyer
void Geant4HitWrapper::operator delete(void *p) {
  HitWrapperAllocator->FreeSingle((Geant4HitWrapper*) p);
}

/// Pointer/Object release
auto Geant4HitWrapper::release() -> void* {
  void* p = m_data.first;
  m_data.first = 0;
  m_data.second = manipulator<InvalidHit>();
  return p;
}

/// Pointer/Object release
auto Geant4HitWrapper::releaseData() -> Geant4HitWrapper::Wrapper {
  Wrapper w = m_data;
  m_data.first = 0;
  m_data.second = manipulator<InvalidHit>();
  return w;
}

/// Default destructor
Geant4HitCollection::Compare::~Compare()  = default;

/// Default destructor
Geant4HitCollection::~Geant4HitCollection() {
  m_hits.clear();
  m_keys.clear();
  InstanceCount::decrement(this);
}

/// Type information of the object stored
auto Geant4HitCollection::type() const -> const ComponentCast& {
  return m_manipulator->cast;
}

/// Type information of the vector type for extracting data
auto Geant4HitCollection::vector_type() const -> const ComponentCast& {
  return m_manipulator->vec_type;
}

/// Notification to increase the instance counter
void Geant4HitCollection::newInstance() {
  InstanceCount::increment(this);
}

/// Clear the collection (Deletes all valid references to real hits)
void Geant4HitCollection::clear()   {
  m_lastHit = ULONG_MAX;
  m_hits.clear();
  m_keys.clear();
}

/// Find hit in a collection by comparison of attributes
auto Geant4HitCollection::findHit(const Compare& cmp) -> void*  {
  void* p = nullptr;
  auto i = m_hits.begin();
  if ( m_flags.bits.repeatedLookup && m_lastHit < m_hits.size() )  {
    if ( (p = cmp(*(i+m_lastHit))) != nullptr ) return p;
  }
  for (size_t cnt=0; i != m_hits.end(); ++i, ++cnt)   {
    if ((p = cmp(*i)) != nullptr)  {
      m_lastHit = cnt;
      return p;
    }
  }
  return p;
}

/// Find hit in a collection by comparison of the key
auto Geant4HitCollection::findHitByKey(VolumeID key) -> Geant4HitWrapper*   {
  auto i=m_keys.find(key);
  if ( i == m_keys.end() ) return nullptr;
  m_lastHit = (*i).second;
  return &m_hits.at(m_lastHit);
}

/// Release all hits from the Geant4 container and pass ownership to the caller
void Geant4HitCollection::releaseData(const ComponentCast& cast, std::vector<void*>* result) {
  result->reserve(m_hits.size());
  for (auto & w : m_hits) {
    Manip* m = w.manip();
    if (&cast == &m->cast)
      result->emplace_back(w.release());
    else
      result->emplace_back(m->cast.apply_downCast(cast, w.release()));
  }
  m_lastHit = ULONG_MAX;
  m_keys.clear();
}

/// Release all hits from the Geant4 container. Ownership stays with the container
void Geant4HitCollection::getData(const ComponentCast& cast, std::vector<void*>* result) {
  result->reserve(m_hits.size());
  for (auto & w : m_hits) {
    Manip* m = w.manip();
    if (&cast == &m->cast)
      result->emplace_back(w.data());
    else
      result->emplace_back(m->cast.apply_downCast(cast, w.data()));
  }
}

/// Release all hits from the Geant4 container and pass ownership to the caller
void Geant4HitCollection::releaseHitsUnchecked(std::vector<void*>& result) {
  result.reserve(m_hits.size());
  for (auto & w : m_hits) {
    result.emplace_back(w.release());
  }
  m_lastHit = ULONG_MAX;
  m_keys.clear();
}

/// Release all hits from the Geant4 container. Ownership stays with the container
void Geant4HitCollection::getHitsUnchecked(std::vector<void*>& result) {
  result.reserve(m_hits.size());
  for (auto & w : m_hits) {
    result.emplace_back(w.data());
  }
}
