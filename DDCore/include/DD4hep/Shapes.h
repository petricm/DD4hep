// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_GEOMETRY_SOLIDS_H
#define DD4HEP_GEOMETRY_SOLIDS_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Objects.h"

// C/C++ include files
#include <vector>

// Forward declarations
#include "TGeoArb8.h"
#include "TGeoCompositeShape.h"
#include "TGeoCone.h"
#include "TGeoParaboloid.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoTorus.h"
#include "TGeoTrd2.h"
#include "TGeoTube.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    // Forward declarations

    ///  Base class for Solid (shape) objects
    /**
     *   Generic handle holding an object of base TGeoShape.
     *
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoShape.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    template <typename T> struct Solid_type : public Handle<T> {
    protected:
      typedef T Implementation;
      void _setDimensions(double* param);
      /// Assign pointrs and register solid to geometry
      void _assign(T* n, const std::string& nam, const std::string& tit, bool cbbox);

    public:
      /// Default constructor for uninitialized object
      Solid_type() : Handle<T>() {}

      /// Direct assignment using the implementation pointer
      Solid_type(T* p) : Handle<T>(p) {}

      /// Constructor to be used when passing an already created object
      Solid_type(const Handle<T>& e) : Handle<T>(e) {}

      /// Constructor to be used when passing an already created object: need to check pointers
      template <typename Q> Solid_type(const Handle<Q>& e) : Handle<T>(e) {}

      /// Access to shape name
      const char* name() const;

      /// Auto conversion to underlying ROOT object
      operator T*() const { return this->m_element; }

      /// Overloaded operator -> to access underlying object
      T* operator->() const { return this->m_element; }
    };
    typedef Solid_type<TGeoShape> Solid;

    /// Class describing a box shape
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoBBox.html 
     *
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Box : public Solid_type<TGeoBBox> {
    protected:
      /// Internal helper method to support object construction
      void make(const std::string& name, double x, double y, double z);

    public:
      /// Constructor to be used when reading the already parsed box object
      template <typename Q> Box(const Handle<Q>& e) : Solid_type<TGeoBBox>(e) {}

      /// Constructor to create an anonymous new box object (retrieves name from volume)
      Box(double x, double y, double z) { make("", x, y, z); }

      /// Constructor to create an anonymous new box object (retrieves name from volume)
      template <typename X, typename Y, typename Z> Box(const X& x, const Y& y, const Z& z) {
        make("", _toDouble(x), _toDouble(y), _toDouble(z));
      }

      /// Set the box dimensions
      Box& setDimensions(double x, double y, double z);

      /// Access half "length" of the box
      double x() const;

      /// Access half "width" of the box
      double y() const;

      /// Access half "depth" of the box
      double z() const;
    };

    /// Class describing a Polycone shape
    /**
     *   Polycone. It has at least 9 parameters :
     *      - the lower phi limit;
     *      - the range in phi;
     *      - the number of z planes (at least two) where the inner/outer
     *        radii are changing;
     *      - z coordinate, inner and outer radius for each z plane
     *
     *
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoPcon.html 
     *
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Polycone : public Solid_type<TGeoPcon> {
      /// Constructor to be used when reading the already parsed polycone object
      template <typename Q> Polycone(const Handle<Q>& e) : Solid_type<TGeoPcon>(e) {}

      /// Constructor to create a new polycone object
      Polycone(double start, double delta);

      /// Constructor to create a new polycone object. Add at the same time all Z planes
      Polycone(double start, double delta, const std::vector<double>& rmin, const std::vector<double>& rmax,
               const std::vector<double>& z);

      /// Add Z-planes to the Polycone
      void addZPlanes(const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);
    };

    /// Class describing a cone segment shape
    /**
     *   A ConeSegment is, in the general case, a Phi segment of a cone, with
     *   half-length dz, inner and outer radii specified at -dz and +dz.
     *
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoConeSeg.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct ConeSegment : public Solid_type<TGeoConeSeg> {
      /// Constructor to be used when reading the already parsed ConeSegment object
      template <typename Q> ConeSegment(const Handle<Q>& e) : Solid_type<TGeoConeSeg>(e) {}

      /// Constructor to create a new ConeSegment object
      ConeSegment(double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1 = 0.0,
                  double phi2 = 2.0 * M_PI);

      /// Set the cone segment dimensions
      ConeSegment& setDimensions(double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1 = 0.0,
                                 double phi2 = 2.0 * M_PI);
    };

    struct MyConeSeg : public TGeoConeSeg {
    public:
      MyConeSeg() : TGeoConeSeg() {}
      virtual ~MyConeSeg() {}
      double GetRmin() const { return GetRmin1(); }
      double GetRmax() const { return GetRmax1(); }
    };

    /// Class describing a tube shape of a section of a tube
    /**
     *   TGeoTube - cylindrical tube class. It takes 3 parameters :
     *            inner radius, outer radius and half-length dz.
     *
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoConeSeg.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Tube : public Solid_type</*TGeoTubeSeg */ MyConeSeg> {
    protected:
      /// Internal helper method to support object construction
      void make(const std::string& name, double rmin, double rmax, double z, double startPhi, double deltaPhi);

    public:
      /// Constructor to assign an object
      template <typename Q> Tube(const Handle<Q>& e) : Solid_type<MyConeSeg>(e) {}

      /// Constructor to create a new anonymous tube object with attribute initialization
      Tube(double rmin, double rmax, double z) { make("", rmin, rmax, z, 0, 2 * M_PI); }

      /// Constructor to create a new anonymous tube object with attribute initialization
      Tube(double rmin, double rmax, double z, double deltaPhi) { make("", rmin, rmax, z, 0, deltaPhi); }

      /// Constructor to create a new anonymous tube object with attribute initialization
      Tube(double rmin, double rmax, double z, double startPhi, double deltaPhi) {
        make("", rmin, rmax, z, startPhi, deltaPhi);
      }

      /// Legacy: Constructor to create a new identifiable tube object with attribute initialization
      Tube(const std::string& name, double rmin, double rmax, double z) { make(name, rmin, rmax, z, 0, 2 * M_PI); }

      /// Legacy: Constructor to create a new identifiable tube object with attribute initialization
      Tube(const std::string& name, double rmin, double rmax, double z, double deltaPhi) {
        make(name, rmin, rmax, z, 0, deltaPhi);
      }

      /// Constructor to create a new anonymous tube object with attribute initialization
      template <typename RMIN, typename RMAX, typename Z, typename DELTAPHI>
      Tube(const RMIN& rmin, const RMAX& rmax, const Z& z, const DELTAPHI& deltaPhi) {
        make("", _toDouble(rmin), _toDouble(rmax), _toDouble(z), 0, _toDouble(deltaPhi));
      }

      /// Set the tube dimensions
      Tube& setDimensions(double rmin, double rmax, double z, double startPhi = 0.0, double deltaPhi = 2 * M_PI);
    };

    /// Class describing a cone shape
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoCone.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Cone : public Solid_type<TGeoCone> {
    protected:
      /// Internal helper method to support object construction
      void make(const std::string& name, double z, double rmin1, double rmax1, double rmin2, double rmax2);

    public:
      /// Constructor to be used when passing an already created object
      template <typename Q> Cone(const Handle<Q>& e) : Solid_type<TGeoCone>(e) {}

      /// Constructor to create a new anonymous object with attribute initialization
      Cone(double z, double rmin1, double rmax1, double rmin2, double rmax2) { make("", z, rmin1, rmax1, rmin2, rmax2); }

      template <typename Z, typename RMIN1, typename RMAX1, typename RMIN2, typename RMAX2>
      Cone(const Z& z, const RMIN1& rmin1, const RMAX1& rmax1, const RMIN2& rmin2, const RMAX2& rmax2) {
        make("", _toDouble(z), _toDouble(rmin1), _toDouble(rmax1), _toDouble(rmin2), _toDouble(rmax2));
      }

      /// Set the box dimensions
      Cone& setDimensions(double z, double rmin1, double rmax1, double rmin2, double rmax2);
    };

    /// Class describing a trap shape
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoTrap.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Trap : public Solid_type<TGeoTrap> {
    private:
      /// Internal helper method to support object construction
      void make(double pz, double py, double px, double pLTX);

    public:
      /// Constructor to be used when passing an already created object
      Trap(const Trap& e) : Solid_type<TGeoTrap>(e) {}
      /// Constructor to be used when passing an already created object
      template <typename Q> Trap(const Handle<Q>& e) : Solid_type<TGeoTrap>(e) {}

      /// Constructor to create a new anonymous object with attribute initialization
      Trap(double z, double theta, double phi, double y1, double x1, double x2, double alpha1, double y2, double x3,
           double x4, double alpha2);

      /// Constructor to create a new anonymous object for right angular wedge from STEP (Se G4 manual for details)
      Trap(double pz, double py, double px, double pLTX) { make(pz, py, px, pLTX); }

      /// Constructor to create a new anonymous object with attribute initialization
      template <typename PZ, typename PY, typename PX, typename PLTX> Trap(PZ pz, PY py, PX px, PLTX pLTX) {
        make(_toDouble(pz), _toDouble(py), _toDouble(px), _toDouble(pLTX));
      }

      /// Set the trap dimensions
      Trap& setDimensions(double z, double theta, double phi, double y1, double x1, double x2, double alpha1, double y2,
                          double x3, double x4, double alpha2);
    };

    /// Class describing a Trapezoid shape
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoTrd2.html 
     *
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Trapezoid : public Solid_type<TGeoTrd2> {
    private:
      /// Internal helper method to support object construction
      void make(double x1, double x2, double y1, double y2, double z);

    public:
      /// Constructor to be used when passing an already created object
      Trapezoid(const Trapezoid& e) : Solid_type<TGeoTrd2>(e) {}
      /// Constructor to be used when passing an already created object
      template <typename Q> Trapezoid(const Handle<Q>& e) : Solid_type<TGeoTrd2>(e) {}
      /// Constructor to create a new anonymous object with attribute initialization
      Trapezoid(double x1, double x2, double y1, double y2, double z);
      /// Constructor to create a new anonymous object with attribute initialization
      template <typename X1, typename X2, typename Y1, typename Y2, typename Z> Trapezoid(X1 x1, X2 x2, Y1 y1, Y2 y2, Z z) {
        make(_toDouble(x1), _toDouble(x2), _toDouble(y1), _toDouble(y2), _toDouble(z));
      }
      /// Set the Trapezoid dimensions
      Trapezoid& setDimensions(double x1, double x2, double y1, double y2, double z);
    };

    /// Class describing a Torus shape
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoTorus.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Torus : public Solid_type<TGeoTorus> {
    private:
      /// Internal helper method to support object construction
      void make(double r, double rmin, double rmax, double phi, double delta_phi);

    public:
      /// Constructor to be used when passing an already created object
      Torus(const Torus& e) : Solid_type<TGeoTorus>(e) {}
      /// Constructor to be used when passing an already created object
      template <typename Q> Torus(const Handle<Q>& e) : Solid_type<TGeoTorus>(e) {}
      /// Constructor to create a new anonymous object with attribute initialization
      template <typename R, typename RMIN, typename RMAX, typename PHI, typename DELTA_PHI>
      Torus(R r, RMIN rmin, RMAX rmax, PHI phi = M_PI, DELTA_PHI delta_phi = 2. * M_PI) {
        make(_toDouble(r), _toDouble(rmin), _toDouble(rmax), _toDouble(phi), _toDouble(delta_phi));
      }
      /// Constructor to create a new anonymous object with attribute initialization
      Torus(double r, double rmin, double rmax, double phi = M_PI, double delta_phi = 2. * M_PI) {
        make(r, rmin, rmax, phi, delta_phi);
      }
      /// Set the Torus dimensions
      Torus& setDimensions(double r, double rmin, double rmax, double phi, double delta_phi);
    };

    /// Class describing a sphere shape
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoSphere.html 
     *
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Sphere : public Solid_type<TGeoSphere> {
      /// Constructor to be used when passing an already created object
      Sphere(const Sphere& e) : Solid_type<TGeoSphere>(e) {}
      /// Constructor to be used when passing an already created object
      template <typename Q> Sphere(const Handle<Q>& e) : Solid_type<TGeoSphere>(e) {}
      /// Constructor to create a new anonymous object with attribute initialization
      Sphere(double rmin, double rmax, double theta = 0., double delta_theta = M_PI, double phi = 0.0,
             double delta_phi = 2. * M_PI);
      /// Set the Sphere dimensions
      Sphere& setDimensions(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi);
    };

    /// Class describing a Paraboloid shape
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoParaboloid.html 
     *
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Paraboloid : public Solid_type<TGeoParaboloid> {
      /// Constructor to be used when passing an already created object
      Paraboloid(const Paraboloid& e) : Solid_type<TGeoParaboloid>(e) {}
      /// Constructor to be used when passing an already created object
      template <typename Q> Paraboloid(const Handle<Q>& e) : Solid_type<TGeoParaboloid>(e) {}
      /// Constructor to create a new anonymous object with attribute initialization
      Paraboloid(double r_low, double r_high, double delta_z);
      /// Set the Paraboloid dimensions
      Paraboloid& setDimensions(double r_low, double r_high, double delta_z);
    };

    /// Class describing a regular polyhedron shape
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoPgon.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct PolyhedraRegular : public Solid_type<TGeoPgon> {
    protected:
      /// Helper function to create the polyhedron
      void _create(const std::string& name, int nsides, double rmin, double rmax, double zpos, double zneg, double start,
                   double delta);

    public:
      /// Constructor to be used when passing an already created object
      PolyhedraRegular(const PolyhedraRegular& e) : Solid_type<TGeoPgon>(e) {}
      /// Constructor to be used when passing an already created object
      template <typename Q> PolyhedraRegular(const Handle<Q>& e) : Solid_type<TGeoPgon>(e) {}

      /// Constructor to create a new object. Phi(start)=0, deltaPhi=2PI, Z-planes at -zlen/2 and +zlen/2
      PolyhedraRegular(int nsides, double rmin, double rmax, double zlen);
      /// Constructor to create a new object. Phi(start)=0, deltaPhi=2PI, Z-planes a zplanes[0] and zplanes[1]
      PolyhedraRegular(int nsides, double rmin, double rmax, double zplanes[2]);
      /// Constructor to create a new object with phi_start, deltaPhi=2PI, Z-planes at -zlen/2 and +zlen/2
      PolyhedraRegular(int nsides, double phi_start, double rmin, double rmax, double zlen);
    };

    /// Class describing an arbitray solid defined by 8 vertices.
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoArb8.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct EightPointSolid : public Solid_type<TGeoArb8> {
    private:
      /// Internal helper method to support object construction
      void make(double dz, const double* vtx);

    public:
      /// Constructor to be used when passing an already created object
      EightPointSolid(const EightPointSolid& e) : Solid_type<TGeoArb8>(e) {}
      /// Constructor to be used when passing an already created object
      template <typename Q> EightPointSolid(const Handle<Q>& e) : Solid_type<TGeoArb8>(e) {}
      /// Constructor to create a new anonymous object with attribute initialization
      EightPointSolid(double dz, const double* vertices) { make(dz, vertices); }
    };

    /// Base class describing boolean (=union,intersection,subtraction) solids
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoCompositeShape.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct BooleanSolid : public Solid_type<TGeoCompositeShape> {
    protected:
      /// Constructor to be used when passing an already created object
      BooleanSolid() : Solid_type<TGeoCompositeShape>() {}

    public:
      /// Constructor to be used when passing an already created object
      template <typename Q> BooleanSolid(const Handle<Q>& e) : Solid_type<TGeoCompositeShape>(e) {}
    };

    /// Class describing boolean subtraction solid
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoCompositeShape.html 
     *   @see http://root.cern.ch/root/html/TGeoSubtraction.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct SubtractionSolid : public BooleanSolid {
      /// Default constructor
      SubtractionSolid() : BooleanSolid() {}
      /// Constructor to be used when passing an already created object
      SubtractionSolid(const SubtractionSolid& e) : BooleanSolid(e) {}
      /// Constructor to be used when passing an already created object
      template <typename Q> SubtractionSolid(const Handle<Q>& e) : BooleanSolid(e) {}
      /// Constructor to create a new object. Position is identity, Rotation is identity-rotation!
      SubtractionSolid(const Solid& shape1, const Solid& shape2);
      /// Constructor to create a new object. Placement by position, Rotation is identity-rotation!
      SubtractionSolid(const Solid& shape1, const Solid& shape2, const Position& pos);
      /// Constructor to create a new object. Placement by a RotationZYX within the mother
      SubtractionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot);
      /// Constructor to create a new object. Placement by a generic rotoation within the mother
      SubtractionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot);
      /// Constructor to create a new object. Placement by a generic transformation within the mother
      SubtractionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& pos);
    };

    /// Class describing boolean union solid
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoCompositeShape.html 
     *   @see http://root.cern.ch/root/html/TGeoUnion.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct UnionSolid : public BooleanSolid {
      /// Default constructor
      UnionSolid() : BooleanSolid() {}
      /// Constructor to be used when passing an already created object
      UnionSolid(const UnionSolid& e) : BooleanSolid(e) {}
      /// Constructor to be used when passing an already created object
      template <typename Q> UnionSolid(const Handle<Q>& e) : BooleanSolid(e) {}
      /// Constructor to create a new object. Position is identity, Rotation is identity-rotation!
      UnionSolid(const Solid& shape1, const Solid& shape2);
      /// Constructor to create a new object. Placement by position, Rotation is identity-rotation!
      UnionSolid(const Solid& shape1, const Solid& shape2, const Position& pos);
      /// Constructor to create a new object. Placement by a RotationZYX within the mother
      UnionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot);
      /// Constructor to create a new object. Placement by a generic rotoation within the mother
      UnionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot);
      /// Constructor to create a new object. Placement by a generic transformation within the mother
      UnionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& pos);
    };

    /// Class describing boolean intersection solid
    /**
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoCompositeShape.html 
     *   @see http://root.cern.ch/root/html/TGeoIntersection.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct IntersectionSolid : public BooleanSolid {
      /// Default constructor
      IntersectionSolid() : BooleanSolid() {}
      /// Constructor to be used when passing an already created object
      IntersectionSolid(const IntersectionSolid& e) : BooleanSolid(e) {}
      /// Constructor to be used when passing an already created object
      template <typename Q> IntersectionSolid(const Handle<Q>& e) : BooleanSolid(e) {}
      /// Constructor to create a new object. Position is identity, Rotation is identity-rotation!
      IntersectionSolid(const Solid& shape1, const Solid& shape2);
      /// Constructor to create a new object. Placement by position, Rotation is identity-rotation!
      IntersectionSolid(const Solid& shape1, const Solid& shape2, const Position& pos);
      /// Constructor to create a new object. Placement by a RotationZYX within the mother
      IntersectionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot);
      /// Constructor to create a new object. Placement by a generic rotoation within the mother
      IntersectionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot);
      /// Constructor to create a new object. Placement by a generic transformation within the mother
      IntersectionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& pos);
    };

  } /* End namespace Geometry           */
} /* End namespace DD4hep             */
#endif /* DD4HEP_GEOMETRY_SOLIDS_H         */
