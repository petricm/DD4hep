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
#include "DDEve/GenericEventHandler.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Plugins.h"
#include <stdexcept>

/// ROOT include files
#include "TGMsgBox.h"
#include "TSystem.h"
#include <climits>

using namespace std;
using namespace dd4hep;

ClassImp(GenericEventHandler)

/// Standard constructor
GenericEventHandler::GenericEventHandler() : m_current(nullptr) {
}

/// Default destructor
GenericEventHandler::~GenericEventHandler()  {
  m_subscriptions.clear();
  detail::deletePtr(m_current);
}

auto GenericEventHandler::current() const -> EventHandler*   {
  if ( m_current )  {
    return m_current;
  }
  throw runtime_error("Invalid event handler");
}

/// Notfy all subscribers
void GenericEventHandler::NotifySubscribers(void (EventConsumer::*pmf)(EventHandler&))   {
  for(auto m_subscription : m_subscriptions)
    (m_subscription->*pmf)(*this);
}

/// Subscribe to notification of new data present
void GenericEventHandler::Subscribe(EventConsumer* consumer)  {
  m_subscriptions.insert(consumer);
}

/// Unsubscribe from notification of new data present
void GenericEventHandler::Unsubscribe(EventConsumer* consumer)  {
  auto i=m_subscriptions.find(consumer);
  if ( i != m_subscriptions.end() ) m_subscriptions.erase(i);
}

/// Access the number of events on the current input data source (-1 if no data source connected)
auto GenericEventHandler::numEvents() const -> long   {
  return current()->numEvents();
}

/// Access the data source name
auto GenericEventHandler::datasourceName() const -> string {
  return current()->datasourceName();
}

/// Access to the collection type by name
auto GenericEventHandler::collectionType(const string& collection) const -> EventHandler::CollectionType   {
  if ( m_current && m_current->hasEvent() )  {
    return m_current->collectionType(collection);
  }
  return NO_COLLECTION;
}

/// Loop over collection and extract data
auto GenericEventHandler::collectionLoop(const string& collection, DDEveHitActor& actor) -> size_t {
  if ( m_current && m_current->hasEvent() )  {
    return m_current->collectionLoop(collection,actor);
  }
  return 0;
}

/// Loop over collection and extract particle data
auto GenericEventHandler::collectionLoop(const string& collection, DDEveParticleActor& actor) -> size_t    {
  if ( m_current && m_current->hasEvent() )  {
    return m_current->collectionLoop(collection,actor);
  }
  return 0;
}

/// Open a new event data file
auto GenericEventHandler::Open(const string& file_type, const string& file_name) -> bool   {
  size_t idx = file_name.find("lcio");
  size_t idr = file_name.find("root");
  string err;
  m_hasFile = false;
  m_hasEvent = false;
  try  {
    detail::deletePtr(m_current);
    //  prefer event handler configured in xml
    if ( file_type.find("FCC") != string::npos ) {
      m_current = (EventHandler*)PluginService::Create<void*>("DDEve_FCCEventHandler",(const char*)nullptr);
    }
    // fall back to defaults according to file ending
    else if ( idx != string::npos )   {
      m_current = (EventHandler*)PluginService::Create<void*>("DDEve_LCIOEventHandler",(const char*)nullptr);
    }
    else if ( idr != string::npos )   {
      m_current = (EventHandler*)PluginService::Create<void*>("DDEve_DDG4EventHandler",(const char*)nullptr);
    }
    else   {
      throw runtime_error("Attempt to open file:"+file_name+" of unknown type:"+file_type);
    }
    if ( m_current )   {
      if ( m_current->Open(file_type, file_name) )   {
        m_hasFile = true;
        NotifySubscribers(&EventConsumer::OnFileOpen);
        return true;
      }
      err = "+++ Failed to open the data file:"+file_name;
      detail::deletePtr(m_current);   
    }
    else  {
      err = "+++ Failed to create fikle reader for file '"+file_name+"' of type '"+file_type+"'";
    }
  }
  catch(const exception& e)  {
    err = "\nAn exception occurred \n"
      "while opening event data:\n" + string(e.what()) + "\n\n";
  }
  string path = TString::Format("%s/icons/stop_t.xpm", gSystem->Getenv("ROOTSYS")).Data();
  const TGPicture* pic = gClient->GetPicture(path.c_str());
  new TGMsgBox(gClient->GetRoot(),nullptr,"Failed to open event data",err.c_str(),pic,
               kMBDismiss,nullptr,kVerticalFrame,kTextLeft|kTextCenterY);
  return false;
}

/// Load the next event
auto GenericEventHandler::NextEvent() -> bool   {
  m_hasEvent = false;
  try {
    if ( m_hasFile )   {
      if ( current()->NextEvent() > 0 )   {
        m_hasEvent = true;
        NotifySubscribers(&EventConsumer::OnNewEvent);
        return true;
      }
    }
    throw runtime_error("+++ EventHandler::readEvent: No file open!");
  }
  catch(const exception& e)  {
    string path = TString::Format("%s/icons/stop_t.xpm", gSystem->Getenv("ROOTSYS")).Data();
    string err = "\nAn exception occurred \n"
      "while reading a new event:\n" + string(e.what()) + "\n\n";
    const TGPicture* pic = gClient->GetPicture(path.c_str());
    new TGMsgBox(gClient->GetRoot(),nullptr,"Failed to read event", err.c_str(),pic,
                 kMBDismiss,nullptr,kVerticalFrame,kTextLeft|kTextCenterY);
  }
  return -1;
}

/// User overloadable function: Load the previous event
auto GenericEventHandler::PreviousEvent() -> bool    {
  m_hasEvent = false;
  if ( m_hasFile && current()->PreviousEvent() > 0 )   {
    m_hasEvent = true;
    NotifySubscribers(&EventConsumer::OnNewEvent);
    return true;
  }
  return -1;
}

/// Goto a specified event in the file
auto GenericEventHandler::GotoEvent(long event_number) -> bool   {
  m_hasEvent = false;
  if ( m_hasFile && current()->GotoEvent(event_number) > 0 )   {
    m_hasEvent = true;
    NotifySubscribers(&EventConsumer::OnNewEvent);
    return true;
  }
  return -1;
}
