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
#include "DDAlign/GlobalDetectorAlignment.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/Printout.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/detail/DetectorInterna.h"

// ROOT include files
#include "TGeoMatrix.h"
#include "TGeoManager.h"


#ifdef __GNUC__    // Disable some diagnostics.
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep::align {

    /// Global alignment data container
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \ingroup DD4HEP_DDALIGN
     */
    class GlobalAlignmentData : public NamedObject  {
    public:
      GlobalAlignment global;
      std::vector<GlobalAlignment> volume_alignments;
    public:
      GlobalAlignmentData(const std::string& path) : NamedObject(path,"global-alignment") {
        global = GlobalAlignment(path);
      }
      ~GlobalAlignmentData() override {
        detail::destroyHandle (global);
      }
    };
  } /* End namespace dd4hep                    */

using namespace std;
using namespace dd4hep;
using namespace dd4hep::align;

typedef vector<pair<int,DetElement> > LevelElements;

DD4HEP_INSTANTIATE_HANDLE_NAMED(GlobalAlignmentData);


namespace {

  static bool s_GlobalDetectorAlignment_debug = false;
  
  auto _align(const GlobalAlignment& a, TGeoHMatrix* transform, bool check, double overlap) -> GlobalAlignment {
    TGeoPhysicalNode* n = a.ptr();
    if ( n )  {
      TGeoMatrix* mm = n->GetNode()->GetMatrix();
      bool dbg = GlobalDetectorAlignment::debug();
      if ( dbg )  {
        printout(INFO,"Alignment","DELTA matrix of %s",n->GetName());
        transform->Print();
        printout(INFO,"Alignment","OLD matrix of %s",n->GetName());
        mm->Print();
      }
      transform->MultiplyLeft(mm); // orig * delta
      n->Align(transform, nullptr, check, overlap);
      if ( dbg )   {
        printout(INFO,"Alignment","NEW matrix of %s",n->GetName());
        n->GetNode()->GetMatrix()->Print();
      }
      /*
        printout(INFO,"Alignment","Apply new relative matrix  mother to daughter:");
        transform->Print();
        transform->MultiplyLeft(mm); // orig * delta
        printout(INFO,"Alignment","With deltas %s ....",n->GetName());
        transform->Print();
        n->Align(transform, 0, check, overlap);

        Position local, global = a.toGlobal(local);
        cout << "Local:" << local << " Global: " << global
        << " and back:" << a.globalToLocal(global) << endl;
      */
      return GlobalAlignment(n);
    }
    throw runtime_error("dd4hep: Cannot align non existing physical node. [Invalid Handle]");
  }

  auto _alignment(const GlobalDetectorAlignment& det) -> GlobalAlignment  {
    DetElement::Object& e = det._data();
    if ( !e.global_alignment.isValid() )  {
      string path = detail::tools::placementPath(det);
      e.global_alignment = Ref_t(new GlobalAlignmentData(path));
    }
    Handle<GlobalAlignmentData> h(e.global_alignment);
    if ( h.isValid() && h->global.isValid() )  {
      return h->global;
    }
    throw runtime_error("dd4hep: Cannot access global alignment data. [Invalid Handle]");
  }

  void _dumpParentElements(GlobalDetectorAlignment& det, LevelElements& elements)   {
    int level = 0;
    detail::tools::PlacementPath nodes;
    detail::tools::ElementPath   det_nodes;
    detail::tools::placementPath(det,nodes);
    detail::tools::elementPath(det,det_nodes);
    ///    cout << "Placement path:";
    auto j=nodes.rbegin();
    auto   k=det_nodes.rbegin();
    for(; j!=nodes.rend(); ++j, ++level)  {
      //cout << "(" << level << ") " << (void*)((*j).ptr())
      //           << " " << string((*j)->GetName()) << " ";
      if ( ::strcmp((*j).ptr()->GetName(),(*k).placement().ptr()->GetName()) )  {
        //cout << "[DE]";
        elements.emplace_back(level,*k);
        ++k;
      }
      else  {
        //elements.emplace_back(level,DetElement());
      }
      //cout << " ";
    }
    //cout << endl;
  }
}

/// Initializing constructor
GlobalDetectorAlignment::GlobalDetectorAlignment(DetElement e)
  : DetElement(e)
{
}

/// Access debugging flag
auto GlobalDetectorAlignment::debug() -> bool   {
  return s_GlobalDetectorAlignment_debug;
}

/// Set debugging flag
auto GlobalDetectorAlignment::debug(bool value) -> bool   {
  bool tmp = s_GlobalDetectorAlignment_debug;
  s_GlobalDetectorAlignment_debug = value;
  return tmp;
}

/// Collect all placements from the detector element up to the world volume
void GlobalDetectorAlignment::collectNodes(vector<PlacedVolume>& nodes)   {
  detail::tools::placementPath(*this,nodes);
}

/// Access to the alignment block
auto GlobalDetectorAlignment::alignment() const -> GlobalAlignment   {
  return _alignment(*this);
}

/// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
auto GlobalDetectorAlignment::volumeAlignments() -> vector<GlobalAlignment>&  {
  Handle<GlobalAlignmentData> h(_data().global_alignment);
  return h->volume_alignments;
}

/// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
auto GlobalDetectorAlignment::volumeAlignments() const -> const vector<GlobalAlignment>&   {
  Handle<GlobalAlignmentData> h(_data().global_alignment);
  return h->volume_alignments;
}

/// Align the PhysicalNode of the placement of the detector element (translation only)
auto GlobalDetectorAlignment::align(const Position& pos, bool chk, double overlap) -> GlobalAlignment {
  return align(detail::matrix::_transform(pos),chk,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (rotation only)
auto GlobalDetectorAlignment::align(const RotationZYX& rot, bool chk, double overlap) -> GlobalAlignment {
  return align(detail::matrix::_transform(rot),chk,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation + rotation)
auto GlobalDetectorAlignment::align(const Position& pos, const RotationZYX& rot, bool chk, double overlap) -> GlobalAlignment {
  return align(detail::matrix::_transform(pos,rot),chk,overlap);
}

/// Align the physical node according to a generic Transform3D
auto GlobalDetectorAlignment::align(const Transform3D& transform, bool chk, double overlap) -> GlobalAlignment  {
  return align(detail::matrix::_transform(transform),chk,overlap);
}

/// Align the physical node according to a generic TGeo matrix
auto GlobalDetectorAlignment::align(TGeoHMatrix* matrix, bool chk, double overlap) -> GlobalAlignment  {
  return _align(_alignment(*this),matrix,chk,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation only)
auto GlobalDetectorAlignment::align(const string& elt_path, const Position& pos, bool chk, double overlap) -> GlobalAlignment {
  return align(elt_path,detail::matrix::_transform(pos),chk,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (rotation only)
auto GlobalDetectorAlignment::align(const string& elt_path, const RotationZYX& rot, bool chk, double overlap) -> GlobalAlignment {
  return align(elt_path,detail::matrix::_transform(rot),chk,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation + rotation)
auto
GlobalDetectorAlignment::align(const string& elt_path, const Position& pos, const RotationZYX& rot, bool chk, double overlap) -> GlobalAlignment {
  return align(elt_path,detail::matrix::_transform(pos,rot),chk,overlap);
}

/// Align the physical node according to a generic Transform3D
auto GlobalDetectorAlignment::align(const string& elt_path, const Transform3D& transform, bool chk, double overlap) -> GlobalAlignment  {
  return align(elt_path,detail::matrix::_transform(transform),chk,overlap);
}

/// Align the physical node according to a generic TGeo matrix
auto GlobalDetectorAlignment::align(const string& elt_path, TGeoHMatrix* matrix, bool chk, double overlap) -> GlobalAlignment  {
  if ( elt_path.empty() )
    return _align(_alignment(*this),matrix,chk,overlap);
  else if ( elt_path == placementPath() )
    return _align(_alignment(*this),matrix,chk,overlap);
  else if ( elt_path[0] == '/' )   {
    GlobalAlignment a(elt_path);
    volumeAlignments().emplace_back(a);
    return _align(a,matrix,chk,overlap);
  }
  GlobalAlignment a(placementPath()+'/'+elt_path);
  volumeAlignments().emplace_back(a);
  return _align(a,matrix,chk,overlap);
}
