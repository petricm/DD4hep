// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4ACTION_H
#define DD4HEP_DDG4_GEANT4ACTION_H

// Framework include files
#include "DD4hep/Printout.h"
#include "DDG4/ComponentProperties.h"
#include "DDG4/Geant4Callback.h"
#include "DDG4/Geant4Context.h"

// Geant4 forward declarations
class G4Run;
class G4Event;
class G4Step;
class G4Track;
class G4TrackStack;
class G4EventGenerator;
class G4VTrajectory;
class G4TrackingManager;
class G4UIdirectory;

// C/C++ include files
#include <cstdarg>
#include <string>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations
    class Geant4UIMessenger;

    template <typename TO, typename FROM> TO fast_cast(FROM from) {
#ifdef USE_FASTCAST
      return static_cast<TO>(from);
#else
      return dynamic_cast<TO>(from);
#endif
    }

    struct TypeName : public std::pair<std::string, std::string> {
      TypeName() : std::pair<std::string, std::string>() {}
      TypeName(const std::pair<std::string, std::string>& c) : std::pair<std::string, std::string>(c) {}
      TypeName(const std::string& typ, const std::string& nam) : std::pair<std::string, std::string>(typ, nam) {}
      /// Split string pair according to default delimiter ('/')
      static TypeName split(const std::string& type_name);
      /// Split string pair according to custom delimiter
      static TypeName split(const std::string& type_name, const std::string& delim);
    };

#if 0

    /** @class Geant4UserTrajectory Geant4Action.h DDG4/Geant4Action.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserTrajectory {
      /// Default constructor
      Geant4UserTrajectory();
      /// Standard destructor
      virtual ~Geant4UserTrajectory();
      /// accessors
      virtual int trackID() const = 0;
      virtual int parentID() const = 0;
      virtual std::string particleName() const = 0;
      virtual double charge() const = 0;
      virtual int pdgID() const = 0;
      virtual G4ThreeVector momentum() const = 0;
      virtual int numPoints() const = 0;
      virtual G4VTrajectoryPoint* point(int i) const = 0;
    };

    /** @class Geant4Trajectory Geant4Action.h DDG4/Geant4Action.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4Trajectory : public G4VTrajectory {
      std::auto_ptr<Geant4UserTrajectory> trajectory;
      /// Default constructor
      Geant4Trajectory(Geant4UserTrajectory* traj);
      /// Standard destructor
      virtual ~Geant4Trajectory();
      /// Mandatory G4 overloads: Get/Set functions
      virtual G4int GetTrackID() const {return trajectory->trackID;}
      virtual G4int GetParentID() const {return trajectory->parentID();}
      virtual G4String GetParticleName() const {return trajectory->particleName();}
      /// Mandatory G4 overloads:  Charge is that of G4DynamicParticle
      virtual G4double GetCharge() const {return trajectory->charge();}
      /// Mandatory G4 overloads:  Zero will be returned if the particle does not have PDG code.
      virtual G4int GetPDGEncoding() const {return trajectory->pdgID();}
      /// Mandatory G4 overloads:  Momentum at the origin of the track in global coordinate system.
      virtual G4ThreeVector GetInitialMomentum() const {return trajectory->momentum();}

      /// Mandatory G4 overloads:  Returns the number of trajectory points
      virtual int GetPointEntries() const {return trajectory->numPoints();}
      virtual G4VTrajectoryPoint* GetPoint(G4int i) const {return trajectory->point(i);}
    };
#endif

    /// Default base class for all geant 4 actions and derivates thereof.
    /**
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4Action {
    protected:
      /// Reference to the Geant4 context
      Geant4Context m_context;
      /// Control directory of this action
      Geant4UIMessenger* m_control;

      /// Default property: Output level
      int m_outputLevel;
      /// Default property: Flag to create control instance
      bool m_needsControl;
      /// Action name
      std::string m_name;
      /// Property pool
      PropertyManager m_properties;
      /// Reference count. Initial value: 1
      long m_refCount;

      //fg: ContextUpdate needs to be public for the clang compiler
      //    as it is used in SequenceHdl::setContextToClients()
    public:
      /// Functor to update the context of a Geant4Action object
      /**
       * @author  M.Frank
       * @version 1.0
       */
      class ContextUpdate {
      public:
        /// reference to the context;
        const Geant4Context* context;
        /// Constructor
        ContextUpdate(const Geant4Context* c = 0) : context(c) {}
        /// Callback
        void operator()(Geant4Action* action) const;
      };
      friend class ContextUpdate;

    protected:
      /// Actor class to manipulate action groups
      /**
       * @author  M.Frank
       * @version 1.0
       */
      template <typename T> struct Actors {
        typedef typename std::vector<T*> _V;
        _V                               m_v;
        Actors() {}
        ~Actors() {}
        void clear() { m_v.clear(); }
        void add(T* obj) { m_v.push_back(obj); }
        operator const _V&() const { return m_v; }
        operator _V&() { return m_v; }
        const _V* operator->() const { return &m_v; }
        _V* operator->() { return &m_v; }
        typename _V::iterator       begin() { return m_v.begin(); }
        typename _V::iterator       end() { return m_v.end(); }
        typename _V::const_iterator begin() const { return m_v.begin(); }
        typename _V::const_iterator end() const { return m_v.end(); }
        /// Context updates
        void operator()(const ContextUpdate& context) {
          if (m_v.empty())
            return;
          for (typename _V::iterator i = m_v.begin(); i != m_v.end(); ++i)
            context(*i);
        }
        /// NON-CONST actions
        template <typename R, typename Q> void operator()(R (Q::*pmf)()) {
          if (m_v.empty())
            return;
          for (typename _V::iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)();
        }
        template <typename R, typename Q, typename A0> void operator()(R (Q::*pmf)(A0), A0 a0) {
          if (m_v.empty())
            return;
          for (typename _V::iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)(a0);
        }
        template <typename R, typename Q, typename A0, typename A1> void operator()(R (Q::*pmf)(A0, A1), A0 a0, A1 a1) {
          if (m_v.empty())
            return;
          for (typename _V::iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)(a0, a1);
        }
        /// CONST actions
        template <typename R, typename Q> void operator()(R (Q::*pmf)() const) const {
          if (m_v.empty())
            return;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)();
        }
        template <typename R, typename Q, typename A0> void operator()(R (Q::*pmf)(A0) const, A0 a0) const {
          if (m_v.empty())
            return;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)(a0);
        }
        template <typename R, typename Q, typename A0, typename A1>
        void operator()(R (Q::*pmf)(A0, A1) const, A0 a0, A1 a1) const {
          if (m_v.empty())
            return;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)(a0, a1);
        }
        /// CONST filters
        template <typename Q> bool filter(bool (Q::*pmf)() const) const {
          if (!m_v.empty())
            return true;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            if (!((*i)->*pmf)())
              return false;
          return true;
        }
        template <typename Q, typename A0> bool filter(bool (Q::*pmf)(A0) const, A0 a0) const {
          if (m_v.empty())
            return true;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            if (!((*i)->*pmf)(a0))
              return false;
          return true;
        }
        template <typename Q, typename A0, typename A1> bool filter(bool (Q::*pmf)(A0, A1) const, A0 a0, A1 a1) const {
          if (m_v.empty())
            return true;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            if (!((*i)->*pmf)(a0, a1))
              return false;
          return true;
        }
      };

      /// Default destructor
      virtual ~Geant4Action();

    public:
      /// Standard constructor
      Geant4Action(Geant4Context* context, const std::string& nam);
      /// Increase reference count
      long addRef();
      /// Decrease reference count. Implicit destruction
      long release();
      /// Access the context
      const Geant4Context* context() const { return &m_context; }
      /// Access name of the action
      const std::string& name() const { return m_name; }
      /// Access name of the action
      const char* c_name() const { return m_name.c_str(); }
      /// Set the object name.
      void setName(const std::string& new_name) { m_name = new_name; }
      /// Access to the properties of the object
      PropertyManager& properties() { return m_properties; }
      /// Access the output level
      PrintLevel outputLevel() const { return (PrintLevel)m_outputLevel; }
      /// Set the output level; returns previous value
      PrintLevel setOutputLevel(PrintLevel new_level);
      /// Access to the UI messenger
      Geant4UIMessenger* control() const;
      /// Enable and install UI messenger
      virtual void enableUI();
      /// Declare property
      template <typename T> Geant4Action& declareProperty(const std::string& nam, T& val);
      /// Declare property
      template <typename T> Geant4Action& declareProperty(const char* nam, T& val);
      /// Check property for existence
      bool hasProperty(const std::string& name) const;
      /// Access single property
      Property& property(const std::string& name);
      /// Set object properties
      Geant4Action& setProperties(PropertyConfigurator& setup);

      /// Install property control messenger if wanted
      virtual void installMessengers();
      /// Install command control messenger if wanted
      virtual void installCommandMessenger();
      /// Install property control messenger if wanted
      void installPropertyMessenger();

      /// Support for messages with variable output level using output level
      void print(const std::string& fmt, ...) const;
      /// Support for messages with variable output level using output level
      void print(const std::string& tag, const std::string& fmt, ...) const;

      /// Support of debug messages.
      void debug(const std::string& fmt, ...) const;
      /// Support of warning messages.
      void debug(const std::string& tag, const std::string& fmt, ...) const;
      /// Support of info messages.
      void info(const std::string& fmt, ...) const;
      /// Support of warning messages.
      void info(const std::string& tag, const std::string& fmt, ...) const;
      /// Support of warning messages.
      void warning(const std::string& fmt, ...) const;
      /// Support of warning messages.
      void warning(const std::string& tag, const std::string& fmt, ...) const;
      /// Support of error messages.
      void error(const std::string& fmt, ...) const;
      /// Support of error messages.
      void error(const std::string& tag, const std::string& fmt, ...) const;
      /// Action to support error messages.
      bool error(bool return_value, const std::string& fmt, ...) const;
      /// Support of fatal messages. Throws exception
      void fatal(const std::string& fmt, ...) const;
      /// Support of warning messages.
      void fatal(const std::string& tag, const std::string& fmt, ...) const;
      /// Support of exceptions: Print fatal message and throw runtime_error.
      void except(const std::string& fmt, ...) const;
      /// Support of warning messages.
      void except(const std::string& tag, const std::string& fmt, ...) const;

      /// Abort Geant4 Run by throwing a G4Exception with type RunMustBeAborted
      void abortRun(const std::string& exception, const std::string& fmt, ...) const;

      /// Access to the main run action sequence from the kernel object
      Geant4RunActionSequence& runAction() const;
      /// Access to the main event action sequence from the kernel object
      Geant4EventActionSequence& eventAction() const;
      /// Access to the main stepping action sequence from the kernel object
      Geant4SteppingActionSequence& steppingAction() const;
      /// Access to the main tracking action sequence from the kernel object
      Geant4TrackingActionSequence& trackingAction() const;
      /// Access to the main stacking action sequence from the kernel object
      Geant4StackingActionSequence& stackingAction() const;
      /// Access to the main generator action sequence from the kernel object
      Geant4GeneratorActionSequence& generatorAction() const;
    };

    /// Declare property
    template <typename T> Geant4Action& Geant4Action::declareProperty(const std::string& nam, T& val) {
      m_properties.add(nam, val);
      return *this;
    }

    /// Declare property
    template <typename T> Geant4Action& Geant4Action::declareProperty(const char* nam, T& val) {
      m_properties.add(nam, val);
      return *this;
    }
  }  // End namespace Simulation
}  // End namespace DD4hep

#endif  // DD4HEP_DDG4_GEANT4ACTION_H
