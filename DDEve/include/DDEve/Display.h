// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_DISPLAY_H
#define DD4HEP_DDEVE_DISPLAY_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DDEve/DisplayConfiguration.h"
#include "DDEve/EventHandler.h"
#include "DDEve/PopupMenu.h"

// C/C++ include files
#include <set>

// Forward declarations
class TEveCaloDataHist;
class TEveCalo3D;
class TEveElementList;
class TEveManager;
class TEveGeoShape;
class TGeoManager;
class TEveElement;
class TEveCaloViz;
class TGMenuBar;
class TGClient;
class TFile;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  class View;
  class ViewMenu;
  class DD4hepMenu;
  class ViewConfiguration;
  class CalodataConfiguration;
  class EventHandler;
  class DisplayConfiguration;

  /** @class Display  Display.h DDEve/Display.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class Display : public EventConsumer {
  public:
    typedef DisplayConfiguration::Config    CalodataConfiguration;
    typedef std::set<View*>                 Views;
    typedef std::set<DisplayConfiguration*> Configurations;
    typedef std::set<PopupMenu*>            Menus;
    typedef std::map<std::string, TEveElementList*>      Topics;
    typedef std::map<std::string, ViewConfiguration*>    ViewConfigurations;
    typedef std::map<std::string, CalodataConfiguration> CalodataConfigurations;

    struct CalodataContext {
      int                          slice;
      TEveCalo3D*                  calo3D;
      TEveCaloViz*                 caloViz;
      TEveCaloDataHist*            eveHist;
      DisplayConfiguration::Config config;
      CalodataContext();
      CalodataContext(const CalodataContext& c);
      CalodataContext& operator=(const CalodataContext& c);
    };
    typedef std::map<std::string, CalodataContext> Calodata;

  protected:
    /// Reference to TEve manager
    TEveManager* m_eve;
    /// Reference to geometry hub
    Geometry::LCDD* m_lcdd;
    /// Reference to the event reader object
    EventHandler*          m_evtHandler;
    TEveElementList*       m_geoGlobal;
    TEveElementList*       m_eveGlobal;
    ViewMenu*              m_viewMenu;
    DD4hepMenu*            m_dd4Menu;
    Topics                 m_geoTopics;
    Topics                 m_eveTopics;
    Views                  m_eveViews;
    Menus                  m_menus;
    ViewConfigurations     m_viewConfigs;
    CalodataConfigurations m_calodataConfigs;
    /// Container with calorimeter data (projections)
    Calodata m_calodata;
    /// TGeoManager visualisation level
    int m_visLevel;
    /// Load level for the eve geometry
    int m_loadLevel;

  public:
    /// Standard constructor
    Display(TEveManager* eve);
    /// Default destructor
    virtual ~Display();

    /// Access to geometry hub
    Geometry::LCDD& lcdd() const;
    /// Access to the EVE manager
    TEveManager& manager() const { return *m_eve; }
    /// Access View configurations
    const ViewConfigurations& viewConfigurations() const { return m_viewConfigs; }
    /// Set Vis level in geo manager (either from XML or BEFORE XML file was loaded)
    void setVisLevel(int new_level) { m_visLevel = new_level; }
    /// Set Eve Geometry load level in manager (either from XML or BEFORE XML file was loaded)
    void setLoadLevel(int new_level) { m_loadLevel = new_level; }
    /// Access to X-client
    TGClient& client() const;

    /// Load geometry with panel
    void ChooseGeometry();
    /// Load geometry from compact xml file
    void LoadXML(const char* xmlFile);
    /// Load geometry from compact xml file
    void LoadGeometryRoot(const char* rootFile);
    /// Open ROOT file
    TFile* Open(const char* rootFile) const;

    /// Access to the event reader
    EventHandler& eventHandler() const;

    /// Open standard message box
    void MessageBox(PrintLevel level, const std::string& text, const std::string& title = "") const;

    /// Popup XML file chooser. returns chosen file name; empty on cancel
    std::string OpenXmlFileDialog(const std::string& default_dir) const;

    /// Popup ROOT file chooser. returns chosen file name; empty on cancel
    std::string OpenRootFileDialog(const std::string& default_dir) const;

    /// Load 'levels' Children into the geometry scene
    void LoadGeoChildren(TEveElement* start, int levels, bool redraw);
    /// Make a set of nodes starting from a top element (in-)visible with a given depth
    void MakeNodesVisible(TEveElement* e, bool visible, int level);

    /// Import configuration parameters
    void ImportConfiguration(const DisplayConfiguration& config);

    /// Register a data filter by name
    void RegisterViewConfiguration(ViewConfiguration* filter);
    /// Access a data filter by name. Data filters are used to customize views
    ViewConfiguration* GetViewConfiguration(const std::string& name) const;
    /// Access a data filter by name. Data filters are used to customize views
    const CalodataConfiguration* GetCalodataConfiguration(const std::string& name) const;
    /// Access to calo data histograms by name as defined in the configuration
    CalodataContext& GetCaloHistogram(const std::string& name);

    /// Configure a view using the view's name and a proper ViewConfiguration if present
    virtual void ConfigureGeometry(View* view);

    /// Prepare the view for adding event data
    virtual void PrepareEvent(View* view) const;
    /// Configure the adding of event data
    virtual void ConfigureEvent(View* view) const;
    /// Register to the main event scene on new events
    virtual void RegisterEvents(View* view);
    /// Unregister from the main event scene
    virtual void UnregisterEvents(View* view);

    /// Access / Create global geometry element
    TEveElementList& GetGeo();
    /// Access/Create an geometry topic by name
    virtual TEveElementList& GetGeoTopic(const std::string& name);
    /// Access/Create an geometry topic by name
    virtual TEveElementList& GetGeoTopic(const std::string& name) const;

    /// Call to import geometry elements
    void ImportGeo(TEveElement* el);
    /// Call to import geometry elements by topic
    void ImportGeo(const std::string& topic, TEveElement* el);

    /// Access/Create an event topic by name
    virtual TEveElementList& GetEve();
    /// Access/Create an event topic by name
    virtual TEveElementList& GetEveTopic(const std::string& name);
    /// Access/Create an event topic by name
    virtual TEveElementList& GetEveTopic(const std::string& name) const;

    /// Call to import top level event elements
    void ImportEvent(TEveElement* el);
    /// Call to import event elements by topic
    void ImportEvent(const std::string& topic, TEveElement* el);

    /// EventConsumer overload: Consumer event data
    virtual void OnNewEvent(EventHandler* handler);

    /// Build the DDEve specific menues. Default bar is the ROOT browser's bar
    virtual void BuildMenus(TGMenuBar* bar = 0);
    /// Add new menu to the main menu bar
    virtual void AddMenu(TGMenuBar* bar, PopupMenu* menu, int hints = kLHintsNormal);

    ClassDef(Display, 0);
  };

} /* End namespace DD4hep   */

#endif /* DD4HEP_DDEVE_DISPLAY_H */
