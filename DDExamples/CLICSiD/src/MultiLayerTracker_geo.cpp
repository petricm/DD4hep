// $Id:$
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

static Ref_t create_detector(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  xml_det_t  x_det     = e;
  string     det_name  = x_det.nameStr();
  string     det_type  = x_det.typeStr();
  Material   air       = lcdd.air();
  DetElement sdet(det_name,x_det.id());
  Volume     motherVol = lcdd.pickMotherVolume(sdet);
  int n = 0;
    
  for(xml_coll_t i(x_det,_X(layer)); i; ++i, ++n)  {
    xml_comp_t x_layer = i;
    string  l_name = det_name+_toString(n,"_layer%d");
    DetElement layer(sdet,_toString(n,"layer%d"),x_layer.id());
    Tube    l_tub;
    Volume  l_vol(l_name,l_tub,air);
    double  z    = x_layer.outer_z();
    double  rmin = x_layer.inner_r();
    double  r    = rmin;
    int m = 0;

    for(xml_coll_t j(x_layer,_X(slice)); j; ++j, ++m)  {
      xml_comp_t x_slice = j;
      Material mat = lcdd.material(x_slice.materialStr());
      string s_name= l_name+_toString(m,"_slice%d");
      double thickness = x_slice.thickness();
      Tube   s_tub(r,r+thickness,2*z,2*M_PI);
      Volume s_vol(s_name, s_tub, mat);

      r += thickness;
      if ( x_slice.isSensitive() ) s_vol.setSensitiveDetector(sens);
      // Set Attributes
      s_vol.setAttributes(lcdd,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
      PlacedVolume spv = l_vol.placeVolume(s_vol,IdentityPos());
      // Slices have no extra id. Take the ID of the layer!
      spv.addPhysVolID("layer",n);
    }
    l_tub.setDimensions(rmin,r,z,0,2*M_PI);
    cout << l_name << " " << rmin << " " << r << " " << z << endl;
    l_vol.setVisAttributes(lcdd,x_layer.visStr());
      
    PlacedVolume lpv = motherVol.placeVolume(l_vol,IdentityPos());
    lpv.addPhysVolID("system",sdet.id()).addPhysVolID(_X(barrel),0);
    layer.setPlacement(lpv);
  }
  sdet.setCombineHits(x_det.attr<bool>(_A(combineHits)),sens);
  return sdet;
}

DECLARE_DETELEMENT(MultiLayerTracker,create_detector);
