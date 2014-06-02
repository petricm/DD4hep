// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_IDDESCRIPTOR_H
#define DD4hep_IDDESCRIPTOR_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Primitives.h"
#include "DDSegmentation/BitField64.h"

// C++ include files
#include <map>
#include <string>
#include <vector>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry sub-namespace declaration
   */
  namespace Geometry {

    class IDDescriptorObject;

    /** @class IDDescriptor IDDescriptor.h DDCore/IDDescriptor.h
     *
     *  @author  M.Frank
     *  @version 1.0
     *  @date    2012/07/31
     */
    class IDDescriptor : public Handle<IDDescriptorObject> {
    public:
      typedef IDDescriptorObject             Object;
      typedef DDSegmentation::BitFieldValue* Field;
      typedef std::vector<std::pair<std::string, Field>>  FieldMap;
      typedef std::vector<std::pair<size_t, std::string>> FieldIDs;
      typedef std::pair<Field, VolumeID>                  VolIDField;
      typedef std::vector<VolIDField> VolIDFields;

    public:
      /// Default constructor
      IDDescriptor() : Handle<Object>() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> IDDescriptor(const Handle<Q>& e) : Handle<Object>(e) {}
      /// Initializing constructor
      IDDescriptor(const std::string& description);
      // the string description of all fields
      std::string fieldDescription() const;
      /// The total number of encoding bits for this descriptor
      unsigned maxBit() const;
      /// Access the field-id container
      const FieldIDs& ids() const;
      /// Access the fieldmap container
      const FieldMap& fields() const;
      /// Get the field descriptor of one field by name
      Field field(const std::string& field_name) const;
      /// Get the field identifier of one field by name
      size_t fieldID(const std::string& field_name) const;
      /// Get the field descriptor of one field by its identifier
      Field field(size_t identifier) const;
#ifndef __MAKECINT__
      /// Encode a set of volume identifiers (corresponding to this description of course!) to a volumeID.
      typedef std::pair<std::string, int> VolID;
      VolumeID encode(const std::vector<VolID>& ids) const;
#endif
      /// Decode volume IDs and return filled descriptor with all fields
      void decodeFields(VolumeID vid, VolIDFields& fields);
      /// Access string representation
      std::string toString() const;
      /// Access the BitField64 object
      DDSegmentation::BitField64* decoder();
    };
  } /* End namespace Geometry    */
} /* End namespace DD4hep      */
#endif /* DD4hep_IDDESCRIPTOR_H     */
