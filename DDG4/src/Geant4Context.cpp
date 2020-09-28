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
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Kernel.h"

// C/C++ include files
#include <algorithm>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

/// Intializing constructor
Geant4Run::Geant4Run(const G4Run* run_pointer)
  : ObjectExtensions(typeid(Geant4Run)), m_run(run_pointer)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Run::~Geant4Run()   {
  InstanceCount::decrement(this);
}

/// Intializing constructor
Geant4Event::Geant4Event(const G4Event* evt, Geant4Random* rnd)
  : ObjectExtensions(typeid(Geant4Event)), m_event(evt), m_random(rnd)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Event::~Geant4Event()  {
  InstanceCount::decrement(this);
}

/// Default constructor
Geant4Context::Geant4Context(Geant4Kernel* kernel_pointer)
  : m_kernel(kernel_pointer) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Context::~Geant4Context() {
  // Do not delete run and event structures here. This is done outside in the framework
  InstanceCount::decrement(this);
}

/// Access to geometry world
auto Geant4Context::world()  const -> G4VPhysicalVolume*  {
  return m_kernel->world();
}

/// Set the geant4 run reference
void Geant4Context::setRun(Geant4Run* new_run)    {
  m_run = new_run;
}

/// Access the geant4 run -- valid only between BeginRun() and EndRun()!
auto Geant4Context::run()  const -> Geant4Run&   {
  if ( m_run ) return *m_run;
  invalidHandleError<Geant4Run>();
  return *m_run;
}

/// Set the geant4 event reference
void Geant4Context::setEvent(Geant4Event* new_event)   {
  m_event = new_event;
}

/// Access the geant4 event -- valid only between BeginEvent() and EndEvent()!
auto Geant4Context::event()  const -> Geant4Event&   {
  if ( m_event ) return *m_event;
  invalidHandleError<Geant4Event>();
  return *m_event;
}

/// Access to detector description
auto Geant4Context::detectorDescription() const -> Detector& {
  return m_kernel->detectorDescription();
}

/// Generic framework access
auto Geant4Context::userFramework() const -> Geant4Context::UserFramework&  {
  return m_kernel->userFramework();
}

/// Create a user trajectory
auto Geant4Context::createTrajectory(const G4Track* /* track */) const -> G4VTrajectory* {
  string err = dd4hep::format("Geant4Kernel", "createTrajectory: Purely virtual method. requires overloading!");
  dd4hep::printout(dd4hep::FATAL, "Geant4Kernel", "createTrajectory: Purely virtual method. requires overloading!");
  throw runtime_error(err);
}

/// Access the tracking manager
auto Geant4Context::trackMgr() const -> G4TrackingManager* {
  return m_kernel->trackMgr();
}

/// Access to the main run action sequence from the kernel object
auto Geant4Context::runAction() const -> Geant4RunActionSequence& {
  return m_kernel->runAction();
}

/// Access to the main event action sequence from the kernel object
auto Geant4Context::eventAction() const -> Geant4EventActionSequence& {
  return m_kernel->eventAction();
}

/// Access to the main stepping action sequence from the kernel object
auto Geant4Context::steppingAction() const -> Geant4SteppingActionSequence& {
  return m_kernel->steppingAction();
}

/// Access to the main tracking action sequence from the kernel object
auto Geant4Context::trackingAction() const -> Geant4TrackingActionSequence& {
  return m_kernel->trackingAction();
}

/// Access to the main stacking action sequence from the kernel object
auto Geant4Context::stackingAction() const -> Geant4StackingActionSequence& {
  return m_kernel->stackingAction();
}

/// Access to the main generator action sequence from the kernel object
auto Geant4Context::generatorAction() const -> Geant4GeneratorActionSequence& {
  return m_kernel->generatorAction();
}

/// Access to the main generator action sequence from the kernel object
auto Geant4Context::sensitiveActions() const -> Geant4SensDetSequences& {
  return m_kernel->sensitiveActions();
}
