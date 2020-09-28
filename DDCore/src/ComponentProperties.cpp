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
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "Parsers/Parsers.h"
#include "DD4hep/ComponentProperties.h"

// C/C++ include files
#include <stdexcept>
#include <cstring>

using namespace std;
using namespace dd4hep;

/// Default destructor
PropertyConfigurator::~PropertyConfigurator()   = default;

/// Default constructor
PropertyGrammar::PropertyGrammar(const BasicGrammar& g) : m_grammar(g) {
}

/// Default destructor
PropertyGrammar::~PropertyGrammar() = default;

/// Error callback on invalid conversion
void PropertyGrammar::invalidConversion(const std::type_info& from,
                                        const std::type_info& to)
{
  BasicGrammar::invalidConversion(from,to);
}

/// Error callback on invalid conversion
void PropertyGrammar::invalidConversion(const std::string& value,
                                        const std::type_info& to)
{
  BasicGrammar::invalidConversion(value,to);
}

/// Access to the type information
auto PropertyGrammar::type() const -> const std::type_info&  {
  return m_grammar.type();
}

/// Serialize an opaque value to a string
auto PropertyGrammar::str(const void* ptr) const -> std::string  {
  return m_grammar.str(ptr);
}

/// Set value from serialized string. On successful data conversion TRUE is returned.
auto PropertyGrammar::fromString(void* ptr, const std::string& value) const -> bool  {
  return m_grammar.fromString(ptr,value);
}

/// Property type name
auto Property::type(const Property& property) -> string {
  return type(property.grammar().type());
}

/// Property type name
auto Property::type(const type_info& typ) -> string {
  return typeName(typ);
}

/// Property type name
auto Property::type() const -> string {
  return Property::type(grammar().type());
}

auto Property::grammar() const -> const PropertyGrammar& {
  if ( m_hdl )
    return *m_hdl;
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion to string value
auto Property::str() const -> string {
  if ( m_hdl && m_par ) {
    return m_hdl->str(m_par);
  }
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion from string value
auto Property::str(const std::string& input)   const -> const Property& {
  if ( m_hdl && m_par )   {
    m_hdl->fromString(m_par,input);
    return *this;
  }
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion from string value
auto Property::str(const std::string& input) -> Property&    {
  if ( m_hdl && m_par )   {
    m_hdl->fromString(m_par,input);
    return *this;
  }
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Assignment operator / set new balue
auto Property::operator=(const char* val) -> Property& {
  if ( val ) {
    this->set < string > (val);
    return *this;
  }
  throw runtime_error("Attempt to set invalid string to property!");
}

/// Default constructor
PropertyManager::PropertyManager() = default;

/// Default destructor
PropertyManager::~PropertyManager() {
  m_properties.clear();
}

/// Access total number of properties
auto PropertyManager::size()  const -> size_t   {
  return m_properties.size();
}

/// Export properties of another instance
void PropertyManager::adopt(const PropertyManager& copy)   {
  m_properties = copy.m_properties;
}

/// Check for existence
auto PropertyManager::exists(const std::string& name) const -> bool   {
  auto i = m_properties.find(name);
  return i != m_properties.end();
}

/// Verify that this property does not exist (throw exception if the name was found)
void PropertyManager::verifyNonExistence(const string& name) const {
  auto i = m_properties.find(name);
  if (i == m_properties.end())
    return;
  throw runtime_error("The property:" + name + " already exists for this component.");
}

/// Verify that this property exists (throw exception if the name was not found)
auto
PropertyManager::verifyExistence(const string& name) const -> PropertyManager::Properties::const_iterator {
  auto i = m_properties.find(name);
  if (i != m_properties.end())
    return i;
  throw runtime_error("PropertyManager: Unknown property:" + name);
}

/// Verify that this property exists (throw exception if the name was not found)
auto
PropertyManager::verifyExistence(const string& name) -> PropertyManager::Properties::iterator {
  auto i = m_properties.find(name);
  if (i != m_properties.end())
    return i;
  throw runtime_error("PropertyManager: Unknown property:" + name);
}

/// Access property by name (CONST)
auto PropertyManager::property(const string& name) -> Property& {
  return (*verifyExistence(name)).second;
}

/// Access property by name
auto PropertyManager::property(const string& name) const -> const Property& {
  return (*verifyExistence(name)).second;
}

/// Access property by name
auto PropertyManager::operator[](const string& name) -> Property& {
  return (*verifyExistence(name)).second;
}

/// Access property by name
auto PropertyManager::operator[](const string& name) const -> const Property& {
  return (*verifyExistence(name)).second;
}

/// Add a new property
void PropertyManager::add(const string& name, const Property& prop) {
  verifyNonExistence(name);
  m_properties.emplace(name, prop);
}

/// Bulk set of all properties
void PropertyManager::set(const string& component_name, PropertyConfigurator& cfg) {
  for (auto& i : m_properties )
    cfg.set(i.second.grammar(), component_name, i.first, i.second.ptr());
}

/// Dump string values
void PropertyManager::dump() const {
  for (const auto& i : m_properties )
    printout(ALWAYS, "PropertyManager", "Property %s = %s",
             i.first.c_str(), i.second.str().c_str());
}

/// Standard PropertyConfigurable constructor
PropertyConfigurable::PropertyConfigurable()  = default;

/// Default PropertyConfigurable destructor
PropertyConfigurable::~PropertyConfigurable()   = default;

/// Check property for existence
auto PropertyConfigurable::hasProperty(const string& nam) const -> bool    {
  return m_properties.exists(nam);
}

/// Access single property
auto PropertyConfigurable::property(const string& nam) -> Property&   {
  return properties()[nam];
}

#include "DD4hep/detail/Grammar_parsed.h"
namespace dd4hep { 
  namespace Parsers {
    template <> auto parse(Property& result, const std::string& input) -> int {
      result.str(input); 
      return 1;
    }
    template <> auto toStream(const Property& result, std::ostream& os) -> std::ostream& {
      return os << result.str();
    }
  }
#if 0
  template<> int Grammar<Property>::evaluate(void* _p, const std::string& _v) const {
    return eval_obj ((Property*)_p,_v);
  }
#endif
  // Ensure the grammars are registered
  template class Grammar<Property>;
  static auto s_registry = GrammarRegistry::pre_note<Property>(1);
}

