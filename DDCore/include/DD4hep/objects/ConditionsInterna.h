// $Id: $
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
//
// NOTE:
//
// This is an internal include file. It should only be included to
// instantiate code. Otherwise the Conditions include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_CONDITIONS_CONDITIONINTERNA_H
#define DD4HEP_CONDITIONS_CONDITIONINTERNA_H

// Framework include files
#include "DD4hep/BasicGrammar.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/Detector.h"
#include "DD4hep/NamedObject.h"
#include "DD4hep/objects/OpaqueData_inl.h"

// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class ConditionsPool;

    /// The data class behind a conditions container handle.
    /**
     *  See ConditionsInterna.cpp for the implementation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsLoader {
    protected:
      typedef Condition::key_type key_type;
      typedef Condition::iov_type iov_type;
      /// Protected destructor
      virtual ~ConditionsLoader();

    public:
      /// Addreference count. Use object
      virtual void addRef() = 0;
      /// Release object. The client may not use any reference any further.
      virtual void release() = 0;
      /// Access the conditions loading mechanism
      virtual Condition get(key_type key, const iov_type& iov) = 0;
      /// Access the conditions loading mechanism. Only conditions in the user pool will be accessed.
      virtual Condition get(key_type key, const UserPool& pool) = 0;
    };

    /// Conditions internal namespace declaration
    /** Internally defined datastructures are not presented to the
     *  user directly, but are used by dedicated views.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    namespace Interna {

      // Forward declarations
      class ConditionContainer;
      class ConditionObject;

      /// The data class behind a conditions handle.
      /**
       *  See ConditionsInterna.cpp for the implementation.
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class ConditionObject : public NamedObject {
      public:
        /// Forward definition of the key type
        typedef Condition::key_type key_type;
        /// Forward definition of the iov type
        typedef Condition::iov_type iov_type;
        /// Forward definition of the object properties
        typedef unsigned int BitMask;
        /// Forward definition of the object mask manipulator
        typedef ReferenceBitMask<BitMask> MaskManip;

        /// Condition value (in string form)
        std::string value;
        /// Condition validity (in string form)
        std::string validity;
        /// Condition address
        std::string address;
        /// Comment string
        std::string comment;
        /// Data block
        OpaqueDataBlock data;
        /// Reference to conditions pool
        ConditionsPool* pool;
        /// Interval of validity
        const iov_type* iov;
        /// Hash value of the name
        key_type hash;
        /// Flags
        BitMask flags;
        /// Reference count
        int refCount;
        /// Standard constructor
        ConditionObject(const std::string& nam = "", const std::string& tit = "");
        /// Standard Destructor
        virtual ~ConditionObject();
        /// Move data content: 'from' will be reset to NULL
        ConditionObject& move(ConditionObject& from);
        /// Access safely the IOV
        const iov_type* iovData() const;
        /// Access safely the IOV-type
        const IOVType* iovType() const;
        /// Check if object is already bound....
        bool is_bound() const { return data.is_bound(); }
        bool is_traced() const { return true; }
        void setFlag(int option) { flags |= option; }
        void unFlag(int option) { flags &= ~option; }
      };

      /// The data class behind a conditions container handle.
      /**
       *  See ConditionsInterna.cpp for the implementation.
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class ConditionContainer : public NamedObject {
      public:
        /// Forward defintion of the key type
        typedef Condition::key_type key_type;
        /// Forward definition of the iov type
        typedef Condition::iov_type iov_type;
        /// Forward definition of the mapping type
        typedef std::pair<key_type, std::string> key_value;
        /// Definition of the keys
        typedef std::map<key_type, key_value> Keys;

      public:
        /// Standard constructor
        ConditionContainer(Geometry::DetElementObject* parent);
        /// Default destructor
        virtual ~ConditionContainer();

#ifdef __CINT__
        Handle<NamedObject> detector;
#else
        /// The hosting detector element
        DetElement detector;

        /// Access to condition objects by key and IOV.
        Condition get(const std::string& condition_key, const iov_type& iov);

        /// Access to condition objects directly by their hash key.
        Condition get(key_type hash_key, const iov_type& iov);

        /// Access to condition objects. Only conditions in the pool are accessed.
        Condition get(const std::string& condition_key, const UserPool& iov);

        /// Access to condition objects. Only conditions in the pool are accessed.
        Condition get(key_type condition_key, const UserPool& iov);
#endif
      public:
        /// Known keys of conditions in this container
        Keys keys;

        /// Add a new key to the conditions access map
        void addKey(const std::string& key_value);
        /// Add a new key to the conditions access map: Allow for alias if key_val != data_val
        void addKey(const std::string& key_value, const std::string& data_value);
      };

    } /* End namespace Interna    */

    /// Bind the data of the conditions object to a given format.
    template <typename T> T& Condition::bind() {
      Object* o = access();
      return o->data.set<T>(o->value);
    }
    /// Generic getter. Specify the exact type, not a polymorph type
    template <typename T> T& Condition::get() { return access()->data.get<T>(); }
    /// Generic getter (const version). Specify the exact type, not a polymorph type
    template <typename T> const T& Condition::get() const { return access()->data.get<T>(); }

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#define DD4HEP_DEFINE_CONDITIONS_TYPE(x)           \
  namespace DD4hep {                               \
    namespace Conditions {                         \
      template x&       Condition::bind<x>();      \
      template x&       Condition::get<x>();       \
      template const x& Condition::get<x>() const; \
    }                                              \
  }

#define DD4HEP_DEFINE_CONDITIONS_TYPE_DUMMY(x)        \
  namespace DD4hep {                                  \
    namespace Parsers {                               \
      int parse(x&, const std::string&) { return 1; } \
    }                                                 \
  }                                                   \
  DD4HEP_DEFINE_CONDITIONS_TYPE(x)

#define DD4HEP_DEFINE_EXTERNAL_CONDITIONS_TYPE(x)     \
  namespace DD4hep {                                  \
    namespace Conditions {                            \
      template <> x&       Condition::bind<x>();      \
      template <> x&       Condition::get<x>();       \
      template <> const x& Condition::get<x>() const; \
    }                                                 \
  }

#if defined(DD4HEP_HAVE_ALL_PARSERS)
#define DD4HEP_DEFINE_CONDITIONS_CONT(x)                           \
  DD4HEP_DEFINE_CONDITIONS_TYPE(x)                                 \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::vector<x>)                    \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::list<x>)                      \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::set<x>)                       \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::deque<x>)                     \
  DD4HEP_DEFINE_CONDITIONS_TYPE(DD4hep::Primitive<x>::int_map_t)   \
  DD4HEP_DEFINE_CONDITIONS_TYPE(DD4hep::Primitive<x>::ulong_map_t) \
  DD4HEP_DEFINE_CONDITIONS_TYPE(DD4hep::Primitive<x>::string_map_t)

#define DD4HEP_DEFINE_CONDITIONS_U_CONT(x) \
  DD4HEP_DEFINE_CONDITIONS_CONT(x)         \
  DD4HEP_DEFINE_CONDITIONS_CONT(unsigned x)

#else

#define DD4HEP_DEFINE_CONDITIONS_CONT(x)                         \
  DD4HEP_DEFINE_CONDITIONS_TYPE(x)                               \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::vector<x>)                  \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::list<x>)                    \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::set<x>)                     \
  DD4HEP_DEFINE_CONDITIONS_TYPE(DD4hep::Primitive<x>::int_map_t) \
  DD4HEP_DEFINE_CONDITIONS_TYPE(DD4hep::Primitive<x>::string_map_t)

#define DD4HEP_DEFINE_CONDITIONS_U_CONT(x) DD4HEP_DEFINE_CONDITIONS_CONT(x)

#endif  //  DD4HEP_HAVE_ALL_PARSERS
#endif  /* DD4HEP_CONDITIONS_CONDITIONINTERNA_H    */
