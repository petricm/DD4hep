// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_SEGMENTATIONS_H
#define DD4HEP_GEOMETRY_SEGMENTATIONS_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/Objects.h"
#include "DDSegmentation/Segmentation.h"
#include "DDSegmentation/SegmentationFactory.h"

// C/C++ include files
#include <cmath>
#include <vector>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   XML namespace declaration
   */
  namespace Geometry {

    typedef DDSegmentation::BitField64 BitField64;

    /** @class Segmentation Segmentations.h DD4hep/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Segmentation : public Handle<DDSegmentation::Segmentation> {
    public:
      typedef DDSegmentation::Segmentation BaseSegmentation;
      typedef DDSegmentation::Parameter    Parameter;
      typedef DDSegmentation::Parameters   Parameters;

      /** @class Segmentation::Object Segmentations.h DD4hep/Segmentations.h
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct Object : public BaseSegmentation {
        /// Magic word to check object integrity
        unsigned long magic;
        /// Flag to use segmentation for hit positioning
        unsigned char useForHitPosition;
        /// determine the local position based on the cell ID
        DDSegmentation::Position position(const long64& cellID) const;
        /// determine the cell ID based on the local position
        long64 cellID(const DDSegmentation::Position& localPosition, const DDSegmentation::Position& globalPosition,
                      const long64& volumeID) const;
        /// Standard constructor
        Object(BaseSegmentation* s = 0);
        /// Default destructor
        virtual ~Object();
        /// Access the encoding string
        std::string fieldDescription() const;
        /// Access the segmentation name
        const std::string& name() const;
        /// Set the segmentation name
        void setName(const std::string& value);
        /// Access the segmentation type
        const std::string& type() const;
        /// Access the description of the segmentation
        const std::string& description() const;
        /// Access the underlying decoder
        BitField64* decoder();
        /// Set the underlying decoder
        void setDecoder(BitField64* decoder);
        /// Access to parameter by name
        Parameter parameter(const std::string& parameterName) const;
        /// Access to all parameters
        Parameters parameters() const;
        /// Set all parameters from an existing set of parameters
        void setParameters(const Parameters& parameters);
        /// Reference to base segmentation
        BaseSegmentation* segmentation;
      };

    public:
      /// Default constructor
      Segmentation() : Handle<Implementation>() {}
      /// Initializing constructor creating a new object of the given DDSegmentation type
      Segmentation(const std::string& type, const std::string& name);
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Segmentation(const Handle<Q>& e) : Handle<Implementation>(e) {}
      /// Access flag for hit positioning
      bool useForHitPosition() const;
      /// Accessor: Segmentation type
      std::string type() const;
      /// Accessor: Set segmentation type
      void setType(const std::string& new_type);
      /// Access segmentation object
      BaseSegmentation* segmentation() const;
      /// Access to the parameters
      Parameters parameters() const;
      /// determine the local position based on the cell ID
      Position position(const long64& cellID) const;
      /// determine the cell ID based on the local position
      long64 cellID(const Position& localPosition, const Position& globalPosition, const long64& volumeID) const;
    };

  } /* End namespace Geometry              */
} /* End namespace DD4hep                */
#endif /* DD4HEP_GEOMETRY_SEGMENTATIONS_H     */
