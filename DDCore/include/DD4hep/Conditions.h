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
#ifndef DD4HEP_GEOMETRY_CONDITION_H
#define DD4HEP_GEOMETRY_CONDITION_H

// Framework include files
#include "DD4hep/Handle.h"

// C/C++ include files
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Grammar definition for type binding
  class BasicGrammar;

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {
    // Forward declarations
    class DetElement;
    class LCDD;
  }

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions {

    // The DetElement is a central object. We alias it here.
    using Geometry::DetElement;
    using Geometry::LCDD;

    // Forward declarations
    class ConditionsManagerObject;
    class ConditionsManager;
    class ConditionsLoader;
    class UserPool;
    class IOVType;
    class IOV;

    /// Conditions intrnal namespace
    namespace Interna {
      class ConditionContainer;
      class ConditionObject;
    }

    class IOVType {
    public:
      enum { UNKNOWN_IOV = ~0x0 } _IOVTypes;

      unsigned int type;
      std::string  name;
      IOVType() : type(UNKNOWN_IOV), name() {}
      ~IOVType() {}
      std::string str() const;
    };

    /// Class describing the interval of validty
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class IOV {
      friend class Condition;

    private:
      /// Initializing constructor: Does not set reference to IOVType !
      explicit IOV();

    public:
      /// Key definition
      typedef long Key_first_type;
      typedef long Key_second_type;
      typedef std::pair<Key_first_type, Key_second_type> Key;

      const IOVType* iovType;
      Key            keyData;
      int            optData;
      /// IOV buffer type: Must be a bitmap!
      unsigned int type;

      /// Initializing constructor
      explicit IOV(const IOVType* typ);
      /// Specialized copy constructor
      explicit IOV(const IOVType* typ, const Key& key);
      /// Copy constructor
      IOV(const IOV& copy);

      /// Standard Destructor
      ~IOV();
      /// Move the data content: 'from' will be reset to NULL
      void move(IOV& from);
      /// Create string representation of the IOV
      std::string str() const;
      /// Check if the IOV corresponds to a range
      bool has_range() const { return keyData.first != keyData.second; }
      /// Check if the IOV corresponds to a range
      bool is_discrete() const { return keyData.first == keyData.second; }
      /// Get the local key of the IOV
      Key key() const { return keyData; }

      /// Set discrete IOV value
      void set(const Key& value);
      /// Set discrete IOV value
      void set(Key_first_type value);
      /// Set range IOV value
      void set(Key_first_type val_1, Key_second_type val_2);
      /// Set keys to unphysical values (LONG_MAX, LONG_MIN)
      IOV& reset();
      /// Invert the key values (first=second and second=first)
      IOV& invert();
      /// Set the intersection of this IOV with the argument IOV
      void iov_intersection(const IOV& comparator);
      /// Set the intersection of this IOV with the argument IOV
      void iov_intersection(const IOV::Key& comparator);
      /// Set the union of this IOV with the argument IOV
      void iov_union(const IOV& comparator);
      /// Set the union of this IOV with the argument IOV
      void iov_union(const IOV::Key& comparator);

      /// Check for validity containment
      /** Check if the caller 'iov' is of the same type and the range 
       *  is fully conained by the caller.
       */
      bool contains(const IOV& iov) const;
      /// Check if 2 IOV objects are of the same type
      static bool same_type(const IOV& iov, const IOV& test) {
        unsigned int typ1 = iov.iovType ? iov.iovType->type : iov.type;
        unsigned int typ2 = test.iovType ? test.iovType->type : test.type;
        return typ1 == typ2;
      }
      /// Check if IOV 'test' is fully contained in IOV 'key'
      static bool key_is_contained(const Key& key, const Key& test) {
        return key.first >= test.first && key.second <= test.second;
      }
      /// Same as above, but reverse logic. Gives sometimes more understandable logic.
      static bool key_contains_range(const Key& key, const Key& test) {
        return key.first <= test.first && key.second >= test.second;
      }
      /// Check if IOV 'test' has an overlap on the lower interval edge with IOV 'key'
      static bool key_overlaps_lower_end(const Key& key, const Key& test) {
        return key.first <= test.second && key.first >= test.first;
      }
      /// Check if IOV 'test' has an overlap on the upper interval edge with IOV 'key'
      static bool key_overlaps_higher_end(const Key& key, const Key& test) {
        return key.second >= test.first && key.second <= test.second;
      }
      /// Check if IOV 'test' has an overlap on the upper interval edge with IOV 'key'
      static bool key_partially_contained(const Key& key, const Key& test) {
        return (test.first <= key.first && key.second >= test.second) ||  // test fully contained in key
               (test.first <= key.first && key.first <= test.second) ||   // test overlaps left edge of key
               (test.first <= key.second && key.second <= test.second);   // test overlaps right edge of key
      }
      /// Check if IOV 'test' is of same type and is fully contained in iov
      static bool full_match(const IOV& iov, const IOV& test) {
        return same_type(iov, test) && key_is_contained(iov.keyData, test.keyData);
      }
      /// Check if IOV 'test' is of same type and is at least partially contained in iov
      static bool partial_match(const IOV& iov, const IOV& test) {
        return same_type(iov, test) && key_partially_contained(iov.keyData, test.keyData);
      }
    };

    /// Class describing an opaque conditions data block
    /**
     *  Access methods are templated. Once the access is fixed
     *  on the first call, the data type may not be changed anymore.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class Block {
      /// Access only through the conditions class!
      friend class Condition;

    private:
    protected:
      /// Standard initializing constructor
      Block();
      /// Standard Destructor
      virtual ~Block();

    public:
      /// Data type
      const BasicGrammar* grammar;

    protected:
      /// Pointer to object data
      void* pointer;

    public:
      /// Create data block from string representation
      bool fromString(const std::string& rep);
      /// Create string representation of the data block
      std::string str() const;
      /// Access type id of the condition
      const std::type_info& typeInfo() const;
      /// Access type name of the condition data block
      const std::string& dataType() const;
      /// Check if object is already bound....
      bool is_bound() const { return 0 != pointer; }
      /// Generic getter. Specify the exact type, not a polymorph type
      template <typename T> inline T& get();
      /// Generic getter (const version). Specify the exact type, not a polymorph type
      template <typename T> inline const T& get() const;
    };

    /// Kaykey definition to optimize the access to conditions entities
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionKey {
    public:
      typedef unsigned int key_type;
      /// String representation of the key object
      std::string name;
      /// Hashed key representation
      key_type hash;

    public:
      /// Default constructor
      ConditionKey() : hash(0) {}
      /// Constructor from string
      ConditionKey(const std::string& compare);
      /// Constructor from string
      ConditionKey(const std::string& s, key_type h) : name(s), hash(h) {}
      /// Copy constructor
      ConditionKey(const ConditionKey& c) : name(c.name), hash(c.hash) {}

      /// Hash code generation from input string
      static key_type hashCode(const char* value);
      /// Hash code generation from input string
      static key_type hashCode(const std::string& value);

      /// Assignment operator from the string representation
      ConditionKey& operator=(const std::string& value);
      /// Assignment operator from object copy
      ConditionKey& operator=(const ConditionKey& key);
      /// Equality operator using key object
      bool operator==(const ConditionKey& compare) const;
      /// Equality operator using hash value
      bool operator==(const key_type compare) const;
      /// Equality operator using the string representation
      bool operator==(const std::string& compare) const;

      /// Operator less (for map insertions) using key object
      bool operator<(const ConditionKey& compare) const;
      /// Operator less (for map insertions) using hash value
      bool operator<(const key_type compare) const;
      /// Operator less (for map insertions) using the string representation
      bool operator<(const std::string& compare) const;

      /// Automatic conversion to the string representation of the key object
      operator const std::string&() const { return name; }
      /// Automatic conversion to the hashed representation of the key object
      operator key_type() const { return hash; }
    };

    /// Hash code generation from input string
    inline ConditionKey::key_type ConditionKey::hashCode(const char* value) { return hash32(value); }

    /// Hash code generation from input string
    inline ConditionKey::key_type ConditionKey::hashCode(const std::string& value) { return hash32(value); }

    /// Assignment operator from object copy
    inline ConditionKey& ConditionKey::operator=(const ConditionKey& key) {
      if (this != &key) {
        hash = key.hash;
        name = key.name;
      }
      return *this;
    }

    /// Equality operator using key object
    inline bool ConditionKey::operator==(const ConditionKey& compare) const { return hash == compare.hash; }

    /// Equality operator using hash value
    inline bool ConditionKey::operator==(const key_type compare) const { return hash == compare; }

    /// Operator less (for map insertions) using key object
    inline bool ConditionKey::operator<(const ConditionKey& compare) const { return hash < compare.hash; }

    /// Operator less (for map insertions) using hash value
    inline bool ConditionKey::operator<(const key_type compare) const { return hash < compare; }

    /// Main condition object handle.
    /**
     *  This objects allows access to the data block and
     *  the interval of validity for a single condition.
     *
     *  Note:
     *  Conditions may be shared between several DetElement objects.
     *  Hence, the back-link to the DetElemetn structure cannot be
     *  set - it would be ambiguous.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class Condition : public Handle<Interna::ConditionObject> {
    public:
      typedef Interna::ConditionObject Object;
      typedef ConditionKey::key_type   key_type;
      typedef IOV                      iov_type;

    public:
      enum StringFlags {
        WITH_IOV      = 1 << 0,
        WITH_ADDRESS  = 1 << 1,
        WITH_TYPE     = 1 << 2,
        WITH_COMMENT  = 1 << 4,
        WITH_DATATYPE = 1 << 5,
        WITH_DATA     = 1 << 6,
        NO_NAME       = 1 << 20,
        NONE
      };
      enum ConditionState { INACTIVE = 0, ACTIVE = 1 << 0, CHECKED = 1 << 2, DERIVED = 1 << 3 };

      /// Abstract base for processing callbacks
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Processor {
      public:
        virtual int operator()(Condition c) = 0;
      };

      /// Default constructor
      Condition();
      /// Copy constructor
      Condition(const Condition& c);
      /// Initializing constructor
      Condition(Object* p);
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Condition(const Handle<Q>& e) : Handle<Object>(e) {}
      /// Initializing constructor
      Condition(const std::string& name, const std::string& type);
      /// Assignment operator
      Condition& operator=(const Condition& c);

      /// Output method
      std::string str(int with_data = WITH_IOV | WITH_ADDRESS | WITH_DATATYPE) const;

      /** Data block (bound type)         */
      /// Access the data type
      int dataType() const;
      /// Access the IOV block
      Block& block() const;

      /** Interval of validity            */
      /// Access the IOV type
      const IOVType& iovType() const;
      /// Access the IOV block
      const iov_type& iov() const;

      /** Direct data items in string form */
      /// Access the name of the condition
      const std::string& name() const;
      /// Access the type field of the condition
      const std::string& type() const;
      /// Access the comment field of the condition
      const std::string& comment() const;
      /// Access the value field of the condition as a string
      const std::string& value() const;
      /// Access the address string [e.g. database identifier]
      const std::string& address() const;
      /// Access the key of the condition
      ConditionKey key() const;

      /** Conditions meta-data   */
      /// Access to the type information
      const std::type_info& typeInfo() const;
      /// Access to the grammar type
      const BasicGrammar& descriptor() const;

      /** Conditions handling */
      /// Re-evaluate the conditions data according to the previous bound type definition
      Condition& rebind();

      /** Bind the data of the conditions object to a given format.
       *
       *  Note: The type definition is possible exactly once.
       *  Any further rebindings MUST match the identical type.
       */
      template <typename T> T& bind();
      /// Generic getter. Specify the exact type, not a polymorph type
      template <typename T> T& get();
      /// Generic getter (const version). Specify the exact type, not a polymorph type
      template <typename T> const T& get() const;
    };

    /// Initializing constructor
    inline Condition::Condition(Condition::Object* p) : Handle<Condition::Object>(p) {}

    /// Copy constructor
    inline Condition::Condition(const Condition& c) : Handle<Condition::Object>(c) {}

    /// Default constructor
    inline Condition::Condition() : Handle<Condition::Object>() {}

    /// Container class for condition handles aggregated by a detector element
    /**
     *  Note: The conditions container is owner by the detector element
     *        On deletion the detector element will destroy the container
     *        and all associated entries.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class Container : public Handle<Interna::ConditionContainer> {
    public:
      /// Standard object type
      typedef Interna::ConditionContainer Object;
      typedef Condition::key_type         key_type;
      typedef Condition::iov_type         iov_type;

    public:
      /// Default constructor
      Container();

      /// Constructor to be used when reading the already parsed object
      template <typename Q> Container(const Container& c) : Handle<Object>(c) {}

      /// Constructor to be used when reading the already parsed object
      template <typename Q> Container(const Handle<Q>& e) : Handle<Object>(e) {}

      /// Access the number of conditons keys available for this detector element
      size_t numKeys() const;

      /// Access to condition objects by key and IOV.
      Condition get(const std::string& condition_key, const iov_type& iov);

      /// Access to condition objects directly by their hash key.
      Condition get(key_type condition_key, const iov_type& iov);

      /// Access to condition objects. Only conditions in the pool are accessed.
      Condition get(const std::string& condition_key, const UserPool& iov);

      /// Access to condition objects. Only conditions in the pool are accessed.
      Condition get(key_type condition_key, const UserPool& iov);
    };

    /// Default constructor
    inline Container::Container() : Handle<Object>() {}

    // Utility type definitions
    typedef std::vector<Condition> RangeConditions;
    typedef std::pair<RangeConditions, bool> RangeStatus;

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */
#endif /* DD4HEP_GEOMETRY_CONDITION_H    */
