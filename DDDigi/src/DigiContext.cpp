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
#include "DD4hep/InstanceCount.h"
#include "DDDigi/DigiContext.h"
#include "DDDigi/DigiKernel.h"

// C/C++ include files
#include <algorithm>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::digi;

/// Default constructor
DigiContext::DigiContext(const DigiKernel* k, DigiEvent* e)
  : m_kernel(k), m_event(e)
{
  if ( !m_kernel )    {
    except("DigiContext","Cannot initialize Digitization context with invalid DigiKernel!");
  }
  InstanceCount::increment(this);
}

/// Default destructor
DigiContext::~DigiContext() {
  // Do not delete run and event structures here. This is done outside in the framework
  InstanceCount::decrement(this);
}

/// Set the geant4 event reference
void DigiContext::setEvent(DigiEvent* new_event)   {
  m_event = new_event;
}

/// Access the geant4 event -- valid only between BeginEvent() and EndEvent()!
auto DigiContext::event()  const -> DigiEvent&   {
  if ( m_event ) return *m_event;
  invalidHandleError<DigiEvent>();
  return *m_event;
}

/// Access to detector description
auto DigiContext::detectorDescription()  const -> Detector& {
  return m_kernel->detectorDescription();
}

/// Generic framework access
auto DigiContext::userFramework()  const -> DigiContext::UserFramework&  {
  return m_kernel->userFramework();
}

/// Access to the main input action sequence from the kernel object
auto DigiContext::inputAction() const -> DigiActionSequence&    {
  return m_kernel->inputAction();
}

/// Access to the main event action sequence from the kernel object
auto DigiContext::eventAction()  const -> DigiActionSequence&  {
  return m_kernel->eventAction();
}

/// Access to the main output action sequence from the kernel object
auto DigiContext::outputAction() const -> DigiActionSequence&    {
  return m_kernel->outputAction();
}

