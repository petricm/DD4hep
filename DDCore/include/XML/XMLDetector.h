// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_XMLDETECTOR_H
#define DD4HEP_XMLDETECTOR_H

// Framework include files
#include "XML/XMLElements.h"
#include "XML/XMLTags.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
  *   XML namespace declaration
  */
  namespace XML {

    struct Dimension : public Element {
      Dimension() : Element(Handle_t(0)) {}
      Dimension(Handle_t e) : Element(e) {}
      Dimension(const Element& e) : Element(e) {}
      // Rotation:
      double theta() const;
      double phi() const;
      double psi() const;

      // Box/Position:
      double x() const;
      double y() const;
      double z() const;

      // Functions return defaults rather than throwing exceptions
      double x(double default_val) const;
      double y(double default_val) const;
      double z(double default_val) const;

      // Trapezoid:
      double x1() const;
      double x2() const;
      double y1() const;
      double y2() const;
      // Tube
      double rmin() const;
      double rmax() const;
      double zhalf() const;
      double deltaphi() const;

      double outer_radius() const;
      double outer_r() const;
      double outer_z() const;
      double inner_radius() const;
      double inner_r() const;
      double inner_z() const;

      double z_length() const;
      double length() const;
      double width() const;
      double gap() const;
      double r_size() const;
      double phi_size_max() const;
      bool   reflect() const;
      bool reflect(bool default_value) const;
      double outgoing_r() const;
      double incoming_r() const;
      double crossing_angle() const;

      double zmin() const;
      double zmax() const;

      double radius() const;
      double offset() const;
      double offset(double default_value) const;
      int number() const;

      int         nModules() const;
      int         RowID() const;
      int         nPads() const;
      double      moduleHeight() const;
      double      moduleWidth() const;
      double      modulePitch() const;
      double      modulePosX() const;
      double      modulePosY() const;
      double      rowPitch() const;
      double      padPitch() const;
      double      rowHeight() const;
      std::string padType() const;

      int numsides() const;

      double phi0() const;
      double phi0(double default_value) const;
      double phi_tilt() const;
      int    nphi() const;
      double rc() const;
      double dr() const;
      double dz() const;
      double dz(double default_value) const;
      double r() const;
      double r(double default_value) const;

      double z0() const;
      double zstart() const;
      int    nz() const;

      double start() const;
      double end() const;
    };

    struct DetElement : public Dimension {
      struct Component : public Dimension {
        Component(Handle_t e) : Dimension(e) {}
        Component(const Element& e) : Dimension(e) {}
        int                      id() const;
        int id(int default_value) const;
        int         repeat() const;
        double      thickness() const;
        bool        isSensitive() const;
        std::string nameStr() const;
        std::string typeStr() const;
        std::string moduleStr() const;
        std::string materialStr() const;
        std::string visStr() const;
        std::string regionStr() const;
        std::string limitsStr() const;
        Dimension   dimensions() const;
      };
      DetElement(Handle_t e) : Dimension(e) {}
      Handle_t            handle() const { return m_element; }
      int                 id() const;
      std::string         nameStr() const;
      std::string         typeStr() const;
      std::string         materialStr() const;
      std::string         visStr() const;
      std::string         regionStr() const;
      std::string         limitsStr() const;
      double              inner_field() const;
      double              outer_field() const;
      Dimension           dimensions() const;
      bool                isTracker() const;
      bool                isCalorimeter() const;
      bool                isInsideTrackingVolume() const;
      void check(bool condition, const std::string& msg) const;
      template <typename T> T attr(const XMLCh* tag) const { return m_element.attr<T>(tag); }
    };
  }
} /* End namespace DD4hep       */
#endif /* DD4HEP_XMLDETECTOR_H    */
