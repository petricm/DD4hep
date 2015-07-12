// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_DDEVE_DICTIONARY_H
#define DD4HEP_DDEVE_DICTIONARY_H

// Framework include files
#include "DDEve/Calo2DProjection.h"
#include "DDEve/Calo3DProjection.h"
#include "DDEve/CaloLego.h"
#include "DDEve/Display.h"
#include "DDEve/EventControl.h"
#include "DDEve/EventHandler.h"
#include "DDEve/MultiView.h"
#include "DDEve/RhoPhiProjection.h"
#include "DDEve/RhoZProjection.h"
#include "DDEve/View3D.h"

#include "DDEve/Annotation.h"
#include "DDEve/ContextMenu.h"
#include "DDEve/DD4hepMenu.h"
#include "DDEve/DisplayConfiguration.h"
#include "DDEve/ElementList.h"
#include "DDEve/EvePgonSetProjectedContextMenu.h"
#include "DDEve/EveShapeContextMenu.h"
#include "DDEve/ViewMenu.h"

#include "DDEve/DDG4EventHandler.h"
#include "DDEve/GenericEventHandler.h"

namespace DD4hep {
  void EveDisplay(const char* xmlFile);
}

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace DD4hep;

#pragma link C++ function DD4hep::EveDisplay(const char* xmlFile);

#pragma link C++ class DD4hep::Display;
#pragma link C++ class DD4hep::Annotation;
#pragma link C++ class DD4hep::ElementList;
#pragma link C++ class DD4hep::FrameControl;
#pragma link C++ class DD4hep::EventHandler;
#pragma link C++ class DD4hep::EventConsumer;
#pragma link C++ class DD4hep::DDG4EventHandler;
#pragma link C++ class DD4hep::GenericEventHandler;
#pragma link C++ class DD4hep::EventControl;

#pragma link C++ class DD4hep::DisplayConfiguration::Config;
//#pragma link C++ class std::list<DD4hep::DisplayConfiguration::Config>;
#pragma link C++ class DD4hep::DisplayConfiguration::ViewConfig;
//#pragma link C++ class std::list<DD4hep::DisplayConfiguration::ViewConfig>;
#pragma link C++ class DD4hep::DisplayConfiguration;

#pragma link C++ class DD4hep::View;
#pragma link C++ class DD4hep::View3D;
#pragma link C++ class DD4hep::Projection;
#pragma link C++ class DD4hep::RhoZProjection;
#pragma link C++ class DD4hep::RhoPhiProjection;
#pragma link C++ class DD4hep::CaloLego;
#pragma link C++ class DD4hep::Calo2DProjection;
#pragma link C++ class DD4hep::Calo3DProjection;
#pragma link C++ class DD4hep::MultiView;

#pragma link C++ class DD4hep::PopupMenu;
#pragma link C++ class DD4hep::ViewMenu;
#pragma link C++ class DD4hep::DD4hepMenu;
#pragma link C++ class DD4hep::ContextMenu;
#pragma link C++ class DD4hep::ContextMenuHandler;
#pragma link C++ class DD4hep::EveUserContextMenu;
#pragma link C++ class DD4hep::ElementListContextMenu;
#pragma link C++ class DD4hep::EveShapeContextMenu;
#pragma link C++ class DD4hep::EvePgonSetProjectedContextMenu;
//#pragma link C++ class DD4hep::;

#endif

#endif /* DD4HEP_DDEVE_DICTIONARY_H */
