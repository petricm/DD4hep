// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_EVESHAPECONTEXTMENU_H
#define DD4HEP_DDEVE_EVESHAPECONTEXTMENU_H

// ROOT include files
#include "DDEve/EveUserContextMenu.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class EveShapeContextMenu  EveShapeContextMenu.h DDEve/EveShapeContextMenu.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class EveShapeContextMenu : public EveUserContextMenu {
  protected:
    /// Initializing constructor
    EveShapeContextMenu(Display* m);
    /// Default destructor
    virtual ~EveShapeContextMenu();

  public:
    /// Instantiator
    static EveShapeContextMenu& install(Display* m);
    /// Root implementation macro
    ClassDef(EveShapeContextMenu, 0);
  };

} /* End namespace DD4hep   */

#endif /* DD4HEP_DDEVE_EVESHAPECONTEXTMENU_H */
