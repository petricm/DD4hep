// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DD4HEP_DDEVE_CALOLEGO_H
#define DD4HEP_DDEVE_CALOLEGO_H

// Framework include files
#include "DDEve/Display.h"
#include "DDEve/View.h"

// Forward declarations
class TH2F;
class TEveCalo3D;
class TEveCaloDataHist;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Lego plot for calorimeter energy deposits
  /*
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class CaloLego : public View {
    Display::CalodataContext m_data;

  public:
    /// Initializing constructor
    CaloLego(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~CaloLego();
    /// Build the projection view and map it to the given slot
    virtual View& Build(TEveWindow* slot);
    /// Configure a single geometry view
    virtual void ConfigureGeometry(const DisplayConfiguration::ViewConfig& config);
    /// Configure a single event scene view
    virtual void ConfigureEvent(const DisplayConfiguration::ViewConfig& config);
    /// Call to import geometry topics
    void ImportGeoTopics(const std::string& title);

    /// Root implementation macro
    ClassDef(CaloLego, 0);
  };

} /* End namespace DD4hep   */

#endif /* DD4HEP_DDEVE_CALOLEGO_H */
