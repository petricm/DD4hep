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
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"

#include "DDG4/Geant4ActionContainer.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4StackingAction.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Geant4UserInitialization.h"
#include "DDG4/Geant4SensDetAction.h"

// C/C++ include files
#include <stdexcept>
#include <algorithm>

using namespace std;
using namespace dd4hep::sim;

/// Standard constructor
Geant4ActionContainer::Geant4ActionContainer(Geant4Context* ctxt)
  : m_context(ctxt), 
    m_generatorAction(nullptr), m_runAction(nullptr), m_eventAction(nullptr), m_trackingAction(nullptr),
    m_steppingAction(nullptr), m_stackingAction(nullptr), m_constructionAction(nullptr),
    m_sensDetActions(nullptr), m_physicsList(nullptr), m_userInit(nullptr)
{
  m_sensDetActions = new Geant4SensDetSequences();
  InstanceCount::increment(this);
}

/// Default destructor
Geant4ActionContainer::~Geant4ActionContainer() {
  this->Geant4ActionContainer::terminate();
  InstanceCount::decrement(this);
}

/// Terminate all associated action instances
auto Geant4ActionContainer::terminate() -> int {
  detail::releasePtr (m_physicsList);
  detail::releasePtr (m_constructionAction);
  detail::releasePtr (m_stackingAction);
  detail::releasePtr (m_steppingAction);
  detail::releasePtr (m_trackingAction);
  detail::releasePtr (m_eventAction);
  detail::releasePtr (m_generatorAction);
  detail::releasePtr (m_runAction);
  detail::deletePtr  (m_sensDetActions);
  detail::deletePtr  (m_context);
  return 1;
}

auto Geant4ActionContainer::workerContext() -> Geant4Context*   {
  if ( m_context ) return m_context;
  throw runtime_error(format("Geant4Kernel", "DDG4: Master kernel object has no thread context! [Invalid Handle]"));
}

/// Set the thread's context
void Geant4ActionContainer::setContext(Geant4Context* ctxt)    {
  m_context = ctxt;
}

template <class C> auto Geant4ActionContainer::registerSequence(C*& seq, const std::string& name) -> bool {
  if (!name.empty()) {
    seq = new C(m_context, name);
    seq->installMessengers();
    return true;
  }
  throw runtime_error(format("Geant4ActionContainer", "DDG4: The action '%s' not found. [Action-NotFound]", name.c_str()));
}

/// Access generator action sequence
auto Geant4ActionContainer::generatorAction(bool create) -> Geant4GeneratorActionSequence* {
  if (!m_generatorAction && create)
    registerSequence(m_generatorAction, "GeneratorAction");
  return m_generatorAction;
}

/// Access run action sequence
auto Geant4ActionContainer::runAction(bool create) -> Geant4RunActionSequence* {
  if (!m_runAction && create)
    registerSequence(m_runAction, "RunAction");
  return m_runAction;
}

/// Access event action sequence
auto Geant4ActionContainer::eventAction(bool create) -> Geant4EventActionSequence* {
  if (!m_eventAction && create)
    registerSequence(m_eventAction, "EventAction");
  return m_eventAction;
}

/// Access stepping action sequence
auto Geant4ActionContainer::steppingAction(bool create) -> Geant4SteppingActionSequence* {
  if (!m_steppingAction && create)
    registerSequence(m_steppingAction, "SteppingAction");
  return m_steppingAction;
}

/// Access tracking action sequence
auto Geant4ActionContainer::trackingAction(bool create) -> Geant4TrackingActionSequence* {
  if (!m_trackingAction && create)
    registerSequence(m_trackingAction, "TrackingAction");
  return m_trackingAction;
}

/// Access stacking action sequence
auto Geant4ActionContainer::stackingAction(bool create) -> Geant4StackingActionSequence* {
  if (!m_stackingAction && create)
    registerSequence(m_stackingAction, "StackingAction");
  return m_stackingAction;
}

/// Access detector construcion action sequence (geometry+sensitives+field)
auto Geant4ActionContainer::detectorConstruction(bool create) -> Geant4DetectorConstructionSequence*  {
  if (!m_constructionAction && create)
    registerSequence(m_constructionAction, "DetectorConstructionAction");
  return m_constructionAction;
}

/// Access to the sensitive detector sequences from the kernel object
auto Geant4ActionContainer::sensitiveActions() const -> Geant4SensDetSequences& {
  return *m_sensDetActions;
}

/// Access to the sensitive detector action from the kernel object
auto Geant4ActionContainer::sensitiveAction(const string& nam) -> Geant4SensDetActionSequence* {
  Geant4SensDetActionSequence* ptr = m_sensDetActions->find(nam);
  if (ptr)   {
    return ptr;
  }
  ptr = new Geant4SensDetActionSequence(workerContext(), nam);
  m_sensDetActions->insert(nam, ptr);
  return ptr;
}

/// Access to the physics list
auto Geant4ActionContainer::physicsList(bool create) -> Geant4PhysicsListActionSequence* {
  if (!m_physicsList && create)
    registerSequence(m_physicsList, "PhysicsList");
  return m_physicsList;
}

/// Access to the physics list
auto Geant4ActionContainer::userInitialization(bool create) -> Geant4UserInitializationSequence* {
  if (!m_userInit && create)   {
    registerSequence(m_userInit, "UserInitialization");
  }
  return m_userInit;
}
