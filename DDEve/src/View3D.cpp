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
// Original Author: Matevz Tadel 2009 (MultiView.C)
//
//==========================================================================

// Framework include files
#include "DDEve/View3D.h"
#include "DDEve/Factories.h"

using namespace DD4hep;

ClassImp(View3D)
DECLARE_VIEW_FACTORY(View3D)

/// Initializing constructor
View3D::View3D(Display* eve, const std::string& nam) : View(eve, nam)
{
}

/// Default destructor
View3D::~View3D()  {
}

/// Build the projection view and map it to the given slot
View& View3D::Build(TEveWindow* slot)   {
  return CreateScenes().Map(slot);
}
