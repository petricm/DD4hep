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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/detail/ConditionsInterna.h"

// C/C++ include files
#include <climits>
#include <iomanip>
#include <cstdio>

using namespace std;
using namespace dd4hep;

/// Initializing constructor for a pure, undecorated conditions object
Condition::Condition(key_type hash_key) : Handle<Object>()
{
  if ( hash_key != 0 && hash_key != ~0x0ULL )  {
    auto* o = new Object();
    assign(o,"","");
    o->hash = hash_key;
    return;
  }
  except("Condition","+++ Cannot create a condition with an invalid key: %016llX",hash_key);
}

/// Initializing constructor for a pure, undecorated conditions object
Condition::Condition(const std::string& nam, const std::string& typ) : Handle<Object>()
{
  auto* o = new Object();
  assign(o,nam,typ);
  o->hash = 0;
}

/// Initializing constructor for a pure, undecorated conditions object with payload buffer
Condition::Condition(const string& nam,const string& typ, size_t memory)
  : Handle<Object>()
{
  void* ptr = ::operator new(sizeof(Object)+memory);
  auto* o = new(ptr) Object();
  assign(o,nam,typ);
  o->hash = 0;
}

/// Output method
auto Condition::str(int flags)  const -> string   {
  stringstream output;
  Object* o = access(); 
#if defined(DD4HEP_CONDITIONS_HAVE_NAME)
  if ( 0 == (flags&NO_NAME) )
    output << setw(16) << left << o->name;
#endif
  if ( flags&WITH_IOV )  {
    const IOV* ptr_iov = o->iovData();
    output << " " << (ptr_iov ? ptr_iov->str().c_str() : "IOV:[UNKNOWN]");
  }
#if !defined(DD4HEP_MINIMAL_CONDITIONS)
  if ( flags&WITH_TYPE )
    output << " (" << o->type << ")";
#endif
  if ( flags&WITH_DATATYPE )
    output << " -> " << o->data.dataType();
  if ( flags&WITH_DATA )
    output << " Data:" << o->data.str();
#if !defined(DD4HEP_MINIMAL_CONDITIONS)
  if ( flags&WITH_ADDRESS )
    output << " " << o->address;
  if ( flags&WITH_COMMENT )
    output << " \"" << o->comment << "\"";
#endif
  return output.str();
}

/// Access the data type
auto Condition::dataType() const -> int   {
  return access()->data.type;
}

/// Access the IOV block
auto Condition::data() const -> dd4hep::OpaqueDataBlock&   {
  return access()->data;
}

/// Access the IOV type
auto Condition::iovType() const -> const dd4hep::IOVType&   {
  return *(access()->iovType());
}

/// Access the IOV block
auto Condition::iov() const -> const dd4hep::IOV&   {
  return *(access()->iovData());
}

#if !defined(DD4HEP_MINIMAL_CONDITIONS)
/// Access the type field of the condition
const string& Condition::type()  const   {
  return access()->type;
}

/// Access the value field of the condition as a string
const string& Condition::value()  const   {
  return access()->value;
}

/// Access the comment field of the condition
const string& Condition::comment()  const   {
  return access()->comment;
}

/// Access the address string [e.g. database identifier]
const string& Condition::address()  const   {
  return access()->address;
}
#endif

/// Access to the type information
auto Condition::typeInfo() const -> const type_info&   {
  return descriptor().type();
}

/// Hash identifier
auto Condition::key()  const -> Condition::key_type    {
  return access()->hash;
}

/// DetElement part of the identifier
auto Condition::detector_key()  const -> Condition::detkey_type   {
  return ConditionKey::KeyMaker(access()->hash).values.det_key;
}

/// Item part of the identifier
auto Condition::item_key()  const -> Condition::itemkey_type   {
  return ConditionKey::KeyMaker(access()->hash).values.item_key;
}

/// Flag operations: Get flags
auto Condition::flags()  const -> Condition::mask_type    {
  return access()->flags;
}

/// Flag operations: Set a conditons flag
void Condition::setFlag(mask_type option)   {
  access()->setFlag(option);
}

/// Flag operations: UN-Set a conditons flag
void Condition::unFlag(mask_type option)   {
  access()->unFlag(option);
}

/// Flag operations: Test for a given a conditons flag
auto Condition::testFlag(mask_type option) const -> bool {
  return access()->testFlag(option);
}

/// Access to the grammar type
auto Condition::descriptor() const -> const dd4hep::BasicGrammar&   {
  const BasicGrammar* grammar = access()->data.grammar;
  if ( !grammar ) {
    invalidHandleError<Condition>();
    // This code is never reached, since function above throws exception!
    // Needed to satisfay CppCheck
    throw runtime_error("Null pointer in Grammar object");
  }
  return *grammar;
}

/// Default destructor. 
ConditionsSelect::~ConditionsSelect()   = default;

/// Constructor from string
ConditionKey::KeyMaker::KeyMaker(DetElement detector, const std::string& value)   {
  hash = KeyMaker(detector.key(), detail::hash32(value)).hash;
}

/// Constructor from detector element and item sub-key
ConditionKey::KeyMaker::KeyMaker(DetElement detector, Condition::itemkey_type item_key)  {
  hash = KeyMaker(detector.key(), item_key).hash;
}

/// Constructor from string
ConditionKey::KeyMaker::KeyMaker(Condition::detkey_type det, const std::string& value)   {
  hash = KeyMaker(det, detail::hash32(value)).hash;
}

/// Constructor from string
ConditionKey::ConditionKey(DetElement detector, const string& value)  {
  hash = KeyMaker(detector,value).hash;
#ifdef DD4HEP_CONDITIONS_HAVE_NAME
  name = detector.path()+"#"+value;
#endif
}

/// Constructor from detector element key and item sub-key
ConditionKey::ConditionKey(Condition::detkey_type det_key, const string& value)    {
  hash = KeyMaker(det_key,value).hash;
#ifdef DD4HEP_CONDITIONS_HAVE_NAME
  char text[32];
  ::snprintf(text,sizeof(text),"%08X#",det_key);
  name = text+value;
#endif
}

/// Constructor from detector element key and item sub-key
ConditionKey::ConditionKey(DetElement detector, Condition::itemkey_type item_key)  {
  hash = KeyMaker(detector.key(),item_key).hash;
#ifdef DD4HEP_CONDITIONS_HAVE_NAME
  char text[32];
  ::snprintf(text,sizeof(text),"#%08X",item_key);
  name = detector.path()+text;
#endif
}

/// Hash code generation from input string
auto ConditionKey::hashCode(DetElement detector, const char* value) -> Condition::key_type  {
  return KeyMaker(detector.key(), value).hash;
}

/// Hash code generation from input string
auto ConditionKey::hashCode(DetElement detector, const string& value) -> Condition::key_type  {
  return KeyMaker(detector, value).hash;
}

/// 32 bit hashcode of the item
auto ConditionKey::itemCode(const char* value) -> Condition::itemkey_type  {
  return detail::hash32(value);
}

/// 32 bit hashcode of the item
auto ConditionKey::itemCode(const std::string& value) -> Condition::itemkey_type   {
  return detail::hash32(value);
}

/// Conversion to string
auto ConditionKey::toString()  const -> string    {
  dd4hep::ConditionKey::KeyMaker key(hash);
  char text[64];
  ::snprintf(text,sizeof(text),"%08X-%08X",key.values.det_key, key.values.item_key);
#if defined(DD4HEP_CONDITIONS_HAVE_NAME)
  if ( !name.empty() )   {
    stringstream str;
    str << "(" << name << ") " << text;
    return str.str();
  }
#endif
  return text;
}

