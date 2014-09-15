// $Id: Geant4Data.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4VERTEX_H
#define DD4HEP_GEANT4VERTEX_H

// C/C++ include files
#include <memory>
#include <set>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations
    class Geant4Vertex;

    class VertexExtension {
    public:
      /// Default constructor
      VertexExtension() {}
      /// Default destructor
      virtual ~VertexExtension();
    };

    /// Data structure to store the MC vertex information
    /**
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4Vertex {
    public:
      typedef std::set<int> Particles;
      /// Reference counter
      int ref;  //! not persistent
      /// Vertex data
      double x, y, z, time;
      /// The list of outgoing particles
      Particles out;
      /// The list of incoming particles
      Particles in;
      /// User data extension if required
      std::auto_ptr<VertexExtension> extension;

      /// Default constructor
      Geant4Vertex();
      /// Copy constructor
      Geant4Vertex(const Geant4Vertex& c);
      /// Default destructor
      virtual ~Geant4Vertex();
      /// Assignment operator
      Geant4Vertex& operator=(const Geant4Vertex& c);
      /// Increase reference count
      Geant4Vertex* addRef();
      /// Decrease reference count. Deletes object if NULL
      void release();
    };

  }  // End namespace Simulation
}  // End namespace DD4hep
#endif  // DD4HEP_GEANT4VERTEX_H
