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
#ifndef DDCOND_CONDITIONSMAPPEDUSERPOOL_H
#define DDCOND_CONDITIONSMAPPEDUSERPOOL_H

// Framework include files
#include "DDCond/ConditionsPool.h"
#include "DD4hep/ConditionsMap.h"

// C/C++ include files
#include <map>
#include <unordered_map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep::cond {

    /// Forward declarations
    class ConditionsDataLoader;
    
    /// Class implementing the conditions user pool for a given IOV type
    /**
     *
     *  Please note:
     *  Users should not directly interact with object instances of this type.
     *  Data are not thread protected and interaction may cause serious harm.
     *  Only the ConditionsManager implementation should interact with
     *  this class or any subclass to ensure data integrity.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    template<typename MAPPING> 
    class ConditionsMappedUserPool : public UserPool    {
      typedef MAPPING Mapping;
      Mapping               m_conditions;
      /// IOV Pool as data source
      ConditionsIOVPool*    m_iovPool = nullptr;
      /// The loader to access non-existing conditions
      ConditionsDataLoader* m_loader = nullptr;

      /// Internal helper to find conditions
      [[nodiscard]] auto i_findCondition(Condition::key_type key)  const -> Condition::Object*;

      /// Internal insertion helper
      auto i_insert(Condition::Object* o) -> bool;

    public:
      /// Default constructor
      ConditionsMappedUserPool(ConditionsManager mgr, ConditionsIOVPool* pool);
      /// Default destructor
      ~ConditionsMappedUserPool() override;
      /// Print pool content
      void print(const std::string& opt)   const  override;
      /// Total entry count
      [[nodiscard]] auto size()  const -> size_t  override;
      /// Full cleanup of all managed conditions.
      void clear()  override;
      /// Check a condition for existence
      [[nodiscard]] auto exists(Condition::key_type hash)  const -> bool  override;
      /// Check a condition for existence
      [[nodiscard]] auto exists(const ConditionKey& key)  const -> bool  override;
      /// Check if a condition exists in the pool and return it to the caller
      [[nodiscard]] auto get(Condition::key_type hash)  const -> Condition  override;
      /// Check if a condition exists in the pool and return it to the caller     
      [[nodiscard]] auto get(const ConditionKey& key)  const -> Condition  override;

      /// Remove condition by key from pool.
      auto remove(Condition::key_type hash_key) -> bool  override;
      /// Remove condition by key from pool.
      auto remove(const ConditionKey& key) -> bool  override;
      /// Do single insertion of condition including registration to the manager
      auto registerOne(const IOV& iov, Condition cond) -> bool  override;
      /// Do block insertions of conditions with identical IOV including registration to the manager
      auto registerMany(const IOV& iov, const std::vector<Condition>& values) -> size_t override;
      /// Register a new condition to this pool
      auto insert(Condition cond) -> bool  override;

      /// ConditionsMap overload: Add a condition directly to the slice
      auto insert(DetElement detector, Condition::itemkey_type key, Condition condition) -> bool  override;
      /// ConditionsMap overload: Access a condition
      [[nodiscard]] auto get(DetElement detector, Condition::itemkey_type key)  const -> Condition  override;
      /// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
      [[nodiscard]] auto get(DetElement detector,
                                         Condition::itemkey_type lower,
                                         Condition::itemkey_type upper)  const -> std::vector<Condition> override;
      /// Access all conditions within the key range of a detector element
      [[nodiscard]] auto get(Condition::key_type lower,
                                         Condition::key_type upper)  const -> std::vector<Condition>  override;

      /// ConditionsMap overload: Interface to scan data content of the conditions mapping
      void scan(const Condition::Processor& processor) const  override;
      /// ConditionsMap overload: Interface to scan data content of the conditions mapping
      void scan(Condition::key_type lower,
                        Condition::key_type upper,
                        const Condition::Processor& processor) const  override;
      /// ConditionsMap overload: Interface to scan data content of the conditions mapping
      void scan(DetElement detector,
                        Condition::itemkey_type lower,
                        Condition::itemkey_type upper,
                        const Condition::Processor& processor) const  override;

      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      auto prepare(const IOV&                  required,
                                                ConditionsSlice&            slice,
                                                ConditionUpdateUserContext* user_param) -> ConditionsManager::Result  override;

      /// Evaluate and register all derived conditions from the dependency list
      auto compute(const Dependencies&         dependencies,
                             ConditionUpdateUserContext* user_param,
                             bool                        force) -> size_t  override;

      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      auto load   (const IOV&              required,
                                                ConditionsSlice&        slice,
                                                ConditionUpdateUserContext* user_param) -> ConditionsManager::Result  override;
      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      auto compute(const IOV&                  required,
                                                ConditionsSlice&            slice,
                                                ConditionUpdateUserContext* user_param) -> ConditionsManager::Result  override;
    };

  }      /* End namespace dd4hep                   */
#endif /* DDCOND_CONDITIONSMAPPEDUSERPOOL_H      */

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
//#include "DDCond/ConditionsMappedPool.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/InstanceCount.h"

#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsSelectors.h"
#include "DDCond/ConditionsDataLoader.h"
#include "DDCond/ConditionsManagerObject.h"
#include "DDCond/ConditionsDependencyHandler.h"

#include <mutex>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cond;

namespace {

  class SimplePrint : public Condition::Processor {
    /// Conditions callback for object processing
    [[nodiscard]] auto process(Condition)  const -> int override    { return 1; }
    /// Conditions callback for object processing
    auto operator()(Condition)  const -> int override { return 1; }
    /// Conditions callback for object processing in maps
    auto operator()(const pair<Condition::key_type,Condition>& i)  const -> int override  {
      Condition c = i.second;
      printout(INFO,"UserPool","++ %16llX/%16llX Val:%s %s",i.first, c->hash, c->value.c_str(), c.str().c_str());
      return 1;
    }
  };
  template <typename T> struct MapSelector : public ConditionsSelect {
    T& m;
    MapSelector(T& o) : m(o) {}
    auto operator()(Condition::Object* o)  const -> bool override
    { return m.emplace(o->hash,o).second;    }
  };
  template <typename T> auto mapSelector(T& container) -> MapSelector<T>
  {  return MapSelector<T>(container);       }

  template <typename T> struct Inserter {
    T& m;
    IOV* iov;
    Inserter(T& o, IOV* i=nullptr) : m(o), iov(i) {}
    void operator()(const Condition& c)  {
      Condition::Object* o = c.ptr();
      m.emplace(o->hash,o);
      if ( iov ) iov->iov_intersection(o->iov->key());
    }
    void operator()(const pair<Condition::key_type,Condition>& e) { (*this)(e.second);  }
  };
}

/// Default constructor
template<typename MAPPING>
ConditionsMappedUserPool<MAPPING>::ConditionsMappedUserPool(ConditionsManager mgr, ConditionsIOVPool* pool) 
  : UserPool(mgr), m_iovPool(pool)
{
  InstanceCount::increment(this);
  if ( mgr.isValid() )  {
    if ( m_iovPool )  {
      m_iov.iovType = m_iovPool->type;
      m_loader = mgr->loader();
      if ( m_loader ) return;
      except("UserPool","The conditions manager is not properly setup. No conditions loader present.");
    }
    except("UserPool","FAILED to create mapped user pool. [Invalid IOV pool]");
  }
  except("UserPool","FAILED to create mapped user pool. [Invalid conditions manager]");
}

/// Default destructor
template<typename MAPPING>
ConditionsMappedUserPool<MAPPING>::~ConditionsMappedUserPool()  {
  clear();
  InstanceCount::decrement(this);
}

template<typename MAPPING> inline auto
ConditionsMappedUserPool<MAPPING>::i_findCondition(Condition::key_type key)  const -> Condition::Object* {
  auto i=m_conditions.find(key);
#if 0
  if ( i == m_conditions.end() )  {
    print("*"); // This causes CTEST to bail out, due too much output!
  }
#endif
  return i != m_conditions.end() ? (*i).second : 0;
}

template<typename MAPPING> inline auto
ConditionsMappedUserPool<MAPPING>::i_insert(Condition::Object* o) -> bool   {
  int ret = m_conditions.emplace(o->hash,o).second;
  if ( flags&PRINT_INSERT )  {
    printout(INFO,"UserPool","++ %s condition [%016llX]: %s [%s].",
             ret ? "Successfully inserted" : "FAILED to insert", o->hash,
#if defined(DD4HEP_CONDITIONS_HAVE_NAME)
             o->GetName(), o->GetTitle());
#else
    "", "");
#endif
  }
  return ret;
}
  
/// Total entry count
template<typename MAPPING>
auto ConditionsMappedUserPool<MAPPING>::size()  const -> size_t  {
  return  m_conditions.size();
}

/// Print pool content
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::print(const string& opt)   const  {
  const IOV* iov = &m_iov;
  printout(INFO,"UserPool","+++ %s Conditions for USER pool with IOV: %-32s [%4d entries]",
           opt.c_str(), iov->str().c_str(), size());
  if ( opt == "*" )  {
    SimplePrint prt;
    scan(prt);
  }
}

/// Full cleanup of all managed conditions.
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::clear()   {
  if ( flags&PRINT_CLEAR )  {
    printout(INFO,"UserPool","++ Cleared %ld conditions from pool.",m_conditions.size());
  }
  m_iov = IOV(nullptr);
  m_conditions.clear();
}

/// Check a condition for existence
template<typename MAPPING>
auto ConditionsMappedUserPool<MAPPING>::exists(Condition::key_type hash)  const -> bool   {
  return i_findCondition(hash) != nullptr;
}

/// Check a condition for existence
template<typename MAPPING>
auto ConditionsMappedUserPool<MAPPING>::exists(const ConditionKey& key)  const -> bool   {
  return i_findCondition(key.hash) != nullptr;
}

/// Check if a condition exists in the pool and return it to the caller
template<typename MAPPING>
auto ConditionsMappedUserPool<MAPPING>::get(Condition::key_type hash)  const -> Condition   {
  return i_findCondition(hash);
}

/// Check if a condition exists in the pool and return it to the caller     
template<typename MAPPING>
auto ConditionsMappedUserPool<MAPPING>::get(const ConditionKey& key)  const -> Condition   {
  return i_findCondition(key.hash);
}

/// Do block insertions of conditions with identical IOV including registration to the manager
template<typename MAPPING> auto
ConditionsMappedUserPool<MAPPING>::registerOne(const IOV& iov,
                                               Condition cond) -> bool   {
  if ( iov.iovType )   {
    ConditionsPool* pool = m_manager.registerIOV(*iov.iovType,iov.keyData);
    if ( pool )   {
      return m_manager.registerUnlocked(*pool, cond);
    }
    except("UserPool","++ Failed to register IOV: %s",iov.str().c_str());
  }
  except("UserPool","++ Cannot register conditions with invalid IOV.");
  return false;
}

/// Do block insertions of conditions with identical IOV including registration to the manager
template<typename MAPPING> auto
ConditionsMappedUserPool<MAPPING>::registerMany(const IOV& iov,
                                                const vector<Condition>& conds) -> size_t   {
  if ( iov.iovType )   {
    ConditionsPool* pool = m_manager.registerIOV(*iov.iovType,iov.keyData);
    if ( pool )   {
      size_t result = m_manager.blockRegister(*pool, conds);
      if ( result == conds.size() )   {
        for(auto c : conds) i_insert(c.ptr());
        return result;
      }
      except("UserPool","++ Conditions registration was incomplete: "
             "registerd only  %ld out of %ld conditions.",
             result, conds.size());
    }
    except("UserPool","++ Failed to register IOV: %s",iov.str().c_str());
  }
  except("UserPool","++ Cannot register conditions with invalid IOV.");
  return 0;
}

/// Register a new condition to this pool
template<typename MAPPING>
auto ConditionsMappedUserPool<MAPPING>::insert(Condition cond) -> bool   {
  bool result = i_insert(cond.ptr());
  if ( result ) return true;
  except("UserPool","++ Attempt to double insert condition: %16llX Name:%s", cond->hash, cond.name());
  return false;
}

/// ConditionsMap overload: Add a condition directly to the slice
template<typename MAPPING>
auto ConditionsMappedUserPool<MAPPING>::insert(DetElement detector,
                                               unsigned int item_key,
                                               Condition cond) -> bool   {
  cond->hash = ConditionKey::KeyMaker(detector.key(),item_key).hash;
  return insert(cond);
}

/// ConditionsMap overload: Access a condition
template<typename MAPPING>
auto ConditionsMappedUserPool<MAPPING>::get(DetElement detector, unsigned int item_key)  const -> Condition  {
  return get(ConditionKey::KeyMaker(detector.key(), item_key).hash);
}
  
/// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
template<typename MAPPING> auto
ConditionsMappedUserPool<MAPPING>::get(DetElement detector,
                                       Condition::itemkey_type lower,
                                       Condition::itemkey_type upper)  const -> std::vector<Condition> {
  Condition::detkey_type det_key = detector.key();
  return this->get(ConditionKey::KeyMaker(det_key,lower).hash,
                   ConditionKey::KeyMaker(det_key,upper).hash);
}

/// Specialization for std::map: Access all conditions within a given key range
template<typename MAPPING> auto
ConditionsMappedUserPool<MAPPING>::get(Condition::key_type lower, Condition::key_type upper)   const -> std::vector<Condition>  {
  vector<Condition> result;
  if ( !m_conditions.empty() )   {
    auto first = m_conditions.lower_bound(lower);
    for(; first != m_conditions.end(); ++first )  {
      if ( (*first).first > upper ) break;
      result.emplace_back((*first).second);
    }
  }
  return result;
}

/// ConditionsMap overload: Interface to scan data content of the conditions mapping
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::scan(const Condition::Processor& processor) const  {
  for( const auto& i : m_conditions )
    processor(i.second);
}

/// ConditionsMap overload: Interface to scan data content of the conditions mapping
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::scan(DetElement detector,
                                             Condition::itemkey_type lower,
                                             Condition::itemkey_type upper,
                                             const Condition::Processor& processor) const
{
  Condition::detkey_type det_key = detector.key();
  scan(ConditionKey::KeyMaker(det_key,lower).hash,
       ConditionKey::KeyMaker(det_key,upper).hash,
       processor);
}

/// ConditionsMap overload: Interface to scan data content of the conditions mapping
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::scan(Condition::key_type lower,
                                             Condition::key_type upper,
                                             const Condition::Processor& processor) const
{
  auto first = m_conditions.lower_bound(lower);
  for(; first != m_conditions.end() && (*first).first <= upper; ++first )
    processor((*first).second);
}

/// Remove condition by key from pool.
template<typename MAPPING>
auto ConditionsMappedUserPool<MAPPING>::remove(const ConditionKey& key) -> bool   {
  return remove(key.hash);
}

/// Remove condition by key from pool.
template<typename MAPPING>
auto ConditionsMappedUserPool<MAPPING>::remove(Condition::key_type hash_key) -> bool    {
  auto i = m_conditions.find(hash_key);
  if ( i != m_conditions.end() ) {
    m_conditions.erase(i);
    return true;
  }
  return false;
}

/// Evaluate and register all derived conditions from the dependency list
template<typename MAPPING>
auto ConditionsMappedUserPool<MAPPING>::compute(const Dependencies& deps,
                                                  ConditionUpdateUserContext* user_param,
                                                  bool force) -> size_t
{
  if ( !deps.empty() )  {
    Dependencies missing;
    // Loop over the dependencies and check if they have to be upgraded
    for ( const auto& i : deps )  {
      auto j = m_conditions.find(i.first);
      if ( j != m_conditions.end() )  {
        if ( !force )  {
          Condition::Object* c = (*j).second;
          // Remeber: key ist first, test is second!
          if ( !IOV::key_is_contained(m_iov.keyData,c->iov->keyData) )  {
            /// This condition is no longer valid. remove it!
            /// It will be added again by the handler.
            m_conditions.erase(j);
            missing.emplace(i);
          }
          continue;
        }
        else  {
          m_conditions.erase(j);
        }
      }
      missing.emplace(i);
    }
    if ( !missing.empty() )  {
      ConditionsManagerObject* mgr(m_manager.access());
      ConditionsDependencyHandler handler(mgr, *this, missing, user_param);
      /// 1rst pass: Compute/create the missing condiions
      handler.compute();
      /// 2nd pass:  Resolve missing dependencies
      handler.resolve();
      return handler.num_callback;
    }
  }
  return 0;
}

namespace {
  struct COMP {
    typedef pair<Condition::key_type,const ConditionDependency*>     Dep;
    using Cond = pair<const Condition::key_type, detail::ConditionObject *>;
    using Info = pair<const Condition::key_type, ConditionsLoadInfo *>;
    using Cond2 = pair<const Condition::key_type, Condition>;
    
    auto operator()(const Dep& a,const Cond& b) const -> bool   { return a.first < b.first; }
    auto operator()(const Cond& a,const Dep& b) const -> bool   { return a.first < b.first; }

    auto operator()(const Info& a,const Cond& b) const -> bool  { return a.first < b.first; }
    auto operator()(const Cond& a,const Info& b) const -> bool  { return a.first < b.first; }

    auto operator()(const Info& a,const Cond2& b) const -> bool { return a.first < b.first; }
    auto operator()(const Cond2& a,const Info& b) const -> bool { return a.first < b.first; }
  };
}

template<typename MAPPING> auto
ConditionsMappedUserPool<MAPPING>::prepare(const IOV&                  required, 
                                           ConditionsSlice&            slice,
                                           ConditionUpdateUserContext* user_param) -> ConditionsManager::Result
{
  using CalcMissing = vector<pair<Condition::key_type, ConditionDependency *> >;
  using CondMissing = vector<pair<Condition::key_type, ConditionsLoadInfo *> >;
  const auto& slice_cond = slice.content->conditions();
  const auto& slice_calc = slice.content->derived();
  auto&  slice_miss_cond = slice.missingConditions();
  auto&  slice_miss_calc = slice.missingDerivations();
  bool   do_load         = m_manager->doLoadConditions();
  bool   do_output_miss  = m_manager->doOutputUnloaded();
  IOV    pool_iov(required.iovType);
  ConditionsManager::Result result;

  // This is a critical operation, because we have to ensure the
  // IOV pools are ONLY manipulated by the current thread.
  // Otherwise the selection and the population are unsafe!
  static mutex lock;
  lock_guard<mutex> guard(lock);

  m_conditions.clear();
  slice_miss_cond.clear();
  slice_miss_calc.clear();
  pool_iov.reset().invert();
  m_iovPool->select(required, Operators::mapConditionsSelect(m_conditions), pool_iov);
  m_iov = pool_iov;
  CondMissing cond_missing(slice_cond.size()+m_conditions.size());
  CalcMissing calc_missing(slice_calc.size()+m_conditions.size());

  auto last_cond = set_difference(begin(slice_cond),   end(slice_cond),
                                                   begin(m_conditions), end(m_conditions),
                                                   begin(cond_missing), COMP());
  long num_cond_miss = last_cond-begin(cond_missing);
  cond_missing.resize(num_cond_miss);
  printout((flags&PRINT_LOAD) ? INFO : DEBUG,"UserPool",
           "%ld conditions out of %ld conditions are MISSING.",
           num_cond_miss, slice_cond.size());
  auto last_calc = set_difference(begin(slice_calc),   end(slice_calc),
                                                   begin(m_conditions), end(m_conditions),
                                                   begin(calc_missing), COMP());
  long num_calc_miss = last_calc-begin(calc_missing);
  calc_missing.resize(num_calc_miss);
  printout((flags&PRINT_COMPUTE) ? INFO : DEBUG,"UserPool",
           "%ld derived conditions out of %ld conditions are MISSING.",
           num_calc_miss, slice_calc.size());

  result.loaded   = 0;
  result.computed = 0;
  result.selected = m_conditions.size();
  result.missing  = num_cond_miss+num_calc_miss;
  //
  // Now we load the missing conditions from the conditions loader
  //
  if ( num_cond_miss > 0 )  {
    if ( do_load )  {
      ConditionsDataLoader::LoadedItems loaded;
      size_t updates = m_loader->load_many(required, cond_missing, loaded, pool_iov);
      if ( updates > 0 )  {
        // Need to compute the intersection: All missing entries are required....
        CondMissing load_missing(cond_missing.size()+loaded.size());
        // Note: cond_missing is already sorted (doc of 'set_difference'). No need to re-sort....
        auto load_last = set_difference(begin(cond_missing), last_cond,
                                                         begin(loaded), end(loaded),
                                                         begin(load_missing), COMP());
        long num_load_miss = load_last-begin(load_missing);
        printout(num_load_miss==0 ? DEBUG : ERROR,"UserPool",
                 "%ld out of %ld conditions CANNOT be loaded...",
                 num_load_miss, loaded.size());
        if ( do_output_miss )  {
          copy(begin(load_missing), load_last, inserter(slice_miss_cond, slice_miss_cond.begin()));
          for ( const auto& missing : slice_miss_cond )   {
            printout (ERROR, "TEST", "Unloaded: %s",missing.second->toString().c_str());
          }
        }
        for_each(loaded.begin(),loaded.end(),Inserter<MAPPING>(m_conditions,&m_iov));
        result.loaded  = slice_cond.size()-num_load_miss;
        result.missing = num_load_miss+num_calc_miss;
        if ( cond_missing.size() != loaded.size() )  {
          // ERROR!
          printout(ERROR,"UserPool","Could not load %ld out of %ld conditions. "
                   "[Unhandled error]",cond_missing.size(), slice_cond.size());
        }
      }
    }
    else if ( do_output_miss )  {
      copy(begin(cond_missing), last_cond, inserter(slice_miss_cond, slice_miss_cond.begin()));
      for ( const auto& missing : slice_miss_cond )   {
        printout (ERROR, "TEST", "Unloaded: %s",missing.second->toString().c_str());
      }
    }
  }
  //
  // Now we update the already existing dependencies, which have expired
  //
  if ( num_calc_miss > 0 )  {
    if ( do_load )  {
      map<Condition::key_type,const ConditionDependency*> deps(calc_missing.begin(),last_calc);
      ConditionsDependencyHandler handler(m_manager, *this, deps, user_param);
      /// 1rst pass: Compute/create the missing condiions
      handler.compute();
      /// 2nd pass:  Resolve missing dependencies
      handler.resolve();
      
      result.computed = handler.num_callback;
      result.missing -= handler.num_callback;
      if ( do_output_miss && result.computed < deps.size() )  {
        // Is this cheaper than an intersection ?
        for( auto i = calc_missing.begin(); i != last_calc; ++i )   {
          auto j = m_conditions.find((*i).first);
          if ( j == m_conditions.end() )
            slice_miss_calc.emplace(*i);
        }
      }
    }
    else if ( do_output_miss )  {
      copy(begin(calc_missing), last_calc, inserter(slice_miss_calc, slice_miss_calc.begin()));
    }
  }
  slice.status = result;
  slice.used_pools.clear();
  if ( slice.flags&ConditionsSlice::REF_POOLS )   {
    m_iovPool->select(required, slice.used_pools);
  }
  return result;
}

template<typename MAPPING> auto
ConditionsMappedUserPool<MAPPING>::load(const IOV&                  required, 
                                        ConditionsSlice&            slice,
                                        ConditionUpdateUserContext* /* user_param */) -> ConditionsManager::Result
{
  using CondMissing = vector<pair<Condition::key_type, ConditionsLoadInfo *> >;
  const auto& slice_cond = slice.content->conditions();
  auto&  slice_miss_cond = slice.missingConditions();
  bool   do_load         = m_manager->doLoadConditions();
  bool   do_output_miss  = m_manager->doOutputUnloaded();
  IOV    pool_iov(required.iovType);
  ConditionsManager::Result result;

  // This is a critical operation, because we have to ensure the
  // IOV pools are ONLY manipulated by the current thread.
  // Otherwise the selection and the population are unsafe!
  static mutex lock;
  lock_guard<mutex> guard(lock);

  m_conditions.clear();
  slice_miss_cond.clear();
  pool_iov.reset().invert();
  m_iovPool->select(required, Operators::mapConditionsSelect(m_conditions), pool_iov);
  m_iov = pool_iov;
  CondMissing cond_missing(slice_cond.size()+m_conditions.size());
  auto last_cond = set_difference(begin(slice_cond),   end(slice_cond),
                                                   begin(m_conditions), end(m_conditions),
                                                   begin(cond_missing), COMP());
  long num_cond_miss = last_cond-begin(cond_missing);
  cond_missing.resize(num_cond_miss);
  printout((flags&PRINT_LOAD) ? INFO : DEBUG,"UserPool",
           "Found %ld missing conditions out of %ld conditions.",
           num_cond_miss, slice_cond.size());
  result.loaded   = 0;
  result.computed = 0;
  result.missing  = num_cond_miss;
  result.selected = m_conditions.size();
  //
  // Now we load the missing conditions from the conditions loader
  //
  if ( num_cond_miss > 0 )  {
    if ( do_load )  {
      ConditionsDataLoader::LoadedItems loaded;
      size_t updates = m_loader->load_many(required, cond_missing, loaded, pool_iov);
      if ( updates > 0 )  {
        // Need to compute the intersection: All missing entries are required....
        CondMissing load_missing(cond_missing.size()+loaded.size());
        // Note: cond_missing is already sorted (doc of 'set_difference'). No need to re-sort....
        auto load_last = set_difference(begin(cond_missing), last_cond,
                                                         begin(loaded), end(loaded),
                                                         begin(load_missing), COMP());
        long num_load_miss = load_last-begin(load_missing);
        printout(num_load_miss==0 ? DEBUG : ERROR,"UserPool",
                 "+++ %ld out of %ld conditions CANNOT be loaded... [Not found by loader]",
                 num_load_miss, loaded.size());
        if ( do_output_miss )  {
          copy(begin(load_missing), load_last, inserter(slice_miss_cond, slice_miss_cond.begin()));
        }
        for_each(loaded.begin(),loaded.end(),Inserter<MAPPING>(m_conditions,&m_iov));
        result.loaded  = slice_cond.size()-num_load_miss;
        result.missing = num_load_miss;
        if ( cond_missing.size() != loaded.size() )  {
          // ERROR!
        }
      }
    }
    else if ( do_output_miss )  {
      copy(begin(cond_missing), last_cond, inserter(slice_miss_cond, slice_miss_cond.begin()));
    }
  }
  slice.status = result;
  return result;
}

template<typename MAPPING> auto
ConditionsMappedUserPool<MAPPING>::compute(const IOV&                  required, 
                                           ConditionsSlice&            slice,
                                           ConditionUpdateUserContext* user_param) -> ConditionsManager::Result
{
  using CalcMissing = vector<pair<Condition::key_type, ConditionDependency *> >;
  const auto& slice_calc = slice.content->derived();
  auto&  slice_miss_calc = slice.missingDerivations();
  bool   do_load         = m_manager->doLoadConditions();
  bool   do_output       = m_manager->doOutputUnloaded();
  IOV    pool_iov(required.iovType);
  ConditionsManager::Result result;

  // This is a critical operation, because we have to ensure the
  // IOV pools are ONLY manipulated by the current thread.
  // Otherwise the selection and the population are unsafe!
  static mutex lock;
  lock_guard<mutex> guard(lock);

  slice_miss_calc.clear();
  CalcMissing calc_missing(slice_calc.size()+m_conditions.size());
  auto last_calc = set_difference(begin(slice_calc),   end(slice_calc),
                                                   begin(m_conditions), end(m_conditions),
                                                   begin(calc_missing), COMP());
  long num_calc_miss = last_calc-begin(calc_missing);
  calc_missing.resize(num_calc_miss);
  printout((flags&PRINT_COMPUTE) ? INFO : DEBUG,"UserPool",
           "Found %ld missing derived conditions out of %ld conditions.",
           num_calc_miss, m_conditions.size());

  result.loaded   = 0;
  result.computed = 0;
  result.missing  = num_calc_miss;
  result.selected = m_conditions.size();
  //
  // Now we update the already existing dependencies, which have expired
  //
  if ( num_calc_miss > 0 )  {
    if ( do_load )  {
      map<Condition::key_type,const ConditionDependency*> deps(calc_missing.begin(),last_calc);
      ConditionsDependencyHandler handler(m_manager, *this, deps, user_param);

      /// 1rst pass: Compute/create the missing condiions
      handler.compute();
      /// 2nd pass:  Resolve missing dependencies
      handler.resolve();

      result.computed = handler.num_callback;
      result.missing -= handler.num_callback;
      if ( do_output && result.computed < deps.size() )  {
        for(auto i=calc_missing.begin(); i != last_calc; ++i)   {
          auto j = m_conditions.find((*i).first);
          if ( j == m_conditions.end() )
            slice_miss_calc.emplace(*i);
        }
      }
    }
    else if ( do_output )  {
      copy(begin(calc_missing), last_calc, inserter(slice_miss_calc, slice_miss_calc.begin()));
    }
  }
  slice.status += result;
  slice.used_pools.clear();
  if ( slice.flags&ConditionsSlice::REF_POOLS )   {
    m_iovPool->select(required, slice.used_pools);
  }
  return result;
}


/// Namespace for the AIDA detector description toolkit
namespace dd4hep::cond {

    using umap_t = unordered_map<Condition::key_type, Condition::Object *>;

    /// Access all conditions within a given key range
    /** Specialization necessary, since unordered maps have no lower bound.
     */
    template<> void
    ConditionsMappedUserPool<umap_t>::scan(Condition::key_type lower,
                                           Condition::key_type upper,
                                           const Condition::Processor& processor)   const  {
      for( const auto& e : m_conditions )
        if ( e.second->hash >= lower && e.second->hash <= upper )
          processor(e.second);
    }
    /// Access all conditions within a given key range
    /** Specialization necessary, since unordered maps have no lower bound.
     */
    template<> auto
    ConditionsMappedUserPool<umap_t>::get(Condition::key_type lower, Condition::key_type upper)   const -> std::vector<Condition>  {
      vector<Condition> result;
      for( const auto& e : m_conditions )  {
        if ( e.second->hash >= lower && e.second->hash <= upper )
          result.emplace_back(e.second);
      }
      return result;
    }
  }      /* End namespace dd4hep                   */

namespace {
  template <typename MAPPING>
  auto create_pool(Detector&, int argc, char** argv) -> void*  {
    if ( argc > 1 )  {
      auto* m = (ConditionsManagerObject*)argv[0];
      auto* p = (ConditionsIOVPool*)argv[1];
      UserPool* pool = new ConditionsMappedUserPool<MAPPING>(m, p);
      return pool;
    }
    except("ConditionsMappedUserPool","++ Insufficient arguments: arg[0] = ConditionManager!");
    return nullptr;
  }
}

// Factory for the user pool using a binary tree map
auto create_map_user_pool(Detector& description, int argc, char** argv) -> void*
{  return create_pool<map<Condition::key_type,Condition::Object*> >(description, argc, argv);  }
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_ConditionsMapUserPool, create_map_user_pool)

// Factory for the user pool using a binary tree map
auto create_unordered_map_user_pool(Detector& description, int argc, char** argv) -> void*
{  return create_pool<unordered_map<Condition::key_type,Condition::Object*> >(description, argc, argv);  }
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_ConditionsUnorderedMapUserPool, create_map_user_pool)
