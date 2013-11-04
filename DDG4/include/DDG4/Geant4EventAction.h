// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4EVENTACTION_H
#define DD4HEP_DDG4_GEANT4EVENTACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"
// Forward declarations
class G4Event;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4EventAction Geant4Action.h DDG4/Geant4Action.h
     * 
     * Concrete basic implementation of the Geant4 event action 
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4EventAction : public Geant4Action {
    public:
      /// Standard constructor
      Geant4EventAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4EventAction();
      /// begin-of-event callback
      virtual void begin(const G4Event* event);
      /// End-of-event callback
      virtual void end(const G4Event* event);
    };

    /** @class Geant4EventActionSequence Geant4Action.h DDG4/Geant4Action.h
     * 
     * Concrete implementation of the Geant4 event action sequence
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4EventActionSequence : public Geant4Action {
    protected:
      /// Callback sequence for event initialization action
      CallbackSequence m_begin;
      /// Callback sequence for event finalization action
      CallbackSequence m_end;
      /// The list of action objects to be called
      Actors<Geant4EventAction> m_actors;

    public:
      /// Standard constructor
      Geant4EventActionSequence(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4EventActionSequence();
      /// Register begin-of-event callback
      template <typename Q, typename T> void callAtBegin(Q* p, void (T::*f)(const G4Event*)) { m_begin.add(p, f); }
      /// Register end-of-event callback
      template <typename Q, typename T> void callAtEnd(Q* p, void (T::*f)(const G4Event*)) { m_end.add(p, f); }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4EventAction* action);
      /// begin-of-event callback
      virtual void begin(const G4Event* event);
      /// End-of-event callback
      virtual void end(const G4Event* event);
    };

  }  // End namespace Simulation
}  // End namespace DD4hep

#endif  // DD4HEP_DDG4_GEANT4EVENTACTION_H
