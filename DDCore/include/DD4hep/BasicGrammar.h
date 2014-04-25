// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_BASICGRAMMAR_H
#define DD4HEP_DDG4_BASICGRAMMAR_H

// C/C++ include files
#include <string>
#include <typeinfo>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class BasicGrammar
   *
   *   @author  M.Frank
   *   @date    13.08.2013
   */
  class BasicGrammar {
  public:
    /// Default constructor
    BasicGrammar();
    /// Default destructor
    virtual ~BasicGrammar();

    /// Instance factory
    template <typename TYPE> static const BasicGrammar& instance();
    /// Error callback on invalid conversion
    static void invalidConversion(const std::type_info& from, const std::type_info& to);
    /// Error callback on invalid conversion
    static void invalidConversion(const std::string& value, const std::type_info& to);
    /// Access to the type information
    virtual const std::type_info& type() const = 0;
    /// Access the object size (sizeof operator)
    virtual size_t sizeOf() const = 0;
    /// Serialize an opaque value to a string
    virtual std::string str(const void* ptr) const = 0;
    /// Set value from serialized string. On successful data conversion TRUE is returned.
    virtual bool fromString(void* ptr, const std::string& value) const = 0;
  };
}  // End namespace DD4hep

#endif /* DD4HEP_DDG4_BASICGRAMMAR_H */
