//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_Geant4GDMLDetector_H
#define DD4HEP_Geant4GDMLDetector_H

#include <string>
#include "G4VUserDetectorConstruction.hh"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Class to read Geant4 geometry from GDML file
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4GDMLDetector : public G4VUserDetectorConstruction {
    public:
      /// Constructor to open gdml file
      Geant4GDMLDetector(const std::string& gdmlFile);
      /// Default destructor
      virtual ~Geant4GDMLDetector() {}
      G4VPhysicalVolume* Construct();

    private:
      std::string        m_fileName;
      G4VPhysicalVolume* m_world;
    };
  }
}

#endif
