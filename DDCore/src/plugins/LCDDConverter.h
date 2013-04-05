// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_LCDDCONVERTER_H
#define DD4HEP_GEOMETRY_LCDDCONVERTER_H

// Framework include files
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/GeoHandler.h"
#include "DD4hep/LCDD.h"

// C/C++ include files
#include <map>
#include <set>
#include <vector>

// Forward declarations
class TGeoVolume;
class TGeoElement;
class TGeoShape;
class TGeoMedium;
class TGeoNode;
class TGeoMatrix;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geomentry namespace declaration
   */
  namespace Geometry {

    /** @class LCDDConverter LCDDConverter.h XML/LCDDConverter.h
     * 
     * Geometry converter from DD4hep to Geant 4.
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct LCDDConverter : public GeoHandler {
      typedef XML::XmlElement XmlElement;
      typedef std::map<const TGeoElement*, XmlElement*> ElementMap;
      typedef std::map<const TGeoMedium*, XmlElement*>  MaterialMap;
      typedef std::map<const TNamed*, XmlElement*>      LimitMap;
      typedef std::map<const TGeoNode*, XmlElement*>    PlacementMap;
      typedef std::map<const TNamed*, XmlElement*>      RegionMap;
      typedef std::map<const TNamed*, XmlElement*>      SensDetMap;
      typedef std::map<const TGeoVolume*, XmlElement*>  VolumeMap;
      typedef std::map<const TGeoShape*, XmlElement*>   SolidMap;
      typedef std::map<const TNamed*, XmlElement*>      VisMap;
      typedef std::map<const TNamed*, XmlElement*>      FieldMap;
      typedef std::map<const TNamed*, XmlElement*>      IdSpecMap;
      typedef std::map<const TGeoMatrix*, XmlElement*>  TrafoMap;
      struct GeometryInfo : public GeoHandler::GeometryInfo {
        ElementMap   xmlElements;
        MaterialMap  xmlMaterials;
        SolidMap     xmlSolids;
        VolumeMap    xmlVolumes;
        PlacementMap xmlPlacements;
        RegionMap    xmlRegions;
        VisMap       xmlVis;
        LimitMap     xmlLimits;
        IdSpecMap    xmlIdSpecs;
        SensDetMap   xmlSensDets;
        TrafoMap     xmlPositions;
        TrafoMap     xmlRotations;
        FieldMap     xmlFields;
        ObjectSet    sensitives;
        ObjectSet    regions;
        ObjectSet    limits;
        // These we need for redundancy and checking the data integrity
        typedef std::map<std::string, const TNamed*> CheckIter;
        struct _checks {
          std::map<std::string, const TNamed *> positions, rotations, volumes, solids, materials;
        };
        mutable _checks checks;
        void check(const std::string& name, const TNamed* n, std::map<std::string, const TNamed*>& array) const;
        void checkPosition(const std::string& name, const TNamed* n) const { check(name, n, checks.positions); }
        void checkRotation(const std::string& name, const TNamed* n) const { check(name, n, checks.rotations); }
        void checkVolume(const std::string& name, const TNamed* n) const { check(name, n, checks.volumes); }
        void checkShape(const std::string& name, const TNamed* n) const { check(name, n, checks.solids); }
        void checkMaterial(const std::string& name, const TNamed* n) const { check(name, n, checks.materials); }

        xml_doc_t doc;
        xml_h     identity_rot, identity_pos;
        xml_elt_t doc_root, doc_header, doc_idDict, doc_detectors, doc_limits, doc_regions, doc_display, doc_gdml,
            doc_fields, doc_define, doc_materials, doc_solids, doc_structure, doc_setup;
        GeometryInfo();
      };
      /// Reference to detector description
      LCDD& m_lcdd;
      /// Processing flag
      bool m_checkOverlaps;

      typedef std::set<std::string> NameSet;
      mutable NameSet               m_checkNames;

      GeometryInfo* m_dataPtr;
      GeometryInfo& data() const { return *m_dataPtr; }

      /// Data integrity checker
      void checkVolumes(const std::string& name, const TGeoVolume* volume) const;

      /// Initializing Constructor
      LCDDConverter(LCDD& lcdd);

      /// Standard destructor
      virtual ~LCDDConverter();

      /// Create geometry conversion in GDML format
      xml_doc_t createGDML(DetElement top);

      /// Create geometry conversion in LCDD format
      xml_doc_t createLCDD(DetElement top);

      /// Create geometry conversion in Vis format
      xml_doc_t createVis(DetElement top);

      /// Add header information in LCDD format
      virtual void handleHeader() const;

      /// Convert the geometry type material into the corresponding Xml object(s).
      virtual xml_h handleMaterial(const std::string& name, const TGeoMedium* medium) const;

      /// Convert the geometry type element into the corresponding Xml object(s).
      virtual xml_h handleElement(const std::string& name, const TGeoElement* element) const;

      /// Convert the geometry type solid into the corresponding Xml object(s).
      virtual xml_h handleSolid(const std::string& name, const TGeoShape* volume) const;

      /// Convert the geometry type logical volume into the corresponding Xml object(s).
      virtual xml_h handleVolume(const std::string& name, const TGeoVolume* volume) const;
      virtual xml_h handleVolumeVis(const std::string& name, const TGeoVolume* volume) const;
      virtual void collectVolume(const std::string& name, const TGeoVolume* volume) const;

      /// Convert the geometry type volume placement into the corresponding Xml object(s).
      virtual xml_h handlePlacement(const std::string& name, const TGeoNode* node) const;

      /// Convert the geometry type field into the corresponding Xml object(s).
      ///virtual xml_h handleField(const std::string& name, Ref_t field) const;

      /// Convert the geometry type region into the corresponding Xml object(s).
      virtual xml_h handleRegion(const std::string& name, const TNamed* region) const;

      /// Convert the geometry visualisation attributes to the corresponding Xml object(s).
      virtual xml_h handleVis(const std::string& name, const TNamed* vis) const;

      /// Convert the geometry id dictionary entry to the corresponding Xml object(s).
      virtual xml_h handleIdSpec(const std::string& name, const TNamed* vis) const;

      /// Convert the geometry type LimitSet into the corresponding Xml object(s).
      virtual xml_h handleLimitSet(const std::string& name, const TNamed* limitset) const;

      /// Convert the geometry type SensitiveDetector into the corresponding Xml object(s).
      virtual xml_h handleSensitive(const std::string& name, const TNamed* sens_det) const;

      /// Convert the Position into the corresponding Xml object(s).
      virtual xml_h handlePosition(const std::string& name, const TGeoMatrix* trafo) const;

      /// Convert the Rotation into the corresponding Xml object(s).
      virtual xml_h handleRotation(const std::string& name, const TGeoMatrix* trafo) const;

      /// Convert the electric or magnetic fields into the corresponding Xml object(s).
      virtual xml_h handleField(const std::string& name, const TNamed* field) const;

      /// Handle the geant 4 specific properties
      void handleProperties(LCDD::Properties& prp) const;
    };
  }  // End namespace XML
}  // End namespace DD4hep

#endif  // DD4HEP_GEOMETRY_LCDDCONVERTER_H
