// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#define _USE_MATH_DEFINES
#include "DD4hep/LCDD.h"
#include "MatrixHelpers.h"
// C/C++ include files
#include <stdexcept>

// ROOT includes
#include "TGeoShape.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TGeoTrd2.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoParaboloid.h"
#include "TGeoSphere.h"
#include "TGeoTorus.h"
#include "TGeoMatrix.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"

using namespace std;
using namespace DD4hep::Geometry;

template<typename T> void Solid_type<T>::_setDimensions(double* param) {
  this->ptr()->SetDimensions(param);
  this->ptr()->ComputeBBox();
}

/// Assign pointrs and register solid to geometry
template<typename T> 
void Solid_type<T>::_assign(T* n, const string& nam, const string& tit, bool cbbox) {
  this->assign(n,nam,tit);
  if ( cbbox ) n->ComputeBBox();
}

/// Access to shape name
template<typename T> const char* Solid_type<T>::name() const {
  return this->ptr()->GetName();
}

void Box::make(const string& name, double x, double y, double z)  {
  _assign(new TGeoBBox(x*MM_2_CM,y*MM_2_CM,z*MM_2_CM),name,"box",true);
}

/// Set the box dimensionsy
Box& Box::setDimensions(double x, double y, double z)   {
  double params[] = {x*MM_2_CM,y*MM_2_CM,z*MM_2_CM};
  _setDimensions(params);
  return *this;
}

/// Access half "length" of the box
double Box::x() const {
  return this->ptr()->GetDX()*CM_2_MM;
}

/// Access half "width" of the box
double Box::y() const {
  return this->ptr()->GetDY()*CM_2_MM;
}

/// Access half "depth" of the box
double Box::z() const {
  return this->ptr()->GetDZ()*CM_2_MM;
}

/// Constructor to be used when creating a new object
Polycone::Polycone(const string& name)   {
  _assign(new TGeoPcon(0,RAD_2_DEGREE*(2.*M_PI),0),name,"polycone",false);
}

/// Constructor to be used when creating a new object
Polycone::Polycone(double start, double delta)   {
  _assign(new TGeoPcon(RAD_2_DEGREE*start,RAD_2_DEGREE*delta,0),"","polycone",false);
}

/// Constructor to be used when creating a new object
Polycone::Polycone(const string& name, double start, double delta)   {
  _assign(new TGeoPcon(RAD_2_DEGREE*start,RAD_2_DEGREE*delta,0),name,"polycone",false);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(double start, double delta, const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z)
{
  vector<double> params;
  if ( rmin.size() < 2 )  {
    throw runtime_error("PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.push_back(RAD_2_DEGREE * start);
  params.push_back(RAD_2_DEGREE * delta);
  params.push_back(rmin.size());
  for( size_t i=0; i<rmin.size(); ++i )  {
    params.push_back(z[i]*MM_2_CM);
    params.push_back(rmin[i]*MM_2_CM);
    params.push_back(rmax[i]*MM_2_CM);
  }
  _assign( new TGeoPcon(&params[0]),"","polycone",true);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(const string& name, double start, double delta, const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z)
{
  vector<double> params;
  if ( rmin.size() < 2 )  {
    throw runtime_error("PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.push_back(RAD_2_DEGREE * start);
  params.push_back(RAD_2_DEGREE * delta);
  params.push_back(rmin.size());
  for( size_t i=0; i<rmin.size(); ++i )  {
    params.push_back(z[i]*MM_2_CM);
    params.push_back(rmin[i]*MM_2_CM);
    params.push_back(rmax[i]*MM_2_CM);
  }
  _assign( new TGeoPcon(&params[0]),name,"polycone",true);
}

/// Add Z-planes to the Polycone
void Polycone::addZPlanes(const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z)  {
  TGeoPcon* s = *this;
  vector<double> params;
  size_t num = s->GetNz();
  if ( rmin.size() < 2 )  {
    throw runtime_error("PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.push_back(s->GetPhi1());
  params.push_back(s->GetDphi());
  params.push_back(num+rmin.size());
  for( size_t i=0; i<num; ++i )  {
    params.push_back(s->GetZ(i));
    params.push_back(s->GetRmin(i));
    params.push_back(s->GetRmax(i));
  }
  for( size_t i=0; i<rmin.size(); ++i )  {
    params.push_back(z[i]*MM_2_CM);
    params.push_back(rmin[i]*MM_2_CM);
    params.push_back(rmax[i]*MM_2_CM);
  }
  _setDimensions(&params[0]);
}

/// Constructor to be used when creating a new object
ConeSegment::ConeSegment(const string& name)   {
  _assign(new TGeoConeSeg(),name,"cone_segment",false);
}

/// Constructor to be used when creating a new cone segment object
ConeSegment::ConeSegment(double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1, double phi2)  {
  _assign(new TGeoConeSeg(dz*MM_2_CM,rmin1*MM_2_CM,rmax1*MM_2_CM,rmin2*MM_2_CM,rmax2*MM_2_CM,RAD_2_DEGREE*phi1,RAD_2_DEGREE*phi2),"","cone_segment",true);
}

/// Constructor to be used when creating a new cone segment object
ConeSegment::ConeSegment(const string& name, double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1, double phi2)  {
  _assign(new TGeoConeSeg(dz*MM_2_CM,rmin1*MM_2_CM,rmax1*MM_2_CM,rmin2*MM_2_CM,rmax2*MM_2_CM,RAD_2_DEGREE*phi1,RAD_2_DEGREE*phi2),name,"cone_segment",true);
}

/// Set the cone segment dimensions
ConeSegment& ConeSegment::setDimensions(double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1, double phi2) {
  double params[] = {dz*MM_2_CM,rmin1*MM_2_CM,rmax1*MM_2_CM,rmin2*MM_2_CM,rmax2*MM_2_CM,RAD_2_DEGREE*phi1,RAD_2_DEGREE*phi2};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Tube::make(const string& name, double rmin, double rmax, double z, double startPhi, double deltaPhi)
{
  //_assign(new TGeoTubeSeg(rmin*MM_2_CM,rmax*MM_2_CM,z*MM_2_CM,RAD_2_DEGREE*startPhi,RAD_2_DEGREE*deltaPhi),name,"tube",true);
  _assign(new MyConeSeg(),name,"tube",true);
  setDimensions(rmin,rmax,z,startPhi,deltaPhi);
}

/// Set the tube dimensions
Tube& Tube::setDimensions(double rmin, double rmax, double z, double startPhi, double deltaPhi)
{
  //double params[] = {rmin*MM_2_CM,rmax*MM_2_CM,z*MM_2_CM,RAD_2_DEGREE*startPhi,RAD_2_DEGREE*deltaPhi};
  double params[] = {z*MM_2_CM,rmin*MM_2_CM,rmax*MM_2_CM,rmin*MM_2_CM,rmax*MM_2_CM,RAD_2_DEGREE*startPhi,RAD_2_DEGREE*deltaPhi};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Cone::make(const string& name,
		double z,
		double rmin1,
		double rmax1,
		double rmin2,
		double rmax2)
{
  _assign(new TGeoCone(z*MM_2_CM,rmin1*MM_2_CM,rmax1*MM_2_CM,rmin2*MM_2_CM,rmax2*MM_2_CM),name,"cone",true);
}

Cone& Cone::setDimensions(double z,double rmin1,double rmax1,double rmin2,double rmax2)  {
  double params[] = {z*MM_2_CM,rmin1*MM_2_CM,rmax1*MM_2_CM,rmin2*MM_2_CM,rmax2*MM_2_CM};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object
Trapezoid::Trapezoid()  {
  _assign(new TGeoTrd2(0,0,0,0,0),"","trd2",true);
}

/// Constructor to be used when creating a new object
Trapezoid::Trapezoid(const string& name)  {
  _assign(new TGeoTrd2(0,0,0,0,0),name,"trd2",true);
}

/// Constructor to be used when creating a new object with attribute initialization
Trapezoid::Trapezoid(const string& name, double x1, double x2, double y1, double y2, double z)  {
  _assign(new TGeoTrd2(x1*MM_2_CM,x2*MM_2_CM,y1*MM_2_CM,y2*MM_2_CM,z*MM_2_CM),name,"trd2",true);
}

/// Constructor to be used when creating a new object with attribute initialization
Trapezoid::Trapezoid(double x1, double x2, double y1, double y2, double z)  {
  _assign(new TGeoTrd2(x1*MM_2_CM,x2*MM_2_CM,y1*MM_2_CM,y2*MM_2_CM,z*MM_2_CM),"","trd2",true);
}

/// Set the Trapezoid dimensions
Trapezoid& Trapezoid::setDimensions(double x1, double x2, double y1, double y2, double z)  {
  double params[] = {x1*MM_2_CM,x2*MM_2_CM,y1*MM_2_CM,y2*MM_2_CM,z*MM_2_CM};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object
Paraboloid::Paraboloid(const string& name)  {
  _assign(new TGeoParaboloid(0,0,0),name,"paraboloid",true);
}

/// Constructor to be used when creating a new identified object with attribute initialization
Paraboloid::Paraboloid(const string& name, double r_low, double r_high, double delta_z)  {
  _assign(new TGeoParaboloid(r_low*MM_2_CM,r_high*MM_2_CM,delta_z*MM_2_CM),name,"paraboloid",true);
}

/// Constructor to be used when creating a new object with attribute initialization
Paraboloid::Paraboloid(double r_low, double r_high, double delta_z)  {
  _assign(new TGeoParaboloid(r_low*MM_2_CM,r_high*MM_2_CM,delta_z*MM_2_CM),"","paraboloid",true);
}

/// Set the Paraboloid dimensions
Paraboloid& Paraboloid::setDimensions(double r_low, double r_high, double delta_z)  {
  double params[] = {r_low*MM_2_CM,r_high*MM_2_CM,delta_z*MM_2_CM};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new anonymous object
Sphere::Sphere()  {
  _assign(new TGeoSphere(0,0),"","sphere",true);
}

/// Constructor to be used when creating a new identified object
Sphere::Sphere(const string& name)  {
  _assign(new TGeoSphere(0,0),name,"sphere",true);
}

/// Constructor to be used when creating a new identified object with attribute initialization
Sphere::Sphere(const string& name, double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi)  {
  _assign(new TGeoSphere(rmin*MM_2_CM,rmax*MM_2_CM,RAD_2_DEGREE*theta,RAD_2_DEGREE*delta_theta,RAD_2_DEGREE*phi,RAD_2_DEGREE*delta_phi),name,"sphere",true);
}

/// Constructor to be used when creating a new object with attribute initialization
Sphere::Sphere(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi)  {
  _assign(new TGeoSphere(rmin*MM_2_CM,rmax*MM_2_CM,RAD_2_DEGREE*theta,RAD_2_DEGREE*delta_theta,RAD_2_DEGREE*phi,RAD_2_DEGREE*delta_phi),"","sphere",true);
}

/// Set the Sphere dimensions
Sphere& Sphere::setDimensions(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi)  {
  double params[] = {rmin*MM_2_CM,rmax*MM_2_CM,RAD_2_DEGREE*theta,RAD_2_DEGREE*delta_theta,RAD_2_DEGREE*phi,RAD_2_DEGREE*delta_phi};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object
Torus::Torus()  {
  _assign(new TGeoTorus(0,0,0),"","torus",true);
}

/// Constructor to be used when creating a new object
Torus::Torus(const string& name)  {
  _assign(new TGeoTorus(0,0,0),name,"torus",true);
}

/// Constructor to be used when creating a new identified object with attribute initialization
Torus::Torus(const string& name, double r, double rmin, double rmax, double phi, double delta_phi)  {
  _assign(new TGeoTorus(r*MM_2_CM,rmin*MM_2_CM,rmax*MM_2_CM,RAD_2_DEGREE*phi,RAD_2_DEGREE*delta_phi),name,"torus",true);
}

/// Constructor to be used when creating a new object with attribute initialization
Torus::Torus(double r, double rmin, double rmax, double phi, double delta_phi)  {
  _assign(new TGeoTorus(r*MM_2_CM,rmin*MM_2_CM,rmax*MM_2_CM,RAD_2_DEGREE*phi,RAD_2_DEGREE*delta_phi),"","torus",true);
}

/// Set the Torus dimensions
Torus& Torus::setDimensions(double r, double rmin, double rmax, double phi, double delta_phi)  {
  double params[] = {r*MM_2_CM,rmin*MM_2_CM,rmax*MM_2_CM,RAD_2_DEGREE*phi,RAD_2_DEGREE*delta_phi};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new identified object with attribute initialization
Trap::Trap( const string& name,
            double z,
            double theta,
            double phi,
            double y1,
            double x1,
            double x2,
            double alpha1,
            double y2,
            double x3,
            double x4,
            double alpha2)
{
  _assign(new TGeoTrap(z,RAD_2_DEGREE*theta,RAD_2_DEGREE*phi,y1*MM_2_CM,x1*MM_2_CM,x2*MM_2_CM,alpha1,y2*MM_2_CM,x3*MM_2_CM,x4*MM_2_CM,alpha2),name,"trap",true);
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
Trap::Trap( double z,
            double theta,
            double phi,
            double y1,
            double x1,
            double x2,
            double alpha1,
            double y2,
            double x3,
            double x4,
            double alpha2)
{
  _assign(new TGeoTrap(z,RAD_2_DEGREE*theta,RAD_2_DEGREE*phi,y1*MM_2_CM,x1*MM_2_CM,x2*MM_2_CM,alpha1,y2*MM_2_CM,x3*MM_2_CM,x4*MM_2_CM,alpha2),"","trap",true);
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
Trap::Trap( double pz, double py, double px, double pLTX) {
  double z      = pz/2e0;
  double theta  = 0e0;
  double phi    = 0e0;
  double y1     = py/2e0;
  double x1     = px/2e0;
  double x2     = pLTX/2e0;
  double alpha1 = (pLTX-px)/py;
  _assign(new TGeoTrap(z,RAD_2_DEGREE*theta,RAD_2_DEGREE*phi,y1*MM_2_CM,x1*MM_2_CM,x2*MM_2_CM,alpha1,y1*MM_2_CM,x1*MM_2_CM,x2*MM_2_CM,alpha1),"","trap",true);
}

/// Set the trap dimensions
Trap& Trap::setDimensions(double z,double theta,double phi,
                         double y1,double x1,double x2,double alpha1,
                         double y2,double x3,double x4,double alpha2)  {
  double params[] = {z,RAD_2_DEGREE * theta,RAD_2_DEGREE * phi,y1*MM_2_CM,x1*MM_2_CM,x2*MM_2_CM,alpha1,y2*MM_2_CM,x3*MM_2_CM,x4*MM_2_CM,alpha2};
  _setDimensions(params);
  return *this;
}

/// Helper function to create holy hedron
void PolyhedraRegular::_create(const string& name, int nsides, double rmin, double rmax, 
			       double zpos, double zneg, double start, double delta)
{
  if ( rmin<0e0 || rmin>rmax )
    throw runtime_error("PolyhedraRegular: Illegal argument rmin:<"+_toString(rmin)+"> is invalid!");
  else if ( rmax<0e0 )
    throw runtime_error("PolyhedraRegular: Illegal argument rmax:<"+_toString(rmax)+"> is invalid!");
  _assign(new TGeoPgon(),name,"polyhedra",false);
  double params[] = {
    RAD_2_DEGREE * start,
    RAD_2_DEGREE * delta,
    double(nsides),
    2e0,
    zpos*MM_2_CM,rmin*MM_2_CM,rmax*MM_2_CM,
    zneg*MM_2_CM,rmin*MM_2_CM,rmax*MM_2_CM
  };
  _setDimensions(&params[0]);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(const string& name, int nsides, double rmin, double rmax, double zlen)  {
  _create(name,nsides,rmin,rmax,zlen/2,-zlen/2,0,2*M_PI);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double rmin, double rmax, double zlen)   {
  _create("",nsides,rmin,rmax,zlen/2,-zlen/2,0,2*M_PI);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double phistart, double rmin, double rmax, double zlen)   {
  _create("",nsides,rmin,rmax,zlen/2,-zlen/2,phistart,2*M_PI);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double rmin, double rmax, double zplanes[2])   {
  _create("",nsides,rmin,rmax,zplanes[0],zplanes[1],0,2*M_PI);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2)   {
  TGeoSubtraction* sub  = new TGeoSubtraction(shape1,shape2,identityTransform(),identityTransform());
  _assign(new TGeoCompositeShape("",sub),"","subtraction",true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans)   {
  TGeoSubtraction* sub  = new TGeoSubtraction(shape1,shape2,identityTransform(),_transform(trans));
  _assign(new TGeoCompositeShape("",sub),"","subtraction",true);
}

/// Constructor to be used when creating a new object. Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Position& pos)   {
  TGeoSubtraction* sub  = new TGeoSubtraction(shape1,shape2,identityTransform(),_translation(pos));
  _assign(new TGeoCompositeShape("",sub),"","subtraction",true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Rotation& rot)   {
  TGeoSubtraction* sub  = new TGeoSubtraction(shape1,shape2,identityTransform(),_rotation(rot));
  _assign(new TGeoCompositeShape("",sub),"","subtraction",true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot)   {
  TGeoSubtraction* sub  = new TGeoSubtraction(shape1,shape2,identityTransform(),_rotation3D(rot));
  _assign(new TGeoCompositeShape("",sub),"","subtraction",true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2)  {
  TGeoUnion* uni = new TGeoUnion(shape1,shape2,identityTransform(),identityTransform());
  _assign(new TGeoCompositeShape("",uni),"","union",true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans)   {
  TGeoUnion* uni = new TGeoUnion(shape1,shape2,identityTransform(),_transform(trans));
  _assign(new TGeoCompositeShape("",uni),"","union",true);
}

/// Constructor to be used when creating a new object. Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Position& pos)   {
  TGeoUnion* uni = new TGeoUnion(shape1,shape2,identityTransform(),_translation(pos));
  _assign(new TGeoCompositeShape("",uni),"","union",true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Rotation& rot)   {
  TGeoUnion *uni = new TGeoUnion(shape1,shape2,identityTransform(),_rotation(rot));
  _assign(new TGeoCompositeShape("",uni),"","union",true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot)   {
  TGeoUnion *uni = new TGeoUnion(shape1,shape2,identityTransform(),_rotation3D(rot));
  _assign(new TGeoCompositeShape("",uni),"","union",true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2)    {
  TGeoIntersection* inter = new TGeoIntersection(shape1,shape2,identityTransform(),identityTransform());
  _assign(new TGeoCompositeShape("",inter),"","intersection",true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans)   {
  TGeoIntersection*  inter = new TGeoIntersection(shape1,shape2,identityTransform(),_transform(trans));
  _assign(new TGeoCompositeShape("",inter),"","intersection",true);
}

/// Constructor to be used when creating a new object. Position is identity.
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Position& pos)    {
  TGeoIntersection* inter = new TGeoIntersection(shape1,shape2,identityTransform(),_translation(pos));
  _assign(new TGeoCompositeShape("",inter),"","intersection",true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Rotation& rot)  {
  TGeoIntersection* inter = new TGeoIntersection(shape1,shape2,identityTransform(),_rotation(rot));
  _assign(new TGeoCompositeShape("",inter),"","intersection",true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot)  {
  TGeoIntersection* inter = new TGeoIntersection(shape1,shape2,identityTransform(),_rotation3D(rot));
  _assign(new TGeoCompositeShape("",inter),"","intersection",true);
}

#define INSTANTIATE(X) template class DD4hep::Geometry::Solid_type<X>

INSTANTIATE(TGeoShape);
INSTANTIATE(TGeoBBox);
INSTANTIATE(TGeoCone);
INSTANTIATE(TGeoConeSeg);
INSTANTIATE(TGeoParaboloid);
INSTANTIATE(TGeoPcon);
INSTANTIATE(TGeoPgon);
INSTANTIATE(TGeoSphere);
INSTANTIATE(TGeoTorus);
INSTANTIATE(TGeoTube);
INSTANTIATE(TGeoTubeSeg);
INSTANTIATE(TGeoTrap);
INSTANTIATE(TGeoTrd2);
INSTANTIATE(TGeoCompositeShape);
