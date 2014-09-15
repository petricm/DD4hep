// $Id: Geant4Data.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4RANDOM_H
#define DD4HEP_GEANT4RANDOM_H

// Framework include files

// C/C++ include files

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations
    class Geant4Exec;

    /// Mini interface to THE random generator of the application
    /**
     *  Mini interface to THE random generator of the application.
     *  Necessary, that on every object creates its own instance, but accesses
     *  the main instance avaible throu the Geant4Context.
     *
     *  This is mandatory to ensure reproducability of the event generation
     *  process. Particular objects may use a dependent generator from 
     *  an experiment framework like GAUDI.
     *
     *  This main interface is supposed to be stable. Unclear however is 
     *  if the generation functions will have to become virtual....
     *  Future will tell us.
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class Geant4Random {
      friend class Geant4Exec;

    protected:
      /// Default constructor
      Geant4Random();
      /// Default destructor
      virtual ~Geant4Random();

    public:
      void circle(double& x, double& y, double r);
      double exp(double tau);
      double gauss(double mean = 0, double sigma = 1);
      double landau(double mean = 0, double sigma = 1);
      double rndm(int i = 0);
      void rndmArray(int n, float* array);
      void rndmArray(int n, double* array);
      void sphere(double& x, double& y, double& z, double r);
      double uniform(double x1 = 1);
      double uniform(double x1, double x2);
    };

  }  // End namespace Simulation
}  // End namespace DD4hep
#endif  // DD4HEP_GEANT4RANDOM_H
