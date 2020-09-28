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
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/detail/ObjectsInterna.h"

// ROOT include files
#include "TClass.h"
#include "TColor.h"
#include "TGeoShape.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"

#include "TGeoVoxelFinder.h"
#include "TGeoShapeAssembly.h"
#include "TGeoScaledShape.h"
#include "TMap.h"

// C/C++ include files
#include <climits>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

/*
 *  The section below uses the new ROOT features using user extensions to volumes
 *  and placements. Once this is common, the above mechanism should be thrown away....
 *
 *  M.Frank
 */

typedef TGeoNode                geo_node_t;
using geo_volume_t = TGeoVolume;
using geo_assembly_t = TGeoVolumeAssembly;
template <typename T> static auto _userExtension(const T& v) -> typename T::Object*  {
  using O = typename T::Object;
  O* o = (O*)(v.ptr()->GetUserExtension());
  return o;
}

ClassImp(PlacedVolumeExtension)
namespace {
  auto _createTGeoVolume(const string& name, TGeoShape* s, TGeoMedium* m) -> TGeoVolume*  {
    auto* e = new geo_volume_t(name.c_str(),s,m);
    e->SetUserExtension(new Volume::Object());
    return e;
  }
  auto _createTGeoVolumeAssembly(const string& name) -> TGeoVolume*  {
    auto* e = new geo_assembly_t(name.c_str()); // It is important to use the correct constructor!!
    e->SetUserExtension(new Assembly::Object());
    return e;
  }
  auto _createTGeoVolumeMulti(const string& name, TGeoMedium* medium) -> TGeoVolumeMulti*  {
    auto* e = new TGeoVolumeMulti(name.c_str(), medium);
    e->SetUserExtension(new VolumeMulti::Object());
    return e;
  }
  auto _data(const PlacedVolume& v) -> PlacedVolume::Object* {
    PlacedVolume::Object* o = _userExtension(v);
    if (o) return o;
    throw runtime_error("dd4hep: Attempt to access invalid handle of type: PlacedVolume");
  }
  /// Accessor to the data part of the Volume
  auto _data(const Volume& v, bool throw_exception = true) -> Volume::Object* {
    //if ( v.ptr() && v.ptr()->IsA() == TGeoVolume::Class() ) return v.data<Volume::Object>();
    Volume::Object* o = _userExtension(v);
    if (o)
      return o;
    else if (!throw_exception)
      return nullptr;
    throw runtime_error("dd4hep: Attempt to access invalid handle of type: PlacedVolume");
  }

  class VolumeImport   {
  public:
    /// Callback for simple imports. Simple add a user extension
    void operator()(TGeoVolume* v)   {
      TClass* c = v->IsA();
      if ( !v->GetUserExtension() )   {
        if ( c == geo_volume_t::Class() )
          v->SetUserExtension(new Volume::Object());
        else if  ( c == geo_assembly_t::Class() )
          v->SetUserExtension(new Assembly::Object());
        else if  ( c == TGeoVolumeMulti::Class() )
          v->SetUserExtension(new VolumeMulti::Object());
        else
          except("dd4hep","VolumeImport: Unknown TGeoVolume sub-class: %s",v->IsA()->GetName());
      }
      if  ( c == TGeoVolumeMulti::Class() )  {
        auto* mv = (TGeoVolumeMulti*)v;
        for(int i=0, n=mv->GetNvolumes(); i<n; ++i)
          (*this)(mv->GetVolume(i));
      }
      for(Int_t i=0; i<v->GetNdaughters(); ++i)  {
        geo_node_t* pv = v->GetNode(i);
        if ( !pv->GetUserExtension() )
          pv->geo_node_t::SetUserExtension(new PlacedVolume::Object());
        (*this)(pv->GetVolume());
      }
    }
    /// Callback for clone imports, where the user extension should be copied
    void operator()(TGeoVolume* new_v, TGeoVolume* old_v, SensitiveDetector sd, int set_bit)   {
      if ( !new_v || !old_v )  {
        except("dd4hep","VolumeImport: ERROR: The refected volume is INVALID!");        
      }
      else if ( !old_v->GetUserExtension() )   {
        except("dd4hep","VolumeImport: ERROR: Reflection of non-dd4hep volume %s",new_v->IsA()->GetName());
      }
      else if ( !new_v->GetUserExtension() )   {
        TClass* c = new_v->IsA();
        Volume old_vol(old_v);
        Volume new_vol(new_v);
        if ( sd.isValid() && old_vol.isSensitive() )   {
          new_vol.setSensitiveDetector(sd);
        }
        if ( c == geo_volume_t::Class() )  {
          Volume::Object *new_e, *old_e = (Volume::Object*)_data(old_vol);
          old_e->reflected = new_v;
          new_v->SetUserExtension(new_e = new Volume::Object(*old_e)); 
          new_e->reflected = old_v;
        }
        else if  ( c == geo_assembly_t::Class() )  {
          Assembly::Object *new_e, *old_e = (Assembly::Object*)_data(old_vol);
          old_e->reflected = new_v;
          new_v->SetUserExtension(new_e = new Assembly::Object(*old_e));
          new_e->reflected = old_v;
        }
        else if  ( c == TGeoVolumeMulti::Class() )   {
          VolumeMulti::Object *new_e, *old_e = (VolumeMulti::Object*)_data(old_vol);
          auto* new_mv = (TGeoVolumeMulti*)new_v;
          auto* old_mv = (TGeoVolumeMulti*)old_v;
          new_v->SetUserExtension(new_e = new VolumeMulti::Object(*old_e));
          old_e->reflected = new_v;
          new_e->reflected = old_v;
          for(int i=0, n=new_mv->GetNvolumes(); i<n; ++i)
            (*this)(new_mv->GetVolume(i), old_mv->GetVolume(i), sd, set_bit);
        }
        else
          except("dd4hep","VolumeImport: Unknown TGeoVolume sub-class: %s",new_v->IsA()->GetName());

        new_vol.setSensitiveDetector(old_vol.sensitiveDetector());
        new_vol.setVisAttributes(old_vol.visAttributes());
        new_vol.setLimitSet(old_vol.limitSet());
        new_vol.setRegion(old_vol.region());

        if ( set_bit >= 0 ) new_vol.setFlagBit(set_bit);
        for(Int_t i=0; i<new_v->GetNdaughters(); ++i)  {
          geo_node_t* pv = new_v->GetNode(i);
          geo_node_t* ov = old_v->GetNode(i);
          if ( !pv->GetUserExtension() )   {
            auto* e = (PlacedVolume::Object*)ov->geo_node_t::GetUserExtension();
            pv->geo_node_t::SetUserExtension(new PlacedVolume::Object(*e));
          }
          (*this)(pv->GetVolume(), ov->GetVolume(), sd, set_bit);
        }
      }
    }
  };

  auto MakeReflection(TGeoVolume* v, const char* newname=nullptr) -> TGeoVolume*  {
    static TMap map(100);
    auto* vol = (TGeoVolume*)map.GetValue(v);
    if ( vol ) {
      return vol;
    }
    vol = v->CloneVolume();
    if (!vol) {
      printout(ERROR,"MakeReflection", "Cannot clone volume %s\n", v->GetName());
      return nullptr;
    }
    map.Add(v, vol);
    string nam;
    if (newname && newname[0])  {
      nam = newname;
      vol->SetName(newname);
    }
    else   {
      nam = v->GetName();
      vol->SetName((nam+"_refl").c_str());
    }
    delete vol->GetNodes();
    vol->SetNodes(nullptr);
    vol->SetBit(TGeoVolume::kVolumeImportNodes, kFALSE);
    v->CloneNodesAndConnect(vol);
    // The volume is now properly cloned, but with the same shape.
    // Reflect the shape (if any) and connect it.
    if (v->GetShape())   {
      auto* scale = new TGeoScale( 1., 1.,-1.);
      TGeoShape* reflected_shape =
        TGeoScaledShape::MakeScaledShape((nam+"_shape_refl").c_str(), v->GetShape(), scale);
      vol->SetShape(reflected_shape);
    }
    // Reflect the daughters.
    Int_t nd = vol->GetNdaughters();
    if ( !nd ) return vol;
    TGeoVolume *new_vol;
    if ( !vol->GetFinder() ) {
      for (Int_t i=0; i < nd; i++) {
        auto* node  = (TGeoNodeMatrix*)vol->GetNode(i);
        TGeoMatrix*     local = node->GetMatrix();
        //         printf("%s before\n", node->GetName());
        //         local->Print();
        Bool_t      reflected = local->IsReflection();
        TGeoMatrix* local_cloned = new TGeoCombiTrans(*local);
        local_cloned->RegisterYourself();
        node->SetMatrix(local_cloned);
        if (!reflected) {
          // We need to reflect only the translation and propagate to daughters.
          // H' = Sz * H * Sz
          local_cloned->ReflectZ(kTRUE,kFALSE);
          local_cloned->ReflectZ(kFALSE,kFALSE);
          //            printf("%s after\n", node->GetName());
          //            node->GetMatrix()->Print();
          new_vol = MakeReflection(node->GetVolume());
          node->SetVolume(new_vol);
          continue;
        }
        // The next daughter is already reflected, so reflect on Z everything and stop
        local_cloned->ReflectZ(kTRUE); // rot + tr
        //         printf("%s already reflected... After:\n", node->GetName());
        //         node->GetMatrix()->Print();
      }
      if ( vol->GetVoxels() ) vol->GetVoxels()->Voxelize();
      return vol;
    }
    // Volume is divided, so we have to reflect the division.
    //   printf("   ... divided %s\n", fFinder->ClassName());
    TGeoPatternFinder *new_finder = v->GetFinder()->MakeCopy(kTRUE);
    if (!new_finder) {
      printout(ERROR,"MakeReflection", "Could not copy finder for volume %s", v->GetName());
      return nullptr;
    }
    new_finder->SetVolume(vol);
    vol->SetFinder(new_finder);
    TGeoNodeOffset *nodeoff;
    new_vol = nullptr;
    for (Int_t i=0; i<nd; i++) {
      nodeoff = (TGeoNodeOffset*)vol->GetNode(i);
      nodeoff->SetFinder(new_finder);
      new_vol = MakeReflection(nodeoff->GetVolume());
      nodeoff->SetVolume(new_vol);
    }
    return vol;
  }

}

/// Default constructor
PlacedVolume::Processor::Processor()   = default;

/// Default destructor
PlacedVolume::Processor::~Processor()   = default;

/// Default constructor
PlacedVolumeExtension::PlacedVolumeExtension()
  : TGeoExtension(), volIDs()
{
  magic = magic_word();
  INCREMENT_COUNTER;
}

/// Default move
PlacedVolumeExtension::PlacedVolumeExtension(PlacedVolumeExtension&& c)
  : TGeoExtension(c), magic(move(c.magic)),  volIDs() {
  INCREMENT_COUNTER;
  volIDs = move(c.volIDs);
}

/// Copy constructor
PlacedVolumeExtension::PlacedVolumeExtension(const PlacedVolumeExtension& c)
  : TGeoExtension(), magic(c.magic),  volIDs() {
  INCREMENT_COUNTER;
  volIDs = c.volIDs;
}

/// Default destructor
PlacedVolumeExtension::~PlacedVolumeExtension() {
  DECREMENT_COUNTER;
}

/// TGeoExtension overload: Method called whenever requiring a pointer to the extension
auto PlacedVolumeExtension::Grab() -> TGeoExtension*   {
  ++this->refCount;
#ifdef ___print_vols
  else  cout << "Placement grabbed....." << endl;
#endif
  return this;
}

/// TGeoExtension overload: Method called always when the pointer to the extension is not needed anymore
void PlacedVolumeExtension::Release() const  {
  auto* ext = const_cast<PlacedVolumeExtension*>(this);
  --ext->refCount;
  if ( 0 == ext->refCount ) delete ext;
}

/// Lookup volume ID
auto
PlacedVolumeExtension::VolIDs::find(const string& name) const -> vector<PlacedVolumeExtension::VolID>::const_iterator {
  for (auto i = this->Base::begin(); i != this->Base::end(); ++i)
    if (name == (*i).first)
      return i;
  return this->end();
}

/// Insert a new value into the volume ID container
auto
PlacedVolumeExtension::VolIDs::insert(const string& name, int value) -> std::pair<vector<PlacedVolumeExtension::VolID>::iterator, bool> {
  auto i = this->Base::begin();
  for (; i != this->Base::end(); ++i)
    if (name == (*i).first)
      break;
  //
  if (i != this->Base::end()) {
    return make_pair(i, false);
  }
  i = this->Base::emplace(this->Base::end(), name, value);
  return make_pair(i, true);
}

/// String representation for debugging
auto PlacedVolumeExtension::VolIDs::str()  const -> string   {
  stringstream str;
  str << hex;
  for(const auto& i : *this )   {
    str << i.first << "=" << setw(4) << right
        << setfill('0') << i.second << setfill(' ') << " ";
  }
  return str.str();
}

/// Check if placement is properly instrumented
auto PlacedVolume::data() const -> PlacedVolume::Object*   {
  PlacedVolume::Object* o = _userExtension(*this);
  return o;
}

/// Access the copy number of this placement within its mother
auto PlacedVolume::copyNumber() const -> int   {
  return m_element ? m_element->GetNumber() : -1;
}

/// Volume material
auto PlacedVolume::material() const -> Material {
  return Material(m_element ? m_element->GetMedium() : nullptr);
}

/// Logical volume of this placement
auto PlacedVolume::volume() const -> Volume {
  return Volume(m_element ? m_element->GetVolume() : nullptr);
}

/// Parent volume (envelope)
auto PlacedVolume::motherVol() const -> Volume {
  return Volume(m_element ? m_element->GetMotherVolume() : nullptr);
}

/// Access to the volume IDs
auto PlacedVolume::volIDs() const -> const PlacedVolume::VolIDs& {
  return _data(*this)->volIDs;
}

/// Add identifier
auto PlacedVolume::addPhysVolID(const string& nam, int value) -> PlacedVolume& {
  _data(*this)->volIDs.emplace_back(nam, value);
  return *this;
}

/// Translation vector within parent volume
auto PlacedVolume::matrix()  const -> const TGeoMatrix&    {
  if ( !isValid() )  {
    except("PlacedVolume","+++ matrix: Failed to access invalid PlacedVolume! [Invalid handle]");
  }
  return *(m_element->GetMatrix());  
}

/// Translation vector within parent volume
auto PlacedVolume::position()  const -> Position    {
  const double* ptr = matrix().GetTranslation();
  return Position(ptr[0],ptr[1],ptr[2]);
}

/// String dump
auto PlacedVolume::toString() const -> string {
  stringstream str;
  Object* obj = _data(*this);
  str << m_element->GetName() << ":  vol='" << m_element->GetVolume()->GetName()
      << "' mat:'" << m_element->GetMatrix()->GetName()
      << "' volID[" << obj->volIDs.size() << "] ";
  for (const auto & volID : obj->volIDs)
    str << volID.first << "=" << volID.second << "  ";
  str << ends;
  return str.str();
}

/// Enable ROOT persistency
ClassImp(VolumeExtension)

/// Default constructor
VolumeExtension::VolumeExtension()
: TGeoExtension(), region(), limits(), vis(), sens_det()   {
  INCREMENT_COUNTER;
}

/// Default destructor
VolumeExtension::~VolumeExtension() {
  region.clear();
  limits.clear();
  vis.clear();
  sens_det.clear();
  DECREMENT_COUNTER;
}

/// TGeoExtension overload: Method called whenever requiring a pointer to the extension
auto VolumeExtension::Grab() -> TGeoExtension*  {
  auto* ext = const_cast<VolumeExtension*>(this);
  ++ext->refCount;
#ifdef ___print_vols
  if ( ext->sens_det.isValid() )
    cout << "Volume grabbed with valid sensitive detector....." << endl;
  else
    cout << "Volume grabbed....." << endl;
#endif
  return ext;
}

/// TGeoExtension overload: Method called always when the pointer to the extension is not needed anymore
void VolumeExtension::Release() const  {
  auto* ext = const_cast<VolumeExtension*>(this);
  --ext->refCount;
  if ( 0 == ext->refCount )   {
#ifdef ___print_vols
    cout << "Volume deleted....." << endl;
#endif
    delete ext;
  }
  else  {
#ifdef ___print_vols
    cout << "VolumeExtension::Release::refCount:" << ext->refCount << endl;
#endif
  }
}

/// Constructor to be used when creating a new geometry tree.
Volume::Volume(const string& nam) {
  m_element = _createTGeoVolume(nam,nullptr,nullptr);
}

/// Constructor to be used when creating a new geometry tree.
Volume::Volume(const string& nam, const string& title) {
  m_element = _createTGeoVolume(nam,nullptr,nullptr);
  m_element->SetTitle(title.c_str());
}

/// Constructor to be used when creating a new geometry tree. Also sets materuial and solid attributes
Volume::Volume(const string& nam, const Solid& sol, const Material& mat) {
  m_element = _createTGeoVolume(nam, sol.ptr(), mat.ptr());
}

/// Constructor to be used when creating a new geometry tree. Also sets materuial and solid attributes
Volume::Volume(const string& nam, const string& title, const Solid& sol, const Material& mat) {
  m_element = _createTGeoVolume(nam, sol.ptr(), mat.ptr());
  m_element->SetTitle(title.c_str());
}

/// Check if placement is properly instrumented
auto Volume::data() const -> Volume::Object*   {
  Volume::Object* o = _userExtension(*this);
  return o;
}

/// Create a reflected volume tree. The reflected volume has left-handed coordinates
auto Volume::reflect()  const -> Volume   {
  return reflect(SensitiveDetector(nullptr));
}
    
/// Create a reflected volume tree. The reflected volume has left-handed coordinates
auto Volume::reflect(SensitiveDetector sd)  const -> Volume   {
  if ( m_element )   {
    VolumeImport imp;
    Object* o = data();
    if ( !o->reflected.isValid() )  {
      TGeoVolume* vol = MakeReflection(m_element);
      imp(vol, m_element, sd, Volume::REFLECTED);
      o->reflected = vol;
    }
    return o->reflected;
  }
  except("dd4hep","Volume: Attempt to reflect an invalid Volume handle.");
  return *this;
}

/// If we import volumes from external sources, we have to attach the extensions to the tree
auto Volume::import() -> Volume&    {
  if ( m_element )   {
    VolumeImport imp;
    imp(m_element);
    return *this;
  }
  except("dd4hep","Volume: Attempt to import an invalid Volume handle.");
  return *this;
}

/// Set user flags in bit-field
void Volume::setFlagBit(unsigned int bit)   {
  if ( bit <= 31 )   {
    data()->flags |= 1<<bit;
    return;
  }
  except("Volume","+++ Volume flag bit outsize range [0...31]: %d",bit);
}

/// Test the user flag bit
auto Volume::testFlagBit(unsigned int bit)   const -> bool    {
  if ( bit <= 31 )   {
    return (data()->flags & 1<<bit) != 0;
  }
  except("Volume","+++ Volume flag bit outsize range [0...31]: %d",bit);
  return false; // Anyhow never called. Only to satisfy the compiler.
}    

/// Test if this volume was reflected
auto Volume::isReflected()   const -> bool    {
  return testFlagBit(REFLECTED);
}

/// Divide volume into subsections (See the ROOT manuloa for details)
auto Volume::divide(const std::string& divname, int iaxis, int ndiv,
                      double start, double step, int numed, const char* option) -> Volume   {
  TGeoVolume* p = m_element;
  if ( p )  {
    VolumeImport imp;
    TGeoVolume* mvp = p->Divide(divname.c_str(), iaxis, ndiv, start, step, numed, option);
    imp(mvp);
    return mvp;
  }
  except("dd4hep","Volume: Attempt to divide an invalid logical volume.");
  return nullptr;
}

auto get_copy_number(TGeoVolume* par) -> Int_t    {
  TObjArray* a = par ? par->GetNodes() : nullptr;
  Int_t copy_nr = (a ? a->GetEntries() : 0);
  return copy_nr;
}

auto _addNode(TGeoVolume* par, TGeoVolume* daughter, int id, TGeoMatrix* transform) -> PlacedVolume {
  TGeoVolume* parent = par;
  if ( !parent )   {
    except("dd4hep","Volume: Attempt to assign daughters to an invalid physical parent volume.");
  }
  if ( !daughter )   {
    except("dd4hep","Volume: Attempt to assign an invalid physical daughter volume.");
  }
  if ( !transform )   {
    except("dd4hep","Volume: Attempt to place volume without placement matrix.");
  }
  if ( transform != detail::matrix::_identity() ) {
    string nam = string(daughter->GetName()) + "_placement";
    transform->SetName(nam.c_str());
  }
#if 0
  if ( transform->IsTranslation() ) {
    cout << daughter->GetName() << ": Translation: " << transform->GetTranslation()[2] << endl;
  }
#endif
  TGeoShape* shape = daughter->GetShape();
  // Need to fix the daughter's BBox of assemblies, if the BBox was not calculated....
  if ( shape->IsA() == TGeoShapeAssembly::Class() )  {
    auto* as = (TGeoShapeAssembly*)shape;
    if ( std::fabs(as->GetDX()) < numeric_limits<double>::epsilon() &&
         std::fabs(as->GetDY()) < numeric_limits<double>::epsilon() &&
         std::fabs(as->GetDZ()) < numeric_limits<double>::epsilon() )  {
      as->NeedsBBoxRecompute();
      as->ComputeBBox();
    }
  }
  geo_node_t* n;
  TString nam_id = TString::Format("%s_%d", daughter->GetName(), id);
  n = static_cast<geo_node_t*>(parent->GetNode(nam_id));
  if ( n != nullptr )  {
    printout(ERROR,"PlacedVolume","++ Attempt to add already exiting node %s",(const char*)nam_id);
  }
  parent->AddNode(daughter, id, transform);
  //n = static_cast<geo_node_t*>(parent->GetNode(id));
  n = static_cast<geo_node_t*>(parent->GetNode(nam_id));
  n->geo_node_t::SetUserExtension(new PlacedVolume::Object());
  return PlacedVolume(n);
}

auto _addNode(TGeoVolume* par, Volume daughter, int copy_nr, const Rotation3D& rot3D) -> PlacedVolume   {
  Position   x,y,z;
  Rotation3D rot = rot3D;
  rot.GetComponents(x,y,z);
  bool left_handed = (x.Cross(y)).Dot(z) < 0;
  if ( left_handed )    {
    // The reflected volume already scales Z with -1 to be left-handed
    // We have to accomplish for and undo this when placing the volume
    rot *= Rotation3D(1., 0., 0., 0., 1., 0., 0., 0., -1.);
    daughter = daughter.reflect();
    //cout << "REFLECTION: (x.Cross(y)).Dot(z) " << (x.Cross(y)).Dot(z) << endl;
  }
  TGeoRotation r;
  double elements[9];
  rot3D.GetComponents(elements);
  r.SetMatrix(elements);
  auto matrix = make_unique<TGeoCombiTrans>(TGeoTranslation(0,0,0),r);
  return _addNode(par, daughter, copy_nr, matrix.release());
}

auto _addNode(TGeoVolume* par, Volume daughter, int copy_nr, const Transform3D& tr) -> PlacedVolume   {
  Position   x, y, z, pos3D;
  Rotation3D rot3D;
  tr.GetRotation(rot3D);
  tr.GetTranslation(pos3D);
  rot3D.GetComponents(x,y,z);
  bool left_handed = (x.Cross(y)).Dot(z) < 0;
  if ( left_handed )    {
    // The reflected volume already scales Z with -1 to be left-handed
    // We have to accomplish for and undo this when placing the volume
    rot3D *= Rotation3D(1., 0., 0., 0., 1., 0., 0., 0., -1.);
    daughter = daughter.reflect();
    //cout << "REFLECTION: (x.Cross(y)).Dot(z) " << (x.Cross(y)).Dot(z) << endl;
  }
  TGeoRotation r;
  double elements[9];
  rot3D.GetComponents(elements);
  r.SetMatrix(elements);
  auto matrix = make_unique<TGeoCombiTrans>(TGeoTranslation(pos3D.x(), pos3D.y(), pos3D.z()),r);
  return _addNode(par, daughter, copy_nr, matrix.release());
}

/// Place daughter volume according to generic Transform3D
auto Volume::placeVolume(const Volume& volume, const Transform3D& trans) const -> PlacedVolume {
  return _addNode(m_element, volume, get_copy_number(m_element), trans);
}

/// Place daughter volume. The position and rotation are the identity
auto Volume::placeVolume(const Volume& volume) const -> PlacedVolume {
  return _addNode(m_element, volume, get_copy_number(m_element), detail::matrix::_identity());
}

/// Place un-rotated daughter volume at the given position.
auto Volume::placeVolume(const Volume& volume, const Position& pos) const -> PlacedVolume {
  return _addNode(m_element, volume, get_copy_number(m_element), detail::matrix::_translation(pos));
}

/// Place rotated daughter volume. The position is automatically the identity position
auto Volume::placeVolume(const Volume& volume, const RotationZYX& rot) const -> PlacedVolume {
  return _addNode(m_element, volume, get_copy_number(m_element), detail::matrix::_rotationZYX(rot));
}

/// Place rotated daughter volume. The position is automatically the identity position
auto Volume::placeVolume(const Volume& volume, const Rotation3D& rot) const -> PlacedVolume {
  return _addNode(m_element, volume, get_copy_number(m_element), rot);
}

/// Place daughter volume according to generic Transform3D
auto Volume::placeVolume(const Volume& volume, int copy_no, const Transform3D& trans) const -> PlacedVolume {
  return _addNode(m_element, volume, copy_no, trans);
}

/// Place daughter volume. The position and rotation are the identity
auto Volume::placeVolume(const Volume& volume, int copy_no) const -> PlacedVolume {
  return _addNode(m_element, volume, copy_no, detail::matrix::_identity());
}

/// Place un-rotated daughter volume at the given position.
auto Volume::placeVolume(const Volume& volume, int copy_no, const Position& pos) const -> PlacedVolume {
  return _addNode(m_element, volume, copy_no, detail::matrix::_translation(pos));
}

/// Place rotated daughter volume. The position is automatically the identity position
auto Volume::placeVolume(const Volume& volume, int copy_no, const RotationZYX& rot) const -> PlacedVolume {
  return _addNode(m_element, volume, copy_no, detail::matrix::_rotationZYX(rot));
}

/// Place rotated daughter volume. The position is automatically the identity position
auto Volume::placeVolume(const Volume& volume, int copy_no, const Rotation3D& rot) const -> PlacedVolume {
  return _addNode(m_element, volume, copy_no, rot);
}

/// Constructor to be used when creating a new parametrized volume object
void Volume::paramVolume1D(size_t count, Volume entity, const Position& inc)   {
  paramVolume1D(Transform3D(), count, entity, Transform3D(inc));
}

/// Constructor to be used when creating a new parametrized volume object
void Volume::paramVolume1D(size_t count, Volume entity, const RotationZYX& inc)  {
  paramVolume1D(Transform3D(), count, entity, Transform3D(inc));
}

/// Constructor to be used when creating a new parametrized volume object
void Volume::paramVolume1D(size_t count, Volume entity, const Transform3D& inc)   {
  paramVolume1D(Transform3D(), count, entity, inc);
}

/// Constructor to be used when creating a new parametrized volume object
void Volume::paramVolume1D(const Transform3D& start,
                           size_t count,
                           Volume entity,
                           const Transform3D& trafo)
{
  Transform3D transformation(start);
  for(size_t i=0; i<count; ++i)    {
    _addNode(m_element, entity, get_copy_number(m_element), detail::matrix::_transform(transformation));
    transformation *= trafo;
  }
}

/// Set the volume's option value
void Volume::setOption(const string& opt) const {
  m_element->SetOption(opt.c_str());
}

/// Access the volume's option value
auto Volume::option() const -> string {
  return m_element->GetOption();
}

/// Set the volume's material
auto Volume::setMaterial(const Material& mat) const -> const Volume& {
  if (mat.isValid()) {
    auto* medium = mat._ptr<TGeoMedium>();
    if (medium) {
      m_element->SetMedium(medium);
      return *this;
    }
    throw runtime_error("dd4hep: Volume: Medium " + string(mat.name()) + " is not registered with geometry manager.");
  }
  throw runtime_error("dd4hep: Volume: Attempt to assign invalid material.");
}

/// Access to the Volume material
auto Volume::material() const -> Material {
  return Material(m_element->GetMedium());
}

#include "TROOT.h"

/// Set Visualization attributes to the volume
auto Volume::setVisAttributes(const VisAttr& attr) const -> const Volume& {
  if ( attr.isValid() ) {
    auto* vis = attr.data<VisAttr::Object>();
    Color_t bright = vis->color;//kBlue;//TColor::GetColorBright(vis->color);
    Color_t dark = vis->color;//kRed;//TColor::GetColorDark(vis->color);
    TColor* c = vis->col;//gROOT->GetColor(dark);
    int draw_style = vis->drawingStyle;
    int line_style = vis->lineStyle;

    m_element->SetVisibility(vis->visible ? kTRUE : kFALSE);
    m_element->SetVisContainers(kTRUE);
    m_element->SetVisDaughters(vis->showDaughters ? kTRUE : kFALSE);
    printout(DEBUG,"setVisAttributes",
             "Set color %3d [%02X,%02X,%02X] DrawingStyle:%9s LineStyle:%6s for volume %s",
             int(vis->color),
             c ? int(255*c->GetRed()) : 0xFF,
             c ? int(255*c->GetGreen()) : 0xFF,
             c ? int(255*c->GetBlue()) : 0xFF,
             draw_style == VisAttr::SOLID ? "Solid" : "Wireframe",
             line_style == VisAttr::SOLID ? "Solid" : "Dashed",
             name()
             );
    m_element->SetLineWidth(10);
    m_element->SetLineColor(dark);
    if (draw_style == VisAttr::SOLID) {
      m_element->SetLineColor(bright);
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,34,25)
      m_element->SetFillColorAlpha(bright,vis->alpha);
#else
      m_element->SetFillColor(bright);
#endif
      m_element->SetFillStyle(1001);   // Root: solid
      // Suggested by Nikiforos. Not optimal.
      //m_element->GetMedium()->GetMaterial()->SetTransparency((1-vis->alpha)*100);

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
      // As suggested by Valentin Volkl https://sft.its.cern.ch/jira/browse/DDFORHEP-20
      //
      // According to https://root.cern.ch/phpBB3/viewtopic.php?t=2309#p66013
      // a transparency>50 will make a volume invisible in the normal pad.
      // Hence: possibly restrict transparency to a maximum of 50.
      //        but let's see first how this behaves.
      m_element->SetTransparency((1-vis->alpha)*100);
#endif
    }
    else {
      printout(DEBUG,"setVisAttributes","Set to wireframe vis:%s",name());
      m_element->SetLineColor(kBlack);
      m_element->SetFillColor(0);
      m_element->SetFillStyle(0);      // Root: hollow
    }
    if (line_style == VisAttr::SOLID)  // Root line style: 1=solid, 2=dash, 3=dot, 4=dash-dot.
      m_element->SetLineStyle(1);
    else if (line_style == VisAttr::DASHED)
      m_element->SetLineStyle(2);
    else
      m_element->SetLineStyle(line_style);


    /*
      m_element->SetVisibility(kTRUE);
      m_element->SetAttBit(TGeoAtt::kVisContainers, kTRUE);
      m_element->SetVisDaughters(kTRUE);
      printout(INFO,"setVisAttributes","Set Line color for volume %s",name());
      m_element->SetLineColor(bright);
      m_element->SetFillColor(bright);
      m_element->SetFillStyle(1001);   // Root: solid
      if (line_style == VisAttr::SOLID)
      m_element->SetFillStyle(1);
      else if (line_style == VisAttr::DASHED)
      m_element->SetFillStyle(2);
      else
      m_element->SetFillStyle(line_style);
      m_element->SetLineWidth(10);
    */
  }
  Volume::Object* o = _userExtension(*this);
  if ( o ) o->vis = attr;
  return *this;
}

/// Set Visualization attributes to the volume
auto Volume::setVisAttributes(const Detector& description, const string& nam) const -> const Volume& {
  if (!nam.empty()) {
    VisAttr attr = description.visAttributes(nam);
    setVisAttributes(attr);
  }
  else {
    /*
      string tag = this->name();
      if ( ::strstr(tag.c_str(),"_slice") )       // Slices turned off by default
      setVisAttributes(description.visAttributes("InvisibleNoDaughters"));
      else if ( ::strstr(tag.c_str(),"_layer") )  // Layers turned off, but daughters possibly visible
      setVisAttributes(description.visAttributes("InvisibleWithDaughters"));
      else if ( ::strstr(tag.c_str(),"_module") ) // Tracker modules similar to layers
      setVisAttributes(description.visAttributes("InvisibleWithDaughters"));
      else if ( ::strstr(tag.c_str(),"_module_component") ) // Tracker modules similar to layers
      setVisAttributes(description.visAttributes("InvisibleNoDaughters"));
    */
  }
  return *this;
}

/// Attach attributes to the volume
auto Volume::setAttributes(const Detector& description, const string& rg, const string& ls, const string& vis) const -> const Volume& {
  if (!rg.empty())
    setRegion(description.region(rg));
  if (!ls.empty())
    setLimitSet(description.limitSet(ls));
  setVisAttributes(description, vis);
  return *this;
}

/// Access the visualisation attributes
auto Volume::visAttributes() const -> VisAttr {
  Object* o = _data(*this, false);
  if (o)
    return o->vis;
  return VisAttr();
}

/// Set the volume's solid shape
auto Volume::setSolid(const Solid& sol) const -> const Volume& {
  m_element->SetShape(sol);
  return *this;
}

/// Access to Solid (Shape)
auto Volume::solid() const -> Solid {
  return Solid((*this)->GetShape());
}

/// Access the bounding box of the volume (if available)
auto Volume::boundingBox() const -> Box {
  Box box = this->solid();
  if ( box.isValid() )   {
    return box;
  }
  else if ( !isValid() )   {
    except("dd4hep","Volume: Cannot access the bounding box of an invalid volume [Invalid Handle]!");
  }
  except("dd4hep","Volume: Cannot access the bounding box an object of type: %s shape: %s",
         this->ptr()->IsA()->GetName(), this->ptr()->GetShape()->IsA()->GetName());
  return box;
}

/// Set the regional attributes to the volume
auto Volume::setRegion(const Detector& description, const string& nam) const -> const Volume& {
  if (!nam.empty()) {
    return setRegion(description.region(nam));
  }
  return *this;
}

/// Set the regional attributes to the volume
auto Volume::setRegion(const Region& obj) const -> const Volume& {
  _data(*this)->region = obj;
  return *this;
}

/// Access to the handle to the region structure
auto Volume::region() const -> Region {
  return _data(*this)->region;
}

/// Set the limits to the volume
auto Volume::setLimitSet(const Detector& description, const string& nam) const -> const Volume& {
  if (!nam.empty()) {
    return setLimitSet(description.limitSet(nam));
  }
  return *this;
}

/// Set the limits to the volume
auto Volume::setLimitSet(const LimitSet& obj) const -> const Volume& {
  _data(*this)->limits = obj;
  return *this;
}

/// Access to the limit set
auto Volume::limitSet() const -> LimitSet {
  return _data(*this)->limits;
}

/// Assign the sensitive detector structure
auto Volume::setSensitiveDetector(const SensitiveDetector& obj) const -> const Volume& {
  //cout << "Setting sensitive detector '" << obj.name() << "' to volume:" << ptr() << " " << name() << endl;
  _data(*this)->sens_det = obj;
  return *this;
}

/// Access to the handle to the sensitive detector
auto Volume::sensitiveDetector() const -> Ref_t {
  const Object* o = _data(*this);
  return o->sens_det;
}

/// Accessor if volume is sensitive (ie. is attached to a sensitive detector)
auto Volume::isSensitive() const -> bool {
  return _data(*this)->sens_det.isValid();
}

/// Constructor to be used when creating a new assembly object
Assembly::Assembly(const string& nam) {
  m_element = _createTGeoVolumeAssembly(nam);
}

/// Constructor to be used when creating a new multi-volume object
VolumeMulti::VolumeMulti(const string& nam, Material mat) {
  m_element = _createTGeoVolumeMulti(nam, mat.ptr());
}

/// Copy from pointer as a result of Solid->Divide()
void VolumeMulti::verifyVolumeMulti()   {
  if ( m_element )  {
    // This will lead to an exception if the type is not TGeoVolumeMulti
    TGeoVolumeMulti* multi = detail::safe_cast<TGeoVolumeMulti>::cast(m_element);
    if ( multi )  {
      import();
      return;
    }
    // Force a bad cast exception
    Handle<TGeoVolumeMulti> handle(m_element);
    if ( handle.isValid() )  {}
  }
}

/// Output mesh vertices to string
auto dd4hep::toStringMesh(PlacedVolume place, int prec) -> std::string   {
  Volume       vol   = place->GetVolume();
  TGeoMatrix*  mat   = place->GetMatrix();
  Solid        sol   = vol.solid();
  stringstream os;


  if ( vol->IsA() == TGeoVolumeAssembly::Class() )    {
    for(Int_t i=0; i<vol->GetNdaughters(); ++i)  {
      os << toStringMesh(vol->GetNode(i), prec) << endl;
    }
    return os.str();
  }

  // Prints shape parameters
  Int_t nvert = 0, nsegs = 0, npols = 0;
  sol->GetMeshNumbers(nvert, nsegs, npols);
  auto* points = new Double_t[3*nvert];
  sol->SetPoints(points);

  os << setw(16) << left << sol->IsA()->GetName()
     << " " << nvert << " Mesh-points:" << endl;
  os << setw(16) << left << sol->IsA()->GetName() << " " << sol->GetName()
     << " N(mesh)=" << sol->GetNmeshVertices()
     << "  N(vert)=" << nvert << "  N(seg)=" << nsegs << "  N(pols)=" << npols << endl;
    
  for(Int_t i=0; i<nvert; ++i)   {
    Double_t* p = points + 3*i;
    Double_t global[3], local[3] = {p[0], p[1], p[2]};
    mat->LocalToMaster(local, global);
    os << setw(16) << left << sol->IsA()->GetName() << " " << setw(3) << left << i
       << " Local  ("  << setw(7) << setprecision(prec) << fixed << right << local[0]
       << ", "         << setw(7) << setprecision(prec) << fixed << right << local[1]
       << ", "         << setw(7) << setprecision(prec) << fixed << right << local[2]
       << ") Global (" << setw(7) << setprecision(prec) << fixed << right << global[0]
       << ", "         << setw(7) << setprecision(prec) << fixed << right << global[1]
       << ", "         << setw(7) << setprecision(prec) << fixed << right << global[2]
       << ")" << endl;
  }
  Box box = sol;
  const Double_t* org = box->GetOrigin();
  os << setw(16) << left << sol->IsA()->GetName()
     << " Bounding box: "
     << " dx="        << setw(7) << setprecision(prec) << fixed << right << box->GetDX()
     << " dy="        << setw(7) << setprecision(prec) << fixed << right << box->GetDY()
     << " dz="        << setw(7) << setprecision(prec) << fixed << right << box->GetDZ()
     << " Origin: x=" << setw(7) << setprecision(prec) << fixed << right << org[0]
     << " y="         << setw(7) << setprecision(prec) << fixed << right << org[1]
     << " z="         << setw(7) << setprecision(prec) << fixed << right << org[2]
     << endl;
  
  /// -------------------- DONE --------------------
  delete [] points;
  return os.str();
}

