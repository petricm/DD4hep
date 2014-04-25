// $Id: Readout.h 951 2013-12-16 23:37:56Z Christian.Grefe@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_CONDITION_H
#define DD4HEP_GEOMETRY_CONDITION_H

// Framework include files
#include "DD4hep/Handle.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /// Grammar definition for type binding
  class BasicGrammar;

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    // Forward declarations
    class DetElement;

    namespace ConditionsInterna {
      class Container;
      class Object;
      class Entry;
      class IOV;
    }

    /** @class Block  ConditionsInterna.h DD4hep/ConditionsInterna.h
     * 
     *  Class describing an opaque conditions data block
     *
     * @author  M.Frank
     * @version 1.0
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

    protected:
      /// Data type
      const BasicGrammar* grammar;
      /// Pointer to object data
      void* pointer;

    public:
      /// Create data block from string representation
      void fromString(const std::string& rep);
      /// Create string representation of the data block
      std::string str();
      /// Generic getter. Specify the exact type, not a polymorph type
      template <typename T> inline T& get();
      /// Generic getter (const version). Specify the exact type, not a polymorph type
      template <typename T> inline const T& get() const;
    };

    /** @class Condition  Condition.h DD4hep/Condition.h
     * 
     *  See the documentation about the TGeoPhysicalNode for further
     *  details.
     *
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Condition : public Handle<ConditionsInterna::Object> {
    public:
      typedef ConditionsInterna::Object Object;
      typedef ConditionsInterna::Entry  Entry;
      typedef ConditionsInterna::IOV    IOV;

      /// Default constructor
      Condition();
      /// Copy constructor
      Condition(const Condition& c);
      /// Initializing constructor
      Condition(Object* p);
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Condition(const Handle<Q>& e) : Handle<Object>(e) {}
      /// Initializing constructor
      Condition(const std::string& name);
      /// Assignment operator
      Condition& operator=(const Condition& c);

      /** Data block (bound type)         */
      /// Access the data type
      int dataType() const;
      /// Access the IOV block
      Block& block() const;

      /** Interval of validity            */
      /// Access the IOV type
      int iovType() const;
      /// Access the IOV block
      IOV& iov() const;

      /** Direct data items in string form */
      /// Access the name of the condition
      const std::string& name() const;
      /// Access the type field of the condition
      const std::string& type() const;
      /// Access the comment field of the condition
      const std::string& comment() const;
      /// Access the validity field of the condition as a string
      const std::string& validity() const;
      /// Access the value field of the condition as a string
      const std::string& value() const;
      /// Access the address string [e.g. database identifier]
      const std::string& address() const;
      /// Access the hosting detector element
      DetElement detector() const;

      /** Conditions meta-data   */
      /// Access to the type information
      const std::type_info& typeInfo() const;
      /// Access to the grammar type
      const BasicGrammar& descriptor() const;

      /** Conditions handling */
      /// Replace the data block of the condition with a new value. Free old data
      Condition& replace(Entry* new_condition);
      /// Re-evaluate the conditions data according to the previous bound type definition
      Condition& rebind();

      /** Bind the data of the conditions object to a given format. 
       *
       *  Note: The type definition is possible exactly once.
       *  Any further rebindings MUST match the identical type.
       */
      template <typename T> Condition& bind();
      /// Generic getter. Specify the exact type, not a polymorph type
      template <typename T> T& get();
      /// Generic getter (const version). Specify the exact type, not a polymorph type
      template <typename T> const T& get() const;
    };

    /// Initializing constructor
    inline Condition::Condition(Object* p) : Handle<Object>(p) {}

    /// Copy constructor
    inline Condition::Condition(const Condition& c) : Handle<Object>(c) {}

    /// Default constructor
    inline Condition::Condition() : Handle<Object>() {}

    /** @class Conditions  Conditions.h DD4hep/Conditions.h
     *
     *  Note: The conditions container is owner by the detector element
     *        On deletion the detector element will destroy the container
     *        and all associated entries.
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Conditions : public Handle<ConditionsInterna::Container> {
      /// Standard object type
      typedef ConditionsInterna::Container Object;
      /// Local helper definition
      typedef ConditionsInterna::Entry Entry;

      /// Definition of the conditions container of this detector element
      typedef std::map<std::string, Condition> Entries;

      /// Default constructor
      Conditions() : Handle<Object>() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Conditions(const Conditions& c) : Handle<Object>(c) {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Conditions(const Handle<Q>& e) : Handle<Object>(e) {}
      /// Access the number of conditons available for this detector element
      size_t count() const;
      /// Access the full map of conditons
      Entries& entries() const;
      /// Clear all conditions. Auto-delete of all existing entries
      void removeElements() const;
      /** Set a single conditions value (eventually existing entries are overwritten)
       * Note: Passing a valid handle also passes ownership!!!
       *
       * Failure return 0 in case an invalid handle is present
       * Successful insertion returns 1
       * Successful overwriting an existing value returns 3
       */
      int set(Entry* data);
    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */
#endif /* DD4HEP_GEOMETRY_CONDITION_H    */
