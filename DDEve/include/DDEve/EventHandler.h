// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_EVENTHANDLER_H
#define DD4HEP_DDEVE_EVENTHANDLER_H

// Framework include files
#include "DDEve/DDEveEventData.h"
#include "TClass.h"
class TH2F;

// C/C++ include files
#include <map>
#include <set>
#include <vector>

// Forward declarations

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  class EventConsumer;
  /// Event data actor base class. Used to extract data from concrete classes.
  /** 
   * @author  M.Frank
   * @version 1.0
   */
  struct DDEveHitActor {
    virtual ~DDEveHitActor() {}
    virtual void operator()(const DDEveHit&) = 0;
    virtual void setSize(size_t /* num_elements */) {}
  };

  /// Event handler base class. Interface to all DDEve I/O actions
  /** 
   * @author  M.Frank
   * @version 1.0
   */
  class EventHandler {
  public:
    /// Collection definition: name, size
    typedef std::pair<const char*, size_t> Collection;
    /// Types collection: collections are grouped by type (class name)
    typedef std::map<std::string, std::vector<Collection>> TypedEventCollections;
    /// Subscriber set
    typedef std::set<EventConsumer*> Subscriptions;

  protected:
    /// Data collection map
    TypedEventCollections m_data;
    /// Data subscriptions (unordered)
    Subscriptions m_subscriptions;
    /// Flag to indicate that a file is opened
    bool m_hasFile;
    /// Flag to indicate that an event is loaded
    bool m_hasEvent;

  public:
    /// Standard constructor
    EventHandler();
    /// Default destructor
    virtual ~EventHandler();
    /// Access the map of simulation data collections
    virtual const TypedEventCollections& data() const { return m_data; }
    /// Check if an event is present in memory
    virtual bool hasEvent() const { return m_hasEvent; }
    /// Check if a data file is connected to the handler
    virtual bool hasFile() const { return m_hasFile; }
    /// Access the number of events on the current input data source (-1 if no data source connected)
    virtual long numEvents() const = 0;
    /// Access the data source name
    virtual std::string datasourceName() const = 0;
    /// Loop over collection and extract data
    virtual size_t collectionLoop(const std::string& collection, DDEveHitActor& actor) = 0;
    /// Clear all event related data caches
    virtual void ClearCache();
    /// Open a new event data file
    virtual bool Open(const std::string& file_name) = 0;
    /// Load the next event
    virtual bool NextEvent() = 0;
    /// User overloadable function: Load the previous event
    virtual bool PreviousEvent() = 0;
    /// Goto a specified event in the file
    virtual bool GotoEvent(long event_number) = 0;

    /// Subscribe to notification of new data present
    virtual void Subscribe(EventConsumer* display);
    /// Unsubscribe from notification of new data present
    virtual void Unsubscribe(EventConsumer* display);

#ifndef __CINT__
    /// Notfy all subscribers
    virtual void NotifySubscribers(void (EventConsumer::*pmf)(EventHandler*));
#endif
    ClassDef(EventHandler, 0);
  };

  /** @class EventConsumer  EventHandler.h DDEve/EventHandler.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class EventConsumer {
  public:
    /// Standard constructor
    EventConsumer();
    /// Default destructor
    virtual ~EventConsumer();
    /// Consumer event data callback
    virtual void OnNewEvent(EventHandler* /* handler */) {}
    /// Consumer file open callback
    virtual void OnFileOpen(EventHandler* /* handler */) {}

    ClassDef(EventConsumer, 0);
  };

} /* End namespace DD4hep   */

#endif /* DD4HEP_DDEVE_EVENTHANDLER_H */
