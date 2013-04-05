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

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    /** @class Readout  Readout.h DD4hep/lcdd/Readout.h
      *
      * @author  M.Frank
      * @version 1.0
      */
    struct Readout : public Ref_t {
    public:
      struct Object {
        Segmentation segmentation;
        Volume       readoutWorld;
        IDDescriptor id;
      };

    protected:
      /// Additional data accessor
      Object& _data() const { return *data<Object>(); }

    public:
      /// Default constructor
      Readout() : Ref_t() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Readout(const Handle<Q>& e) : Ref_t(e) {}
      /// Initializing constructor
      Readout(const std::string& name);
      /// Assign IDDescription to readout structure
      void setIDDescriptor(const Ref_t& spec) const;
      /// Access IDDescription structure
      IDDescriptor idSpec() const;
      /// Assign segmentation structure to readout
      void setSegmentation(const Segmentation& segment) const;
      /// Access segmentation structure
      Segmentation segmentation() const;
    };

    /** @class Alignment  Readoutn.h DD4hep/lcdd/Readout.h
      *
      * @author  M.Frank
      * @version 1.0
      */
    struct Alignment : public Ref_t {
      struct Object {
        Volume volume;
      };
      /// Default constructor
      Alignment() : Ref_t() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Alignment(const Handle<Q>& e) : Ref_t(e) {}
      /// Initializing constructor
      Alignment(const LCDD& doc, const std::string& name);
      /// Additional data accessor
      Object& _data() const { return *data<Object>(); }
    };

    /** @class Conditions  Readout.h DD4hep/lcdd/Readout.h
      *
      * @author  M.Frank
      * @version 1.0
      */
    struct Conditions : public Ref_t {
      struct Object {};
      /// Default constructor
      Conditions() : Ref_t() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Conditions(const Handle<Q>& e) : Ref_t(e) {}
      /// Initializing constructor
      Conditions(const LCDD& doc, const std::string& name);
      /// Additional data accessor
      Object& _data() const { return *data<Object>(); }
    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                */
#endif /* DD4hep_GEOMETRY_READOUT_H           */
