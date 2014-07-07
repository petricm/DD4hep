// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DD4HEP_DDEVE_MULTIVIEW_H
#define DD4HEP_DDEVE_MULTIVIEW_H

// Framework include files
#include "DDEve/DisplayConfiguration.h"
#include "DDEve/View.h"

class TEveWindowPack;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class MultiView  MultiView.h DDEve/MultiView.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class MultiView : public View {
  public:
    /// Initializing constructor
    MultiView(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~MultiView();
    /// Build the 3d view and map it to the given slot
    virtual View& Build(TEveWindow* slot);
    /// Root implementation macro
    ClassDef(MultiView, 0);
  };
} /* End namespace DD4hep   */

#endif /* DD4HEP_DDEVE_MULTIVIEW_H */
