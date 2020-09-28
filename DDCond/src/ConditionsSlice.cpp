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



#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cond;

/// Initializing constructor
ConditionsSlice::ConditionsSlice(ConditionsManager mgr) : manager(mgr)
{
  InstanceCount::increment(this);  
}

/// Initializing constructor
ConditionsSlice::ConditionsSlice(ConditionsManager mgr,
                                 shared_ptr<ConditionsContent>  cont)
  : manager(mgr), content(std::move(cont))
{
  InstanceCount::increment(this);  
}

/// Copy constructor (Special, partial copy only. Hence no assignment!)
ConditionsSlice::ConditionsSlice(const ConditionsSlice& copy)
  : manager(copy.manager), content(copy.content)
{
  InstanceCount::increment(this);  
}

/// Default destructor. 
ConditionsSlice::~ConditionsSlice()   {
  reset();
  InstanceCount::decrement(this);  
}

/// Set flag to not reference the used pools during prepare (and drop possibly pending)
void ConditionsSlice::derefPools()     {
  used_pools.clear(); // Drop all refs possibly pending
  this->flags &= ~REF_POOLS;
}

/// Access the combined IOV of the slice from the pool
auto ConditionsSlice::iov()  const -> const IOV&    {
  if ( pool.get() ) return pool->validity();
  dd4hep::except("ConditionsSlice",
                 "pool-iov: Failed to access validity of non-existing pool.");
  return pool->validity();
}

/// Clear the conditions access and the user pool.
void ConditionsSlice::reset()   {
  derefPools();
  if ( pool.get() ) pool->clear();
}

/// Local optimization: Insert a set of conditions to the slice AND register them to the conditions manager.
auto ConditionsSlice::manage(ConditionsPool* p, Condition condition, ManageFlag flg) -> bool   {
  if ( condition.isValid() )  {
    bool ret = false;
    if ( flg&REGISTER_MANAGER )  {
      if ( !p )   {
        dd4hep::except("ConditionsSlice",
                       "manage_condition: Cannot access conditions pool according to IOV:%s.",
                       pool->validity().str().c_str());
      }
      ret = manager.registerUnlocked(*p,condition);
      if ( !ret )  {
        dd4hep::except("ConditionsSlice",
                       "manage_condition: Failed to register condition %016llx according to IOV:%s.",
                       condition->hash, pool->validity().str().c_str());
      }
    }
    if ( flg&REGISTER_POOL )  {
      ret = pool->insert(condition);
      if ( !ret )  {
        dd4hep::except("ConditionsSlice",
                       "manage_condition: Failed to register condition %016llx to user pool with IOV:%s.",
                       condition->hash, pool->validity().str().c_str());
      }
    }
    return ret;
  }
  dd4hep::except("ConditionsSlice",
                 "manage_condition: Cannot manage invalid condition!");
  return false;
}

/// Insert a set of conditions to the slice AND register them to the conditions manager.
auto ConditionsSlice::manage(Condition condition, ManageFlag flg) -> bool    {
  ConditionsPool* p = (flg&REGISTER_MANAGER) ? manager.registerIOV(pool->validity()) : nullptr;
  return manage(p, condition, flg);
}

/// Access all conditions from a given detector element
auto ConditionsSlice::get(DetElement detector)  const -> vector<Condition>  {
  return pool->get(detector,FIRST_ITEM,LAST_ITEM);
}

/// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
auto ConditionsSlice::get(DetElement detector,
                                       Condition::itemkey_type lower,
                                       Condition::itemkey_type upper)  const -> vector<Condition>  {
  return pool->get(detector, lower, upper);
}

/// ConditionsMap overload: Add a condition directly to the slice
auto ConditionsSlice::insert(DetElement detector, Condition::itemkey_type key, Condition condition) -> bool   {
  if ( condition.isValid() )  {
    ConditionsPool* p = manager.registerIOV(pool->validity());
    if ( !p )   {
      dd4hep::except("ConditionsSlice",
                     "manage_condition: Cannot access conditions pool according to IOV:%s.",
                     pool->validity().str().c_str());
    }
    bool ret = manager.registerUnlocked(*p,condition);
    if ( !ret )  {
      dd4hep::except("ConditionsSlice",
                     "manage_condition: Failed to register condition %016llx according to IOV:%s.",
                     condition->hash, pool->validity().str().c_str());
    }
    return pool->insert(detector, key, condition);
  }
  dd4hep::except("ConditionsSlice",
                 "insert_condition: Cannot insert invalid condition to the user pool!");
  return false;
}

/// ConditionsMap overload: Access a condition
auto ConditionsSlice::get(DetElement detector, Condition::itemkey_type key)  const -> Condition   {
  return pool->get(detector, key);
}

/// ConditionsMap overload: Interface to scan data content of the conditions mapping
void ConditionsSlice::scan(const Condition::Processor& processor) const   {
  pool->scan(processor);
}

/// ConditionsMap overload: Interface to partially scan data content of the conditions mapping
void ConditionsSlice::scan(DetElement         detector,
                           Condition::itemkey_type       lower,
                           Condition::itemkey_type       upper,
                           const Condition::Processor&   processor) const  {
  pool->scan(detector, lower, upper, processor);
}

namespace  {
  
  struct SliceOper  : public ConditionsSelect  {
    ConditionsContent& content;
    SliceOper(ConditionsContent& c) : content(c) {}
    void operator()(const ConditionsIOVPool::Elements::value_type& v)    {
      v.second->select_all(*this);
    }
    auto operator()(Condition::Object* c)  const -> bool override  {
      if ( 0 == (c->flags&Condition::DERIVED) )   {
#if !defined(DD4HEP_MINIMAL_CONDITIONS)
        content.addLocation(c->hash,c->address);
#endif
        return true;
      }
      return true;
    }
    /// Return number of conditions selected
    [[nodiscard]] auto size()  const -> size_t override { return content.conditions().size(); }
  };
}

/// Populate the conditions slice from the conditions manager (convenience)
void dd4hep::cond::fill_content(ConditionsManager mgr,
                                ConditionsContent& content,
                                const IOVType& typ)
{
  ConditionsIOVPool* iovPool = mgr.iovPool(typ);
  ConditionsIOVPool::Elements& pools = iovPool->elements;
  for_each(begin(pools),end(pools),SliceOper(content));
}
