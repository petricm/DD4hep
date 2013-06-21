// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/GeoHandler.h"
#include "DD4hep/InstanceCount.h"
#include "LCDDImp.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>

#include "TGeoCompositeShape.h"
#include "TGeoBoolNode.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TClass.h"
#include "Reflex/PluginService.h"
#include "XML/DocumentHandler.h"

#if DD4HEP_USE_PYROOT
  #include "TPython.h"
#endif
#ifndef __TIXML__
#include "xercesc/dom/DOMException.hpp"
namespace DD4hep { namespace XML {  
  typedef xercesc::DOMException  XmlException;
}}
#endif

using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;
namespace {
  struct TopDetElement : public DetElement {
    TopDetElement(const string& nam, Volume vol) : DetElement(nam,/* "structure", */0) { object<Object>().volume = vol;    }
  };
  struct TypePreserve {
    LCDDBuildType& m_t;
    TypePreserve(LCDDBuildType& t) : m_t(t) { }
    ~TypePreserve() { m_t = BUILD_NONE; }
  };
}

LCDD& LCDD::getInstance() {
  static LCDD* s_lcdd = new LCDDImp();
  return *s_lcdd; 
}

/// Default constructor
LCDDImp::LCDDImp() 
  : m_world(), m_trackers(), m_worldVol(), m_trackingVol(), m_field("global"),
    m_buildType(BUILD_NONE)
{
  InstanceCount::increment(this);
  m_properties = new Properties();
  if ( 0 == gGeoManager )
  {
    gGeoManager = new TGeoManager();
  }
  {
    m_manager = gGeoManager;
    m_manager->AddNavigator();
    m_manager->SetCurrentNavigator(0);
    //cout << "Navigator:" << (void*)m_manager->GetCurrentNavigator() << endl;
  }
  //if ( 0 == gGeoIdentity ) 
  {
    gGeoIdentity = new TGeoIdentity();
  }
  VisAttr attr("invisible");
  attr.setColor(0.5,0.5,0.5);
  attr.setAlpha(1);
  attr.setLineStyle(VisAttr::SOLID);
  attr.setDrawingStyle(VisAttr::SOLID);
  attr.setVisible(false);
  attr.setShowDaughters(true);
  addVisAttribute(attr);
  m_invisibleVis = attr;
}

/// Standard destructor
LCDDImp::~LCDDImp() {
  DestroyHandles<> del;
  destroyHandle(m_world);
  destroyHandle(m_field);
  destroyHandle(m_header);
  destroyHandle(m_volManager);
  destroyObject(m_properties);
  for_each(m_readouts.begin(),    m_readouts.end(),     del);
  for_each(m_idDict.begin(),      m_idDict.end(),       del);
  for_each(m_limits.begin(),      m_limits.end(),       del);
  for_each(m_regions.begin(),     m_regions.end(),      del);
  for_each(m_alignments.begin(),  m_alignments.end(),   del);
  for_each(m_sensitive.begin(),   m_sensitive.end(),    del);
  for_each(m_display.begin(),     m_display.end(),      del);
  for_each(m_fields.begin(),      m_fields.end(),       del);
  for_each(m_define.begin(),      m_define.end(),       del);

  m_trackers.clear();
  m_worldVol.clear();
  m_trackingVol.clear();
  m_invisibleVis.clear();
  m_materialVacuum.clear();
  m_materialAir.clear();
  delete m_manager;
  InstanceCount::decrement(this);
}

Volume LCDDImp::pickMotherVolume(const DetElement&) const  {     // throw if not existing
  return m_worldVol;
}

LCDD& LCDDImp::addDetector(const Ref_t& x)    { 
  m_detectors.append(x);
  m_world.add(DetElement(x));
  return *this;
}

/// Typed access to constants: access string values
string LCDDImp::constantAsString(const string& name) const {
  Ref_t c = constant(name);
  if ( c.isValid() ) return c->GetTitle();
  throw runtime_error("LCDD: The constant "+name+" is not known to the system.");
}

/// Typed access to constants: long values
long LCDDImp::constantAsLong(const string& name) const {
  return _toLong(constantAsString(name));
}

/// Typed access to constants: double values
double LCDDImp::constantAsDouble(const string& name) const {
  return _toDouble(constantAsString(name));
}

/// Add a field component by named reference to the detector description
LCDD& LCDDImp::addField(const Ref_t& x) {
  m_field.add(x);
  m_fields.append(x);
  return *this;
}

/// Retrieve a matrial by it's name from the detector description
Material LCDDImp::material(const string& name)  const    {
  TGeoMedium* mat = m_manager->GetMedium(name.c_str());
  if ( mat )   {
    return Material(Ref_t(mat));
  }
  throw runtime_error("Cannot find a material the reference name:"+name);
}

Handle<TObject> LCDDImp::getRefChild(const HandleMap& e, const string& name, bool do_throw)  const  {
  HandleMap::const_iterator i = e.find(name);
  if ( i != e.end() )  {
    return (*i).second;
  }
  if ( do_throw )  {
    throw runtime_error("Cannot find a child with the reference name:"+name);
  }
  return 0;
}

namespace {
  struct ShapePatcher : public GeoScan {
    VolumeManager m_volManager;
    DetElement    m_world;
    ShapePatcher(VolumeManager m, DetElement e) : GeoScan(e), m_volManager(m), m_world(e) {}
    void patchShapes()  {
      GeoHandler::Data& data = *m_data;
      string nam;
      cout << "++ Patching names of anonymous shapes...." << endl;
      for(GeoHandler::Data::const_reverse_iterator i=data.rbegin(); i != data.rend(); ++i)   {
	int level = (*i).first;
        const GeoHandler::Data::mapped_type& v = (*i).second;
        for(GeoHandler::Data::mapped_type::const_iterator j=v.begin(); j != v.end(); ++j) {
          const TGeoNode* n = *j;
          TGeoVolume* v     = n->GetVolume();
          TGeoShape*  s     = v->GetShape();
	  const char* sn    = s->GetName();
          if ( 0 == sn || 0 == ::strlen(sn) ) {
            nam = v->GetName();
            nam += "_shape";
            s->SetName(nam.c_str());
          }
          else if ( 0 == ::strcmp(sn,s->IsA()->GetName()) ) {
            nam = v->GetName();
            nam += "_shape";
            s->SetName(nam.c_str());
          }
	  else {
	    nam = sn;
	    if ( nam.find("_shape") == string::npos ) nam += "_shape";
            s->SetName(nam.c_str());
	  }
          if ( s->IsA() == TGeoCompositeShape::Class() ) {
            TGeoCompositeShape* c = (TGeoCompositeShape*)s;
            const TGeoBoolNode* boolean = c->GetBoolNode();
            s  = boolean->GetLeftShape();
	    sn = s->GetName();
	    if ( 0 == sn || 0 == ::strlen(sn) ) {
              s->SetName((nam+"_left").c_str());
	    }
            else if ( 0 == ::strcmp(sn,s->IsA()->GetName()) ) {
              s->SetName((nam+"_left").c_str());
	    }
            s  = boolean->GetRightShape();
	    sn = s->GetName();
	    if ( 0 == sn || 0 == ::strlen(sn) ) {
              s->SetName((nam+"_right").c_str());
	    }
            else if ( 0 == ::strcmp(s->GetName(),s->IsA()->GetName()) ) {
              s->SetName((nam+"_right").c_str());
            }
          }
        }
      }
    }
  };

}

void LCDDImp::endDocument()  {
  TGeoManager* mgr = m_manager;
  if ( !mgr->IsClosed() ) {
    LCDD& lcdd = *this;

#if 0
    Region trackingRegion("TrackingRegion");
    trackingRegion.setThreshold(1);
    trackingRegion.setStoreSecondaries(true);
    add(trackingRegion);
    m_trackingVol.setRegion(trackingRegion);
    // Set the tracking volume to invisible.
    VisAttr trackingVis("TrackingVis");
    trackingVis.setVisible(false);               
    m_trackingVol.setVisAttributes(trackingVis);
    add(trackingVis); 
#endif
    // Set the world volume to invisible.
    VisAttr worldVis("WorldVis");
    worldVis.setAlpha(1.0);
    worldVis.setVisible(false);
    worldVis.setShowDaughters(true);
    worldVis.setColor(1.0,1.0,1.0);
    worldVis.setLineStyle(VisAttr::SOLID);
    worldVis.setDrawingStyle(VisAttr::WIREFRAME);
    m_worldVol.setVisAttributes(worldVis);
    add(worldVis);

    /// Since we allow now for anonymous shapes,
    /// we will rename them to use the name of the volume they are assigned to
    mgr->CloseGeometry();
    ShapePatcher patcher(m_volManager,m_world);
    patcher.patchShapes();
  }
}

void LCDDImp::init()  {
  if ( !m_world.isValid() ) {
    TGeoManager* mgr = m_manager;
    Box worldSolid("world_box","world_x","world_y","world_z");
    Material vacuum = material("Vacuum");
    Material  air   = material("Air");
    Volume   world("world_volume",worldSolid,air);

    m_world          = TopDetElement("world",world);
    m_worldVol       = world;

#if 0
    Tube trackingSolid("tracking_cylinder",
		       0.,
		       _toDouble("tracking_region_radius"),
		       _toDouble("2*tracking_region_zmax"),2*M_PI);
    Volume tracking("tracking_volume",trackingSolid, air);
    m_trackers       = TopDetElement("tracking",tracking);
    m_trackingVol    = tracking;
    PlacedVolume pv  = m_worldVol.placeVolume(tracking);
    m_trackers.setPlacement(pv);
    m_world.add(m_trackers);
#endif
    m_materialAir    = air;
    m_materialVacuum = vacuum;
    m_detectors.append(m_world);
    m_manager->SetTopVolume(m_worldVol);
    m_world.setPlacement(PlacedVolume(mgr->GetTopNode()));    
  }
}

void LCDDImp::fromXML(const string& xmlfile, LCDDBuildType build_type) {
  TypePreserve build_type_preserve(m_buildType=build_type);
#if DD4HEP_USE_PYROOT
  string cmd;
  TPython::Exec("import lcdd");
  cmd = "lcdd.fromXML('" + xmlfile + "')";
  TPython::Exec(cmd.c_str());  
#else
  XML::Handle_t xml_root = XML::DocumentHandler().load(xmlfile).root();
  string tag = xml_root.tag();
  try {
    LCDD* lcdd = this;
    string type = tag + "_XML_reader";
    long result = ROOT::Reflex::PluginService::Create<long>(type,lcdd,&xml_root);
    if ( 0 == result ) {
      throw runtime_error("Failed to locate plugin to interprete files of type"
			  " \""+tag+"\" - no factory:"+type);
    }
    result = *(long*)result;
    if ( result != 1 ) {
      throw runtime_error("Failed to parse the XML file "+xmlfile+" with the plugin "+type);
    }
  }
  catch(const XML::XmlException& e)  {
    throw runtime_error("XML-DOM Exception:\n\""+XML::_toString(e.msg)+
			"\"\n  while parsing "+xmlfile);
  } 
  catch(const exception& e)  {
    throw runtime_error(string(e.what())+"\n           while parsing "+xmlfile);
  }
  catch(...)  {
    throw runtime_error("UNKNOWN exception while parsing "+xmlfile);
  }
#endif
}

void LCDDImp::dump() const  {
  TGeoManager* mgr = m_manager;
  mgr->SetVisLevel(4);
  mgr->SetVisOption(1);
  m_worldVol->Draw("ogl");
}

/// Manipulate geometry using facroy converter
void LCDDImp::apply(const char* factory_type, int argc, char** argv)   {
  string fac = factory_type;
  try {
    long result = ROOT::Reflex::PluginService::Create<long>(fac,(LCDD*)this,argc,argv);
    if ( 0 == result ) {
      throw runtime_error("apply-plugin: Failed to locate plugin "+fac);
    }
    result = *(long*)result;
    if ( result != 1 ) {
      throw runtime_error("apply-plugin: Failed to execute plugin "+fac);
    }
  }
  catch(const XML::XmlException& e)  {
    cout << "XML-DOM Exception:" << XML::_toString(e.msg) << endl;
    throw runtime_error("XML-DOM Exception:\""+XML::_toString(e.msg)+"\" with plugin:"+fac);
  } 
  catch(const exception& e)  {
    cout << "Exception:" << e.what() << endl;
    throw runtime_error("Exception:\""+string(e.what())+"\" with plugin:"+fac);
  }
  catch(...)  {
    cout << "UNKNOWN Exception" << endl;
    throw runtime_error("UNKNOWN exception from plugin:"+fac);
  }
}
