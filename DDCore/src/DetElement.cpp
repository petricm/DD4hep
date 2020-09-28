//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/ConditionsInterna.h"
#include "DD4hep/detail/AlignmentsInterna.h"
#include "DD4hep/AlignmentTools.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/Printout.h"
#include "DD4hep/World.h"
#include "DD4hep/Detector.h"

using namespace std;
using namespace dd4hep;
using dd4hep::Alignment;
    
namespace {
  static string s_empty_string;
}

/// Default constructor
DetElement::Processor::Processor()   = default;

/// Default destructor
DetElement::Processor::~Processor()   = default;

/// Clone constructor
DetElement::DetElement(Object* det_data, const string& det_name, const string& det_type)
  : Handle<DetElementObject>(det_data)
{
  this->assign(det_data, det_name, det_type);
}

/// Constructor for a new subdetector element
DetElement::DetElement(const string& det_name, const string& det_type, int det_id) {
  assign(new Object(det_name,det_id), det_name, det_type);
  ptr()->id = det_id;
}

/// Constructor for a new subdetector element
DetElement::DetElement(const string& det_name, int det_id) {
  assign(new Object(det_name,det_id), det_name, "");
  ptr()->id = det_id;
}

/// Constructor for a new subdetector element
DetElement::DetElement(DetElement det_parent, const string& det_name, int det_id) {
  assign(new Object(det_name,det_id), det_name, det_parent.type());
  ptr()->id = det_id;
  det_parent.add(*this);
}

/// Add an extension object to the detector element
auto DetElement::addExtension(ExtensionEntry* e) const -> void*  {
  return access()->addExtension(e->hash64(), e);
}

/// Access an existing extension object from the detector element
auto DetElement::extension(unsigned long long int k, bool alert) const -> void* {
  return access()->extension(k, alert);
}

/// Internal call to extend the detector element with an arbitrary structure accessible by the type
void DetElement::i_addUpdateCall(unsigned int callback_type, const Callback& callback)  const  {
  access()->updateCalls.emplace_back(callback,callback_type);
}

/// Remove callback from object
void DetElement::removeAtUpdate(unsigned int typ, void* pointer)  const {
  access()->removeAtUpdate(typ,pointer);
}

/// Access to the full path to the placed object
auto DetElement::placementPath() const -> const string& {
  Object* o = ptr();
  if ( o ) {
    if (o->placementPath.empty()) {
      o->placementPath = detail::tools::placementPath(*this);
    }
    return o->placementPath;
  }
  return s_empty_string;
}

/// Access detector type (structure, tracker, calorimeter, etc.).
auto DetElement::type() const -> string {
  return m_element ? m_element->GetTitle() : "";
}

/// Set the type of the sensitive detector
auto DetElement::setType(const string& typ) -> DetElement& {
  access()->SetTitle(typ.c_str());
  return *this;
}

auto DetElement::typeFlag() const -> unsigned int {
  return m_element ? m_element->typeFlag :  0 ;
}

/// Set the type of the sensitive detector
auto DetElement::setTypeFlag(unsigned int types) -> DetElement& {
  access()->typeFlag = types ;
  return *this;
}

namespace {
  static void make_path(DetElement::Object* o)   {
    DetElement par = o->parent;
    if ( par.isValid() )  {
      o->path = par.path() + "/" + o->name;
      if ( o->level < 0 ) o->level = par.level() + 1;
    }
    else {
      o->path = "/" + o->name;
      o->level = 0;
    }
    o->key = dd4hep::detail::hash32(o->path);
  }
}

/// Access hash key of this detector element (Only valid once geometry is closed!)
auto DetElement::key()  const -> unsigned int   {
  Object* o = ptr();
  if ( o )  {
    if ( o->key != 0 )
      return o->key;
    make_path(o);
    return o->key;
  }
  return 0;
}

/// Access the hierarchical level of the detector element
auto DetElement::level()  const -> int   {
  Object* o = ptr();
  if ( o )  {
    if ( o->level >= 0 )
      return o->level;
    make_path(o);
    return o->level;
  }
  return -1;
}

/// Access the full path of the detector element
auto DetElement::path() const -> const string& {
  Object* o = ptr();
  if ( o ) {
    if ( !o->path.empty() )
      return o->path;
    make_path(o);
    return o->path;
  }
  return s_empty_string;
}

auto DetElement::id() const -> int {
  return access()->id;
}

auto DetElement::combineHits() const -> bool {
  return access()->combineHits != 0;
}

auto DetElement::setCombineHits(bool value, SensitiveDetector& sens) -> DetElement& {
  access()->combineHits = value;
  if (sens.isValid())
    sens.setCombineHits(value);
  return *this;
}

/// Access to the alignment information
auto DetElement::nominal() const -> Alignment   {
  Object* o = access();
  if ( !o->nominal.isValid() )   {
    o->nominal = AlignmentCondition("nominal");
    o->nominal->values().detector = *this;
    //o->flag |= Object::HAVE_WORLD_TRAFO;
    //o->flag |= Object::HAVE_PARENT_TRAFO;
    dd4hep::detail::tools::computeIdeal(o->nominal);
  }
  return o->nominal;
}

/// Access to the survey alignment information
auto DetElement::survey() const -> Alignment  {
  Object* o = access();
  if ( !o->survey.isValid() )   {
    o->survey = AlignmentCondition("survey");
    dd4hep::detail::tools::copy(nominal(), o->survey);
  }
  return o->survey;
}

auto DetElement::children() const -> const DetElement::Children& {
  return access()->children;
}

/// Access to individual children by name
auto DetElement::child(const string& child_name) const -> DetElement {
  if (isValid()) {
    const Children& c = ptr()->children;
    auto i = c.find(child_name);
    return i == c.end() ? DetElement() : (*i).second;
  }
  return DetElement();
}

/// Access to the detector elements's parent
auto DetElement::parent() const -> DetElement {
  Object* o = ptr();
  return (o) ? o->parent : DetElement();
}

/// Access to the world object. Only possible once the geometry is closed.
auto DetElement::world()  const -> DetElement   {
  Object* o = ptr();
  return (o) ? o->world() : World();
}

/// Simple checking routine
void DetElement::check(bool cond, const string& msg) const {
  if (cond) {
    throw runtime_error("dd4hep: " + msg);
  }
}

/// Add a new child subdetector element
auto DetElement::add(DetElement sdet) -> DetElement& {
  if (isValid()) {
    pair<Children::iterator, bool> r = object<Object>().children.emplace(sdet.name(), sdet);
    if (r.second) {
      sdet.access()->parent = *this;
      return *this;
    }
    throw runtime_error("dd4hep: DetElement::add: Element " + string(sdet.name()) + 
                        " is already present in path " + path() + " [Double-Insert]");
  }
  throw runtime_error("dd4hep: DetElement::add: Self is not defined [Invalid Handle]");
}

/// Clone (Deep copy) the DetElement structure
auto DetElement::clone(int flg) const -> DetElement   {
  Object* o = access();
  Object* n = o->clone(o->id, flg);
  n->SetName(o->GetName());
  n->SetTitle(o->GetTitle());
  return n;
}

auto DetElement::clone(const string& new_name) const -> DetElement {
  return clone(new_name, access()->id);
}

auto DetElement::clone(const string& new_name, int new_id) const -> DetElement {
  Object* o = access();
  Object* n = o->clone(new_id, COPY_PLACEMENT);
  n->SetName(new_name.c_str());
  n->SetTitle(o->GetTitle());
  return n;
}

auto DetElement::reflect(const string& new_name) const -> pair<DetElement,Volume> {
  return reflect(new_name, access()->id);
}

auto DetElement::reflect(const string& new_name, int new_id) const -> pair<DetElement,Volume> {
  return reflect(new_name, new_id, SensitiveDetector(nullptr));
}

auto DetElement::reflect(const string& new_name, int new_id, SensitiveDetector sd) const -> pair<DetElement,Volume> {
  if ( placement().isValid() )   {
    return m_element->reflect(new_name, new_id, sd);
  }
  except("DetElement","reflect: Only placed DetElement objects can be reflected: %s",
         path().c_str());
  return make_pair(DetElement(),Volume());
}

/// Access to the ideal physical volume of this detector element
auto DetElement::idealPlacement() const -> PlacedVolume    {
  if (isValid()) {
    auto& o = object<Object>();
    return o.idealPlace;
  }
  return PlacedVolume();
}

/// Access to the physical volume of this detector element
auto DetElement::placement() const -> PlacedVolume {
  if (isValid()) {
    auto& o = object<Object>();
    return o.placement;
  }
  return PlacedVolume();
}

/// Set the physical volumes of the detector element
auto DetElement::setPlacement(const PlacedVolume& pv) -> DetElement& {
  if (pv.isValid()) {
    Object* o = access();
    o->placement = pv;
    if ( !o->idealPlace.isValid() )  {
      o->idealPlace = pv;
    }
    return *this;
  }
  throw runtime_error("dd4hep: DetElement::setPlacement: Placement is not defined [Invalid Handle]");
}

/// The cached VolumeID of this subdetector element
auto DetElement::volumeID() const -> dd4hep::VolumeID   {
  if (isValid()) {
    return object<Object>().volumeID;
  }
  return 0;
}

/// Access to the logical volume of the placements (all daughters have the same!)
auto DetElement::volume() const -> Volume {
  return access()->placement.volume();
}

/// Access to the shape of the detector element's placement
auto DetElement::solid() const -> Solid    {
  return volume()->GetShape();
}

auto DetElement::setVisAttributes(const Detector& description, const string& nam, const Volume& vol) -> DetElement& {
  vol.setVisAttributes(description, nam);
  return *this;
}

auto DetElement::setRegion(const Detector& description, const string& nam, const Volume& vol) -> DetElement& {
  if (!nam.empty()) {
    vol.setRegion(description.region(nam));
  }
  return *this;
}

auto DetElement::setLimitSet(const Detector& description, const string& nam, const Volume& vol) -> DetElement& {
  if (!nam.empty()) {
    vol.setLimitSet(description.limitSet(nam));
  }
  return *this;
}

auto DetElement::setAttributes(const Detector& description,
                                      const Volume& vol,
                                      const string& region,
                                      const string& limits,
                                      const string& vis) -> DetElement&
{
  return setRegion(description, region, vol).setLimitSet(description, limits, vol).setVisAttributes(description, vis, vol);
}

/// Constructor
SensitiveDetector::SensitiveDetector(const string& nam, const string& typ) {
  /*
    <calorimeter ecut="0" eunit="MeV" hits_collection="EcalEndcapHits" name="EcalEndcap" verbose="0">
    <global_grid_xy grid_size_x="3.5" grid_size_y="3.5"/>
    <idspecref ref="EcalEndcapHits"/>
    </calorimeter>
  */
  assign(new Object(nam), nam, typ);
  object<Object>().ecut = 0e0;
  object<Object>().verbose = 0;
}

/// Set the type of the sensitive detector
auto SensitiveDetector::setType(const string& typ) -> SensitiveDetector& {
  access()->SetTitle(typ.c_str());
  return *this;
}

/// Access the type of the sensitive detector
auto SensitiveDetector::type() const -> string {
  return m_element ? m_element->GetTitle() : s_empty_string;
}

/// Assign the IDDescriptor reference
auto SensitiveDetector::setReadout(Readout ro) -> SensitiveDetector& {
  access()->readout = ro;
  return *this;
}

/// Assign the IDDescriptor reference
auto SensitiveDetector::readout() const -> Readout {
  return access()->readout;
}

/// Assign the IDDescriptor reference
auto SensitiveDetector::idSpec() const -> IDDescriptor {
  return readout().idSpec();
}

/// Set energy cut off
auto SensitiveDetector::setEnergyCutoff(double value) -> SensitiveDetector& {
  access()->ecut = value;
  return *this;
}

/// Access energy cut off
auto SensitiveDetector::energyCutoff() const -> double {
  return access()->ecut;
}

/// Assign the name of the hits collection
auto SensitiveDetector::setHitsCollection(const string& collection) -> SensitiveDetector& {
  access()->hitsCollection = collection;
  return *this;
}

/// Access the hits collection name
auto SensitiveDetector::hitsCollection() const -> const string& {
  return access()->hitsCollection;
}

/// Assign the name of the hits collection
auto SensitiveDetector::setVerbose(bool value) -> SensitiveDetector& {
  int v = value ? 1 : 0;
  access()->verbose = v;
  return *this;
}

/// Access flag to combine hist
auto SensitiveDetector::verbose() const -> bool {
  return access()->verbose == 1;
}

/// Assign the name of the hits collection
auto SensitiveDetector::setCombineHits(bool value) -> SensitiveDetector& {
  int v = value ? 1 : 0;
  access()->combineHits = v;
  return *this;
}

/// Access flag to combine hist
auto SensitiveDetector::combineHits() const -> bool {
  return access()->combineHits == 1;
}

/// Set the regional attributes to the sensitive detector
auto SensitiveDetector::setRegion(Region reg) -> SensitiveDetector& {
  access()->region = reg;
  return *this;
}

/// Access to the region setting of the sensitive detector (not mandatory)
auto SensitiveDetector::region() const -> Region {
  return access()->region;
}

/// Set the limits to the sensitive detector
auto SensitiveDetector::setLimitSet(LimitSet ls) -> SensitiveDetector& {
  access()->limits = ls;
  return *this;
}

/// Access to the limit set of the sensitive detector (not mandatory).
auto SensitiveDetector::limits() const -> LimitSet {
  return access()->limits;
}

/// Add an extension object to the detector element
auto SensitiveDetector::addExtension(unsigned long long int k,ExtensionEntry* e)  const -> void*
{
  return access()->addExtension(k,e);
}

/// Access an existing extension object from the detector element
auto SensitiveDetector::extension(unsigned long long int k) const -> void* {
  return access()->extension(k);
}

/// Access an existing extension object from the detector element
auto SensitiveDetector::extension(unsigned long long int k, bool alert) const -> void* {
  return access()->extension(k, alert);
}
