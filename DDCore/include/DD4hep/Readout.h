// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_GEOMETRY_READOUT_H
#define DD4hep_GEOMETRY_READOUT_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/Volumes.h"

// C/C++ include files
#include <map>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    // Forward declarations
    class DetElement;
    class ReadoutObject;

    /// Handle to the implementation of the readout structure of a subdetector
    /** @class Readout  Readout.h DD4hep/Readout.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Readout : public Handle<ReadoutObject> {
    public:
      /// Implementation type
      typedef ReadoutObject Object;

    public:
      /// Default constructor
      Readout() : Handle<Object>() {}
      /// Copy Constructor from object
      Readout(const Readout& e) : Handle<Object>(e) {}
#ifndef __CINT__
      /// Copy Constructor from handle
      Readout(const Handle<ReadoutObject>& e) : Handle<Object>(e) {}
#endif
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Readout(const Handle<Q>& e) : Handle<Object>(e) {}
      /// Initializing constructor
      Readout(const std::string& name);
      /// Assignment operator
      Readout& operator=(const Readout& ro) {
        if (&ro == this)
          return *this;
        m_element = ro.m_element;
        return *this;
      }
      /// Assign IDDescription to readout structure
      void setIDDescriptor(const Ref_t& spec) const;
      /// Access IDDescription structure
      IDDescriptor idSpec() const;
      /// Assign segmentation structure to readout
      void setSegmentation(const Segmentation& segment) const;
      /// Access segmentation structure
      Segmentation segmentation() const;
    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */
#endif /* DD4hep_GEOMETRY_READOUT_H      */
