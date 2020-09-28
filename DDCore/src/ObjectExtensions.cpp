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
#include "DD4hep/ObjectExtensions.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace dd4hep;

namespace {
  std::string obj_type(void* ptr)  {
    auto* o = (ObjectExtensions*)ptr;
    return typeName(typeid(*o));
  }
}

/// Default constructor
ObjectExtensions::ObjectExtensions(const type_info& /* parent_type */)    {
  InstanceCount::increment(this);
}

/// Default destructor
ObjectExtensions::~ObjectExtensions()   {
  clear();
  InstanceCount::decrement(this);
}

/// Move extensions to target object
void ObjectExtensions::move(ObjectExtensions& source)   {
  extensions = source.extensions;
  source.extensions.clear();
}

/// Internal object destructor: release extension object(s)
void ObjectExtensions::clear(bool destroy) {
  for( const auto& i : extensions )  {
    if ( i.second ) {
      if ( destroy ) i.second->destruct();
      delete i.second;
    }
  }
  extensions.clear();
}

/// Copy object extensions from another object
void ObjectExtensions::copyFrom(const map<unsigned long long int,ExtensionEntry*>& ext, void* arg)  {
  for( const auto& i : ext )  {
    extensions[i.first] = i.second->clone(arg);
  }
}

/// Add an extension object to the detector element
void* ObjectExtensions::addExtension(unsigned long long int key, ExtensionEntry* e)  {
  if ( e )   {
    if ( e->object() )  {
      auto j = extensions.find(key);
      if (j == extensions.end()) {
        extensions[key] = e;
        return e->object();
      }
      except("ObjectExtensions::addExtension","Object already has an extension of type: %s.",obj_type(e->object()).c_str());
    }
    except("ObjectExtensions::addExtension","Invalid extension object for key %016llX!",key);
  }
  except("ObjectExtensions::addExtension","Invalid extension entry for key %016llX!",key);
  return nullptr;
}

/// Remove an existing extension object from the instance
void* ObjectExtensions::removeExtension(unsigned long long int key, bool destroy)  {
  auto j = extensions.find(key);
  if ( j != extensions.end() )   {
    void* ptr = (*j).second->object();
    if ( destroy )  {
      (*j).second->destruct();
    }
    delete (*j).second;
    extensions.erase(j);
    return ptr;
  }
  except("ObjectExtensions::removeExtension","The object of type %016llX is not present.",key);
  return nullptr;
}

/// Access an existing extension object from the detector element
void* ObjectExtensions::extension(unsigned long long int key) const {
  const auto j = extensions.find(key);
  if (j != extensions.end()) {
    return (*j).second->object();
  }
  string msg = format("ObjectExtensions::extension","The object has no extension of type %016llX.",key);
  throw runtime_error(msg);
}

/// Access an existing extension object from the detector element
void* ObjectExtensions::extension(unsigned long long int key, bool alert) const {
  const auto j = extensions.find(key);
  if (j != extensions.end()) {
    return (*j).second->object();
  }
  else if ( !alert )
    return nullptr;
  string msg = format("ObjectExtensions::extension","The object has no extension of type %016llX.",key);
  throw runtime_error(msg);
}
