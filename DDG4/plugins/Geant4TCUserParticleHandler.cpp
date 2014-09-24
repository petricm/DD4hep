// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4TCUSERPARTICLEHANDLER_H
#define DD4HEP_DDG4_GEANT4TCUSERPARTICLEHANDLER_H

// Framework include files
#include "DDG4/Geant4UserParticleHandler.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** Geant4TCUserParticleHandler
     *
     *  Rejects to keep particles, which are created outside a tracking cylinder.
     *
     *  TC stands for TrackingCylinder ;-)
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TCUserParticleHandler : public Geant4UserParticleHandler  {
      double m_zTracker, m_rTracker;
    public:
      /// Standard constructor
      Geant4TCUserParticleHandler(Geant4Context* context, const std::string& nam);

      /// Default destructor
      virtual ~Geant4TCUserParticleHandler() {}

      /// Post-track action callback
      /** Allow the user to force the particle handling in the post track action
       *  set the reason mask to NULL in order to drop the particle.
       *  The parent's reasoning mask will be or'ed with the particle's mask
       *  to preserve the MC truth for the hit creation.
       *  The default implementation is empty.
       *
       *  Note: The particle passed is a temporary and will be copied if kept.
       */
      virtual void end(const G4Track* track, Particle& particle);

    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4TCUSERPARTICLEHANDLER_H

// $Id: Geant4Field.cpp 888 2013-11-14 15:54:56Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
//#include "DDG4/Geant4TCUserParticleHandler.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Factories.h"

using namespace DD4hep::Simulation;
DECLARE_GEANT4ACTION(Geant4TCUserParticleHandler)

/// Standard constructor
Geant4TCUserParticleHandler::Geant4TCUserParticleHandler(Geant4Context* context, const std::string& nam)
: Geant4UserParticleHandler(context,nam) 
{
  declareProperty("TrackingVolume_Zmax",m_zTracker=1e100);
  declareProperty("TrackingVolume_Rmax",m_rTracker=1e100);
}

/// Post-track action callback
void Geant4TCUserParticleHandler::end(const G4Track* /* track */, Particle& p)  {
  double r_end  = sqrt(p.vex*p.vex + p.vey*p.vey);
  double z_end  = fabs(p.vez);
  // Keep particles with end-vertex inside tracking (including back-scattered particles)
  if ( r_end <= m_rTracker && z_end <= m_zTracker )  {
    return;
  }
  double r_prod = sqrt(p.vsx*p.vsx + p.vsy*p.vsy);
  double z_prod = fabs(p.vsz);
  // Otherwise: reject particles created outside the tracking volume.
  if ( r_prod > m_rTracker || z_prod > m_zTracker )  {
    p.reason = 0;
  }
}
