// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DDEve/EventHandler.h"

using namespace DD4hep;

ClassImp(EventHandler)
ClassImp(EventConsumer)

/// Standard constructor
EventHandler::EventHandler() : m_hasFile(false), m_hasEvent(false) {
}

/// Default destructor
EventHandler::~EventHandler()   {
}

/// Standard constructor
EventConsumer::EventConsumer()  {
}

/// Default destructor
EventConsumer::~EventConsumer()   {
}


