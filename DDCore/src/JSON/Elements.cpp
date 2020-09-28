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
#include "JSON/Printout.h"
#include "JSON/Evaluator.h"
#include "JSON/Elements.h"

// C/C++ include files
#include <cstdio>
#include <iostream>
#include <map>
#include <stdexcept>
#include <utility>


using namespace std;
using namespace dd4hep::json;
static const size_t INVALID_NODE = ~0U;

// Forward declarations
namespace dd4hep {
  auto evaluator() -> dd4hep::tools::Evaluator&;
}
// Static storage
namespace {
  dd4hep::tools::Evaluator& eval(dd4hep::evaluator());
  auto _checkEnviron(const string& env) -> string  {
    string r = getEnviron(env);
    return r.empty() ? env : r;
  }
}

namespace {

  // This should ensure we are not passing temporaries of std::string and then
  // returning the "const char*" content calling .c_str()
  auto value_data(const ptree& entry) -> const ptree::data_type&  {
    return entry.data();
  }

  auto node_first(JsonElement* e, const char* tag) -> JsonElement* {
    if ( e )  {
      string t(tag);
      if ( t == "*" )  {
        ptree::iterator i = e->second.begin();
        return i != e->second.end() ? &(*i) : nullptr;
      }
      ptree::assoc_iterator i = e->second.find(t);
      return i != e->second.not_found() ? &(*i) : nullptr;
    }
    return nullptr;
  }

  auto node_count(JsonElement* e, const string& t) -> size_t {
    return e ? (t=="*" ? e->second.size() : e->second.count(t)) : 0;
  }

  auto attribute_node(JsonElement* n, const char* t) -> Attribute  {
    if ( n )  {
      auto i = n->second.find(t);
      return i != n->second.not_found() ? &(*i) : nullptr;
    }
    return nullptr;
  }

  auto attribute_value(Attribute a) -> const char* {
    return value_data(a->second).c_str();
  }
}

auto dd4hep::json::_toString(Attribute attr) -> string {
  if (attr)
    return _toString(attribute_value(attr));
  return "";
}

template <typename T> static inline auto __to_string(T value, const char* fmt) -> string {
  char text[128];
  ::snprintf(text, sizeof(text), fmt, value);
  return text;
}

/// Do-nothing version. Present for completeness and argument interchangeability
auto dd4hep::json::_toString(const char* s) -> string {
  if ( !s || *s == 0 ) return "";
  else if ( !(*s == '$' && *(s+1) == '{') ) return s;
  return _checkEnviron(s);
}

/// Do-nothing version. Present for completeness and argument interchangeability
auto dd4hep::json::_toString(const string& s) -> string {
  if ( s.length() < 3 || s[0] != '$' ) return s;
  else if ( !(s[0] == '$' && s[1] == '{') ) return s;
  return _checkEnviron(s);
}

/// Format unsigned long integer to string with arbitrary format
auto dd4hep::json::_toString(unsigned long v, const char* fmt) -> string {
  return __to_string(v, fmt);
}

/// Format unsigned integer (32 bits) to string with arbitrary format
auto dd4hep::json::_toString(unsigned int v, const char* fmt) -> string {
  return __to_string(v, fmt);
}

/// Format signed integer (32 bits) to string with arbitrary format
auto dd4hep::json::_toString(int v, const char* fmt) -> string {
  return __to_string(v, fmt);
}

/// Format signed long integer to string with arbitrary format
auto dd4hep::json::_toString(long v, const char* fmt) -> string   {
  return __to_string(v, fmt);
}

/// Format single procision float number (32 bits) to string with arbitrary format
auto dd4hep::json::_toString(float v, const char* fmt) -> string {
  return __to_string(v, fmt);
}

/// Format double procision float number (64 bits) to string with arbitrary format
auto dd4hep::json::_toString(double v, const char* fmt) -> string {
  return __to_string(v, fmt);
}

/// Format pointer to string with arbitrary format
auto dd4hep::json::_ptrToString(const void* v, const char* fmt) -> string {
  return __to_string(v, fmt);
}

auto dd4hep::json::_toLong(const char* value) -> long {
  if (value) {
    string s = _toString(value);
    size_t idx = s.find("(int)");
    if (idx != string::npos)
      s.erase(idx, 5);
    idx = s.find("(long)");
    if (idx != string::npos)
      s.erase(idx, 6);
    while (s[0] == ' ')
      s.erase(0, 1);
    double result = eval.evaluate(s.c_str());
    if (eval.status() != tools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
    }
    return (long) result;
  }
  return -1;
}

auto dd4hep::json::_toInt(const char* value) -> int {
  if (value) {
    string s = _toString(value);
    size_t idx = s.find("(int)");
    if (idx != string::npos)
      s.erase(idx, 5);
    while (s[0] == ' ')
      s.erase(0, 1);
    double result = eval.evaluate(s.c_str());
    if (eval.status() != tools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
    }
    return (int) result;
  }
  return -1;
}

auto dd4hep::json::_toBool(const char* value) -> bool {
  if (value) {
    string s = _toString(value);
    return s == "true";
  }
  return false;
}

auto dd4hep::json::_toFloat(const char* value) -> float {
  if (value) {
    string s = _toString(value);
    double result = eval.evaluate(s.c_str());

    if (eval.status() != tools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
    }
    return (float) result;
  }
  return 0.0;
}

auto dd4hep::json::_toDouble(const char* value) -> double {
  if (value) {
    string s = _toString(value);
    double result = eval.evaluate(s.c_str());
    if (eval.status() != tools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
    }
    return result;
  }
  return 0.0;
}

void dd4hep::json::_toDictionary(const char* name, const char* value) {
  string n = _toString(name).c_str(), v = _toString(value);
  size_t idx = v.find("(int)");
  if (idx != string::npos)
    v.erase(idx, 5);
  while (v[0] == ' ')
    v.erase(0, 1);
  double result = eval.evaluate(v.c_str());
  if (eval.status() != tools::Evaluator::OK) {
    cerr << v << ": ";
    eval.print_error();
    throw runtime_error("dd4hep: Severe error during expression evaluation of " + v);
  }
  eval.setVariable(n.c_str(), result);
}

template <typename T>
void dd4hep::json::_toDictionary(const char* name, T value)   {
  string item = _toString(value);
  _toDictionary(name, item.c_str());
}

template void dd4hep::json::_toDictionary(const char* name, const string& value);
template void dd4hep::json::_toDictionary(const char* name, unsigned long value);
template void dd4hep::json::_toDictionary(const char* name, unsigned int value);
template void dd4hep::json::_toDictionary(const char* name, unsigned short value);
template void dd4hep::json::_toDictionary(const char* name, int value);
template void dd4hep::json::_toDictionary(const char* name, long value);
template void dd4hep::json::_toDictionary(const char* name, short value);
template void dd4hep::json::_toDictionary(const char* name, float value);
template void dd4hep::json::_toDictionary(const char* name, double value);

/// Evaluate string constant using environment stored in the evaluator
auto dd4hep::json::getEnviron(const string& env) -> string   {
  size_t id1 = env.find("${");
  size_t id2 = env.rfind("}");
  if ( id1 == string::npos || id2 == string::npos )   {
    return "";
  }
  else  {
    string v = env.substr(0,id2+1);
    const char* ret = eval.getEnviron(v.c_str());
    if (eval.status() != tools::Evaluator::OK) {
      cerr << env << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during environment lookup of " + env);
    }
    v = env.substr(0,id1);
    v += ret;
    v += env.substr(id2+1);
    return v;
  }
}

/// Copy constructor
NodeList::NodeList(const NodeList& copy)
  : m_tag(copy.m_tag), m_node(copy.m_node)
{
  reset();
}

/// Initializing constructor
NodeList::NodeList(JsonElement* node, string  tag_value)
  : m_tag(std::move(tag_value)), m_node(node)
{
  reset();
}

/// Default destructor
NodeList::~NodeList() = default;

/// Reset the nodelist
auto NodeList::reset() -> JsonElement* {
  if ( m_tag == "*" )
    m_ptr = make_pair(m_node->second.ordered_begin(), m_node->second.not_found());
  else
    m_ptr = m_node->second.equal_range(m_tag);
  if ( m_ptr.first != m_ptr.second )
    return &(*m_ptr.first);
  return nullptr;
}

/// Advance to next element
auto NodeList::next() const -> JsonElement* {
  if ( m_ptr.first != m_ptr.second )  {
    m_ptr.first = ++m_ptr.first;
    if ( m_ptr.first != m_ptr.second ) return &(*m_ptr.first);
  }
  return nullptr;
}

/// Go back to previous element
auto NodeList::previous() const -> JsonElement* {
  if ( m_ptr.first != m_ptr.second )  {
    m_ptr.first = --m_ptr.first;
    if ( m_ptr.first != m_ptr.second ) return &(*m_ptr.first);
  }
  return nullptr;
}

/// Assignment operator
auto NodeList::operator=(const NodeList& l) -> NodeList& {
  if ( this != &l ) {
    m_tag  = l.m_tag;
    m_node = l.m_node;
    reset();
  }
  return *this;
}

/// Unicode text access to the element's tag. This must be wrong ....
auto Handle_t::rawTag() const -> const char* {
  return m_node->first.c_str();
}

/// Unicode text access to the element's text
auto Handle_t::rawText() const -> const char* {
  return value_data(m_node->second).c_str();
}

/// Unicode text access to the element's value
auto Handle_t::rawValue() const -> const char* {
  return value_data(m_node->second).c_str();
}

/// Access attribute pointer by the attribute's unicode name (no exception thrown if not present)
auto Handle_t::attr_nothrow(const char* tag_value) const -> Attribute {
  return attribute_node(m_node, tag_value);
}

/// Check for the existence of a named attribute
auto Handle_t::hasAttr(const char* tag_value) const -> bool {
  return m_node && nullptr != node_first(m_node, tag_value);
}

/// Retrieve a collection of all attributes of this DOM element
auto Handle_t::attributes() const -> vector<Attribute> {
  vector < Attribute > attrs;
  if (m_node) {
    for(ptree::iterator i=m_node->second.begin(); i!=m_node->second.end(); ++i)  {
      Attribute a = &(*i);
      attrs.emplace_back(a);
    }
  }
  return attrs;
}

auto Handle_t::numChildren(const char* t, bool throw_exception) const -> size_t {
  size_t n = node_count(m_node, t);
  if (n == INVALID_NODE && !throw_exception)
    return 0;
  else if (n != INVALID_NODE)
    return n;
  string msg = "Handle_t::numChildren: ";
  if (m_node)
    msg += "Element [" + tag() + "] has no children of type '" + _toString(t) + "'";
  else
    msg += "Element [INVALID] has no children of type '" + _toString(t) + "'";
  throw runtime_error(msg);
}

/// Remove a single child node identified by it's handle from the tree of the element
auto Handle_t::child(const char* t, bool throw_exception) const -> Handle_t {
  Elt_t e = node_first(m_node, t);
  if (e || !throw_exception)
    return e;
  string msg = "Handle_t::child: ";
  if (m_node)
    msg += "Element [" + tag() + "] has no child of type '" + _toString(t) + "'";
  else
    msg += "Element [INVALID]. Cannot remove child of type: '" + _toString(t) + "'";
  throw runtime_error(msg);
}

auto Handle_t::children(const char* tag_value) const -> NodeList {
  return NodeList(m_node, tag_value);
}

auto Handle_t::hasChild(const char* tag_value) const -> bool {
  return node_first(m_node, tag_value) != nullptr;
}

/// Access attribute pointer by the attribute's unicode name (throws exception if not present)
auto Handle_t::attr_ptr(const char* t) const -> Attribute {
  Attribute a = attribute_node(m_node, t);
  if (nullptr != a)
    return a;
  string msg = "Handle_t::attr_ptr: ";
  if (m_node)
    msg += "Element [" + tag() + "] has no attribute of type '" + _toString(t) + "'";
  else
    msg += "Element [INVALID] has no attribute of type '" + _toString(t) + "'";
  throw runtime_error(msg);
}

/// Access attribute name (throws exception if not present)
auto Handle_t::attr_name(const Attribute a) const -> const char* {
  if (a) {
    return a->first.c_str();
  }
  throw runtime_error("Attempt to access invalid XML attribute object!");
}

/// Access attribute value by the attribute's unicode name (throws exception if not present)
auto Handle_t::attr_value(const char* attr_tag) const -> const char* {
  return attribute_value(attr_ptr(attr_tag));
}

/// Access attribute value by the attribute  (throws exception if not present)
auto Handle_t::attr_value(const Attribute attr_val) const -> const char* {
  return attribute_value(attr_val);
}

/// Access attribute value by the attribute's unicode name (no exception thrown if not present)
auto Handle_t::attr_value_nothrow(const char* attr_tag) const -> const char* {
  Attribute a = attr_nothrow(attr_tag);
  return a ? attribute_value(a) : nullptr;
}


/// Access the ROOT eleemnt of the DOM document
auto Document::root() const -> Handle_t   {
  if ( m_doc )   {
    return m_doc;
  }
  throw runtime_error("Document::root: Invalid handle!");
}

/// Assign new document. Old document is dropped.
auto DocumentHolder::assign(DOC d) -> DocumentHolder&   {
  if ( m_doc )   {
    printout(DEBUG,"DocumentHolder","+++ Release JSON document....");
    delete m_doc;
  }
  m_doc = d;
  return *this;
}

/// Standard destructor - releases the document
DocumentHolder::~DocumentHolder()   {
  assign(nullptr);
}

auto Element::getAttr(const char* name) const -> Attribute {
  return m_element ? attribute_node(m_element, name) : nullptr;
}

Collection_t::Collection_t(Handle_t element, const char* tag_value)
  : m_children(element, tag_value) {
  m_node = m_children.reset();
}

/// Constructor over XmlElements in a node list
Collection_t::Collection_t(NodeList node_list)
  : m_children(node_list) {
  m_node = m_children.reset();
}

/// Reset the collection object to restart the iteration
auto Collection_t::reset() -> Collection_t& {
  m_node = m_children.reset();
  return *this;
}

/// Access the collection size. Avoid this call -- sloooow!
auto Collection_t::size() const -> size_t {
  return Handle_t(m_children.m_node).numChildren(m_children.m_tag.c_str(), false);
}

/// Helper function to throw an exception
void Collection_t::throw_loop_exception(const exception& e) const {
  if (m_node) {
    throw runtime_error(string(e.what()) + "\n" + "dd4hep: Error interpreting XML nodes of type <" + tag() + "/>");
  }
  throw runtime_error(string(e.what()) + "\n" + "dd4hep: Error interpreting collections XML nodes.");
}

void Collection_t::operator++() const {
  while (m_node) {
    m_node = m_children.next();
    if (m_node && m_node->second.size() > 0 )
      return;
  }
}

void Collection_t::operator--() const {
  while (m_node) {
    m_node = m_children.previous();
    if (m_node && m_node->second.size() > 0 )
      return;
  }
}

void Collection_t::operator++(int) const {
  ++(*this);
}

void Collection_t::operator--(int) const {
  --(*this);
}

void dd4hep::json::dumpTree(Handle_t elt)   {
  dumpTree(elt.ptr());
}

void dd4hep::json::dumpTree(Element elt)   {
  dumpTree(elt.ptr());
}

void dd4hep::json::dumpTree(const JsonElement* elt)   {
  struct Dump {
    void operator()(const JsonElement* e, const string& tag)   const  {
      string t = tag+"   ";
      printout(INFO,"DumpTree","+++ %s %s: %s",tag.c_str(), e->first.c_str(), e->second.data().c_str());
      for(auto i=e->second.begin(); i!=e->second.end(); ++i)
        (*this)(&(*i), t);
    }
  } _dmp;
  _dmp(elt," ");
}

