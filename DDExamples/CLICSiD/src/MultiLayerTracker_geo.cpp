// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t  x_det     = e;
  string     det_name  = x_det.nameStr();
  string     det_type  = x_det.typeStr();
  Material   air       = lcdd.air();
  DetElement sdet        (det_name,x_det.id());
  Assembly   assembly    (det_name);
  PlacedVolume pv;
  int n = 0;

  for(xml_coll_t i(x_det,_U(layer)); i; ++i, ++n)  {
    xml_comp_t x_layer = i;
    string  l_name = det_name+_toString(n,"_layer%d");
    DetElement layer(sdet,_toString(n,"layer%d"),x_layer.id());
    Tube    l_tub;
    Volume  l_vol(l_name,l_tub,air);
    double  z    = x_layer.outer_z();
    double  rmin = x_layer.inner_r();
    double  r    = rmin;
    int m = 0;

    for(xml_coll_t j(x_layer,_U(slice)); j; ++j, ++m)  {
      xml_comp_t x_slice = j;
      Material mat = lcdd.material(x_slice.materialStr());
      string s_name= l_name+_toString(m,"_slice%d");
      double thickness = x_slice.thickness();
      Tube   s_tub(r,r+thickness,z,2*M_PI);
      Volume s_vol(s_name, s_tub, mat);

      r += thickness;
      if ( x_slice.isSensitive() ) {
	sens.setType("tracker");
	s_vol.setSensitiveDetector(sens);
      }
      // Set Attributes
      s_vol.setAttributes(lcdd,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
      pv = l_vol.placeVolume(s_vol);
      // Slices have no extra id. Take the ID of the layer!
      pv.addPhysVolID("slice",m);
    }
    l_tub.setDimensions(rmin,r,z,0,2*M_PI);
    //cout << l_name << " " << rmin << " " << r << " " << z << endl;
    l_vol.setVisAttributes(lcdd,x_layer.visStr());
      
    pv = assembly.placeVolume(l_vol);
    pv.addPhysVolID("layer",n);
    layer.setPlacement(pv);
  }
  if ( x_det.hasAttr(_U(combineHits)) ) {
    sdet.setCombineHits(x_det.combineHits(),sens);
  }

  pv = lcdd.pickMotherVolume(sdet).placeVolume(assembly);
  pv.addPhysVolID("system",sdet.id()).addPhysVolID("barrel",0);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(MultiLayerTracker,create_detector);
