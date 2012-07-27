// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_LCDD_DETECTOR_H
#define DD4hep_LCDD_DETECTOR_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Readout.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/Volumes.h"

// C/C++ include files
#include <map>
#include <typeinfo>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    struct SensitiveDetector;
    struct Detector;
    struct LCDD;

    /** @class SensitiveDetector Detector.h DD4hep/lcdd/Detector.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct SensitiveDetector : public Ref_t {
      struct Object {
        unsigned int magic;
        int          verbose;
        int          combine_hits;
        double       ecut;
        std::string  eunit;
        std::string  hits_collection;
        Readout      readout;
        Object() : magic(magic_word()), verbose(0), readout() {}
      };

      /// Default constructor
      SensitiveDetector() : Ref_t() {}
      /// Templated constructor for handle conversions
      template <typename Q> SensitiveDetector(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor for a new sensitive detector element
      SensitiveDetector(const std::string& type, const std::string& name);

      /// Additional data accessor
      Object& _data() const { return *data<Object>(); }
      /// Access the type of the sensitive detector
      std::string type() const;
      /// Set flag to handle hits collection
      SensitiveDetector& setCombineHits(bool value);
      /// Assign the name of the hits collection
      SensitiveDetector& setHitsCollection(const std::string& spec);
      /// Assign the IDDescriptor reference
      SensitiveDetector& setReadout(Readout readout);
      /// Access readout structure of the sensitive detector
      Readout readout() const;
    };

    /** @class SubDetector Detector.h DD4hep/lcdd/Detector.h
     *
     *  Please note: 
     *  Though nowhere enforced, it is obvious that only placements
     *  of the **SAME** logical volume should be added.
     *
     *  We explicitly rely here on the common sense of the user
     *  of this class. People without a brain can always screw 
     *  things completely - nothing one can do about!
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct DetElement : public Ref_t {
      typedef Ref_t Parent;
      typedef std::map<std::string, DetElement> Children;
      typedef std::vector<PlacedVolume> Placements;
      typedef std::map<const std::type_info*, void*> Extensions;

      enum { COPY_NONE = 0, COPY_PLACEMENT = 1 << 0, COPY_PARENT = 1 << 1, COPY_ALIGNMENT = 1 << 2, LAST } CopyParameters;
      struct Object {
        unsigned int magic;
        int          id;
        std::string  path;
        int          combine_hits;
        Volume       volume;
        Readout      readout;
        Alignment    alignment;
        Conditions   conditions;
        PlacedVolume placement;
        Placements   placements;
        Parent       parent;
        Parent       reference;
        Children     children;
        Extensions   extensions;
        TGeoMatrix*  worldTrafo;
        TGeoMatrix*  parentTrafo;
        TGeoMatrix*  referenceTrafo;
        std::string  placementPath;
        /// Default constructor
        Object();
        /// Deep object copy to replicate DetElement trees e.g. for reflection
        virtual Value<TNamed, Object>* clone(int new_id, int flag) const;
        /// Conversion to reference object
        operator Ref_t();
        /// Conversion to reference object
        Ref_t asRef();
        /// Create cached matrix to transform to world coordinates
        TGeoMatrix* worldTransformation();
        /// Create cached matrix to transform to parent coordinates
        TGeoMatrix* parentTransformation();
        /// Create cached matrix to transform to reference coordinates
        TGeoMatrix* referenceTransformation();
      };

      /// Additional data accessor
      Object& _data() const { return *data<Object>(); }

      void check(bool condition, const std::string& msg) const;

      /// Templated default constructor
      template <typename T> static void* _construct() { return new T(); }
      /// Templated copy constructor
      template <typename T> static void* _copy(const void* ptr, DetElement elt) {
        return new T(*(dynamic_cast<const T*>((T*)ptr)), elt);
      }
      /// Templated destructor function
      template <typename T> static void _delete(void* ptr) { delete (T*)(ptr); }

      /// Add an extension object to the detector element
      void* i_addExtension(void* ptr, const std::type_info& info, void* (*copy)(const void*, DetElement),
                           void (*destruct)(void*));
      /// Access an existing extension object from the detector element
      void* i_extension(const std::type_info& info) const;

      /// Default constructor
      DetElement() : Ref_t() {}

      /// Default constructor
      template <typename Q> DetElement(Q* data, const std::string& name, const std::string& type) : Ref_t(data) {
        this->assign(data, name, type);
      }

      /// Templated constructor for handle conversions
      template <typename Q> DetElement(const Handle<Q>& e) : Ref_t(e) {}

      /// Constructor to copy handle
      DetElement(const DetElement& e) : Ref_t(e) {}

      /// Constructor for a new subdetector element
      DetElement(const std::string& name, const std::string& type, int id);

      /// Constructor for a new subdetector element
      DetElement(const std::string& name, int id);

      /// Constructor for a new subdetector element
      DetElement(DetElement parent, const std::string& name, int id);

      /// Clone (Deep copy) the DetElement structure with a new name
      DetElement clone(const std::string& new_name) const;

      /// Clone (Deep copy) the DetElement structure with a new name and new identifier
      DetElement clone(const std::string& new_name, int new_id) const;

      template <typename IFACE, typename CONCRETE> IFACE* addExtension(CONCRETE* c) {
        return (IFACE*)i_addExtension(dynamic_cast<IFACE*>(c), typeid(IFACE), _copy<CONCRETE>, _delete<IFACE>);
      }

      template <class T> T* extension() const { return (T*)i_extension(typeid(T)); }

      DetElement& setCombineHits(bool value, SensitiveDetector& sens);
      int         id() const;
      std::string type() const;
      std::string path() const;
      /// Access to the full path to the placed object
      std::string placementPath() const;
      bool        isTracker() const;
      bool        isCalorimeter() const;
      bool        combineHits() const;

      /// Set all attributes in one go
      DetElement& setAttributes(const LCDD& lcdd, const Volume& volume, const std::string& region, const std::string& limits,
                                const std::string& vis);

      /// Set Visualization attributes to the detector element
      DetElement& setVisAttributes(const LCDD& lcdd, const std::string& name, const Volume& volume);
      /// Set the regional attributes to the detector element
      DetElement& setRegion(const LCDD& lcdd, const std::string& name, const Volume& volume);
      /// Set the limits to the detector element
      DetElement& setLimitSet(const LCDD& lcdd, const std::string& name, const Volume& volume);

      /// Access the readout structure
      Readout readout() const;
      /// Assign readout definition
      DetElement& setReadout(const Readout& readout);

      /// Access to the logical volume of the placements (all daughters have the same!)
      Volume volume() const;
      /// Set the logical volume of the placements (all daughters have the same!)
      //void            setVolume(Volume vol);

      /// Access to the physical volume of this detector element
      PlacedVolume placement() const;
      /// Set the physical volumes of the detector element
      DetElement& setPlacement(const PlacedVolume& volume);

      /// Add new child to the detector structure
      DetElement& add(DetElement sub_element);
      /// Access to the list of children
      const Children& children() const;
      /// Access to individual children by name
      DetElement child(const std::string& name) const;
      /// Access to the detector elements's parent
      DetElement parent() const;

      /// Set detector element for reference transformations. Will delete existing reference trafo.
      DetElement& setReference(DetElement reference);

      /// Transformation from local coordinates of the placed volume to the world system
      bool localToWorld(const Position& local, Position& global) const;
      /// Transformation from local coordinates of the placed volume to the parent system
      bool localToParent(const Position& local, Position& parent) const;
      /// Transformation from local coordinates of the placed volume to arbitrary parent system set as reference
      bool localToReference(const Position& local, Position& reference) const;

      /// Transformation from world coordinates of the local placed volume coordinates
      bool worldToLocal(const Position& global, Position& local) const;
      /// Transformation from world coordinates of the local placed volume coordinates
      bool parentToLocal(const Position& parent, Position& local) const;
      /// Transformation from world coordinates of the local placed volume coordinates
      bool referenceToLocal(const Position& reference, Position& local) const;
    };

  } /* End namespace Geometry      */
} /* End namespace DD4hep       */
#endif /* DD4hep_LCDD_DETECTOR_H     */
