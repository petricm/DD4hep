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
#define DETECTORTOOLS_CPP
#include "DD4hep/DetectorTools.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"
#include "DD4hep/detail/DetectorInterna.h"

// C/C++ include files
#include <stdexcept>
#include <memory>

// ROOT include files
#include "TGeoMatrix.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep::detail::tools  {
      /// Assemble the path of the PlacedVolume selection
      auto elementPath(const PlacementPath& nodes, bool reverse) -> std::string;
      /// Collect detector elements to any parent detector element
      void elementPath(DetElement parent, DetElement elt, ElementPath& detectors);
      /// Collect detector elements placements to the top detector element (world) [fast, but may have holes!]
      void elementPath(DetElement elt, PlacementPath& nodes);
      /// Collect detector elements placements to the parent detector element [no holes!]
      void elementPath(DetElement parent, DetElement element, PlacementPath& nodes);
      /// Find Child of PlacedVolume and assemble on the fly the path of PlacedVolumes
      auto findChild(PlacedVolume parent, PlacedVolume child, PlacementPath& path) -> bool;


      // Internal helper
      static void makePlacementPath(PlacementPath det_nodes, PlacementPath& all_nodes);
    }

using namespace std;
using namespace dd4hep;

/// Find path between the child element and the parent element
auto detail::tools::isParentElement(DetElement parent, DetElement child) -> bool   {
  if ( parent.isValid() && child.isValid() )  {
    if ( parent.ptr() == child.ptr() ) return true;
    for(DetElement par=child; par.isValid(); par=par.parent())  {
      if ( par.ptr() == parent.ptr() ) return true;
    }
  }
  throw runtime_error("Search for parent detector element with invalid handles not allowed.");
}

/// Find Child of PlacedVolume and assemble on the fly the path of PlacedVolumes
auto detail::tools::findChild(PlacedVolume parent, PlacedVolume child, PlacementPath& path) -> bool {
  if ( parent.isValid() && child.isValid() ) {
    // Check self
    if ( parent.ptr() == child.ptr() ) {
      path.emplace_back(child);
      return true;
    }
    TIter next(parent->GetVolume()->GetNodes());
    // Now check next layer children
    for (auto *daughter = (TGeoNode*) next(); daughter; daughter = (TGeoNode*) next()) {
      if ( daughter == child.ptr() ) {
        path.emplace_back(daughter);
        return true;
      }
    }
    next.Reset();
    // Finally crawl down the tree
    for (auto *daughter = (TGeoNode*) next(); daughter; daughter = (TGeoNode*) next()) {
      PlacementPath sub_path;
      bool res = findChild(daughter, child, sub_path);
      if (res) {
        path.insert(path.end(), sub_path.begin(), sub_path.end());
        path.emplace_back(daughter);
        return res;
      }
    }
  }
  return false;
}

/// Find Child of PlacedVolume and assemble on the fly the path of PlacedVolumes
static auto findChildByName(PlacedVolume parent, PlacedVolume child, detail::tools::PlacementPath& path) -> bool {
  if ( parent.isValid() && child.isValid() ) {
    // Check self
    if ( 0 == ::strcmp(parent.ptr()->GetName(),child.ptr()->GetName()) ) {
      path.emplace_back(child);
      return true;
    }
    TIter next(parent->GetVolume()->GetNodes());
    // Now check next layer children
    for (auto *daughter = (TGeoNode*) next(); daughter; daughter = (TGeoNode*) next()) {
      if ( 0 == ::strcmp(daughter->GetName(),child.ptr()->GetName()) ) {
        path.emplace_back(daughter);
        return true;
      }
    }
    next.Reset();
    // Finally crawl down the tree
    for (auto *daughter = (TGeoNode*) next(); daughter; daughter = (TGeoNode*) next()) {
      detail::tools::PlacementPath sub_path;
      bool res = findChildByName(daughter, child, sub_path);
      if (res) {
        path.insert(path.end(), sub_path.begin(), sub_path.end());
        path.emplace_back(daughter);
        return res;
      }
    }
  }
  return false;
}

/// Collect detector elements to the top detector element (world)
void detail::tools::elementPath(DetElement element, ElementPath& detectors) {
  for(DetElement par = element; par.isValid(); par = par.parent())
    detectors.emplace_back(par);
}

/// Collect detector elements to any parent detector element
void detail::tools::elementPath(DetElement parent, DetElement child, ElementPath& detectors)  {
  detectors.clear();
  if ( parent.isValid() && child.isValid() )  {
    if ( parent.ptr() == child.ptr() )  {
      detectors.emplace_back(child);
      return;
    }
    ElementPath elements;
    for(DetElement par = child; par.isValid(); par = par.parent())  {
      elements.emplace_back(par);
      if ( par.ptr() == parent.ptr() )  {
        detectors = elements;
        return;
      }
    }
    throw runtime_error(string("The detector element ")+parent.name()+string(" is no parent of ")+child.name());
  }
  throw runtime_error("Search for parent detector element with invalid handles not allowed.");
}

/// Collect detector elements placements to the top detector element (world) [fast, but may have holes!]
void detail::tools::elementPath(DetElement parent, DetElement element, PlacementPath& det_nodes) {
  for(DetElement par = element; par.isValid(); par = par.parent())  {
    PlacedVolume pv = par.placement();
    if ( pv.isValid() )  {
      det_nodes.emplace_back(pv);
    }
    if ( par.ptr() == parent.ptr() ) return;
  }
  throw runtime_error(string("The detector element ")+parent.name()+string(" is no parent of ")+element.name());
}

/// Collect detector elements placements to the top detector element (world) [fast, but may have holes!]
void detail::tools::elementPath(DetElement element, PlacementPath& det_nodes) {
  for(DetElement par = element; par.isValid(); par = par.parent())  {
    PlacedVolume pv = par.placement();
    if ( pv.isValid() )  {
      det_nodes.emplace_back(pv);
    }
  }
}

/// Assemble the path of the PlacedVolume selection
auto detail::tools::elementPath(const PlacementPath& nodes, bool reverse) -> string   {
  string path = "";
  if ( reverse )  {
    for(auto i=nodes.rbegin(); i != nodes.rend(); ++i)
      path += "/" + string((*i).name());
  }
  else  {
    for(auto i=begin(nodes); i != end(nodes); ++i)
      path += "/" + string((*i)->GetName());
  }
  return path;
}

/// Assemble the path of the PlacedVolume selection
auto detail::tools::elementPath(const ElementPath& nodes, bool reverse) -> string  {
  string path = "";
  if ( reverse )  {
    for(auto i=nodes.rbegin();i!=nodes.rend();++i)
      path += "/" + string((*i)->GetName());
  }
  else  {
    for(auto node : nodes)
      path += "/" + string(node->GetName());
  }
  return path;
}

/// Assemble the path of a particular detector element
auto detail::tools::elementPath(DetElement element) -> string  {
  ElementPath nodes;
  elementPath(element,nodes);
  return elementPath(nodes);
}

/// Find DetElement as child of the top level volume by it's absolute path
auto detail::tools::findElement(const Detector& description, const string& path) -> DetElement   {
  return findDaughterElement(description.world(),path);
}

/// Find DetElement as child of a parent by it's relative or absolute path
auto detail::tools::findDaughterElement(DetElement parent, const string& subpath) -> DetElement  {
  if ( parent.isValid() )   {
    size_t idx = subpath.find('/',1);
    if ( subpath[0] == '/' )   {
      DetElement top = topElement(parent);
      if ( idx == string::npos ) return top;
      return findDaughterElement(top,subpath.substr(idx+1));
    }
    if ( idx == string::npos )
      return parent.child(subpath);
    string name = subpath.substr(0,idx);
    DetElement node = parent.child(name);
    if ( node.isValid() )   {
      return findDaughterElement(node,subpath.substr(idx+1));
    }
    throw runtime_error("dd4hep: DetElement "+parent.path()+" has no child named:"+name+" [No such child]");
  }
  throw runtime_error("dd4hep: Cannot determine child with path "+subpath+" from invalid parent [invalid handle]");
}

/// Determine top level element (=world) for any element walking up the detector element tree
auto detail::tools::topElement(DetElement child) -> DetElement   {
  if ( child.isValid() )   {
    if ( child.parent().isValid() )
      return topElement(child.parent());
    return child;
  }
  throw runtime_error("dd4hep: DetElement cannot determine top parent (world) [invalid handle]");
}

static void detail::tools::makePlacementPath(PlacementPath det_nodes, PlacementPath& all_nodes)   {
  for (size_t i = 0, n = det_nodes.size(); n > 0 && i < n-1; ++i)   {
    if (!findChildByName(det_nodes[i + 1], det_nodes[i], all_nodes))   {
      throw runtime_error("dd4hep: DetElement cannot determine placement path of "
                          + string(det_nodes[i].name()) + " [internal error]");
    }
  }
  if ( det_nodes.size() > 0 )   {
    all_nodes.emplace_back(det_nodes.back());
  }
}

/// Collect detector elements placements to the top detector element (world) [no holes!]
void detail::tools::placementPath(DetElement element, PlacementPath& all_nodes)   {
  PlacementPath det_nodes;
  elementPath(element,det_nodes);
  makePlacementPath(det_nodes, all_nodes);
}

/// Collect detector elements placements to the parent detector element [no holes!]
void detail::tools::placementPath(DetElement parent, DetElement element, PlacementPath& all_nodes)   {
  PlacementPath det_nodes;
  elementPath(parent,element,det_nodes);
  makePlacementPath(det_nodes, all_nodes);
}

/// Assemble the path of the PlacedVolume selection
auto detail::tools::placementPath(DetElement element) -> string  {
  PlacementPath path;
  placementPath(element,path);
  return placementPath(path);
}

/// Assemble the path of the PlacedVolume selection
auto detail::tools::placementPath(const PlacementPath& nodes, bool reverse) -> string  {
  string path = "";
  if ( reverse )  {
    for(auto i=nodes.rbegin();i!=nodes.rend();++i)
      path += "/" + string((*i)->GetName());
  }
  else  {
    for(auto node : nodes)
      path += "/" + string(node->GetName());
  }
  return path;
}

/// Assemble the path of the PlacedVolume selection
auto detail::tools::placementPath(const vector<const TGeoNode*>& nodes, bool reverse) -> string   {
  string path = "";
  if ( reverse )  {
    for(auto i=nodes.rbegin();i!=nodes.rend();++i)
      path += "/" + string((*i)->GetName());
  }
  else  {
    for(auto node : nodes)
      path += "/" + string(node->GetName());
  }
  return path;
}

/// Update cached matrix to transform to positions to an upper level Placement
void detail::tools::placementTrafo(const PlacementPath& nodes, bool inverse, TGeoHMatrix*& mat) {
  if ( !mat ) mat = new TGeoHMatrix(*gGeoIdentity);
  placementTrafo(nodes,inverse,*mat);
}

/// Update cached matrix to transform to positions to an upper level Placement
void detail::tools::placementTrafo(const PlacementPath& nodes, bool inverse, TGeoHMatrix& mat) {
  mat = *gGeoIdentity;
  if (nodes.size() > 0) {
    for (size_t i = 0, n=nodes.size(); n>0 && i < n-1; ++i)  {
      const PlacedVolume& p = nodes[i];
      mat.MultiplyLeft(p->GetMatrix());
    }
    if ( inverse ) mat = mat.Inverse();
  }
}

/// Find a given node in the hierarchy starting from the top node (absolute placement!)
auto detail::tools::findNode(PlacedVolume top_place, const string& place) -> PlacedVolume   {
  TGeoNode* top = top_place.ptr();
  const char* path = place.c_str();
  // Check if a geometry path is valid without changing the state of the navigator.
  Int_t length = strlen(path);
  if (!length) return nullptr;
  TString spath = path;
  TGeoVolume *vol;
  // Check first occurance of a '/'
  Int_t ind1 = spath.Index("/");
  if (ind1<0) {
    // No '/' so we check directly the path against the name of the top
    if ( strcmp(path,top->GetName()) ) return nullptr;
    return top;
  }
  Int_t ind2 = ind1;
  Bool_t end = kFALSE;
  if (ind1>0) ind1 = -1;   // no trailing '/'
  else ind2 = spath.Index("/", ind1+1);

  if (ind2<0) ind2 = length;
  TString name(spath(ind1+1, ind2-ind1-1));
  if ( name == top->GetName() ) {
    if (ind2>=length-1) return top;
    ind1 = ind2;
  }
  else  {
    return nullptr;
  }
  TGeoNode *node = top;
  // Deeper than just top level
  while (!end) {
    ind2 = spath.Index("/", ind1+1);
    if (ind2<0) {
      ind2 = length;
      end  = kTRUE;
    }
    vol = node->GetVolume();
    name = spath(ind1+1, ind2-ind1-1);
    node = vol->GetNode(name.Data());
    if (!node)
      return nullptr;
    else if (ind2>=length-1)
      return node;
    ind1 = ind2;
  }
  return node;
}

/// Convert VolumeID to string
auto detail::tools::toString(const PlacedVolume::VolIDs& ids) -> string   {
  stringstream log;
  for( const auto& v : ids )
    log << v.first << "=" << v.second << "; ";
  return log.str();
}

/// Convert VolumeID to string
auto detail::tools::toString(const IDDescriptor& dsc, const PlacedVolume::VolIDs& ids, VolumeID code) -> string   {
  stringstream log;
  for( const auto& id : ids )  {
    const BitFieldElement* f = dsc.field(id.first);
    VolumeID value = f->value(code);
    log << id.first << "=" << id.second << "," << value << " [" << f->offset() << "," << f->width() << "] ";
  }
  return log.str();
}

/// Extract all the path elements from a path
auto detail::tools::pathElements(const string& path) -> vector<string>   {
  vector<string> result;
  if ( !path.empty() )  {
    string tmp = path[0]=='/' ? path.substr(1) : path;
    for(size_t idx=tmp.find('/'); idx != string::npos; idx=tmp.find('/'))  {
      string val = tmp.substr(0,idx);
      result.emplace_back(val);
      tmp = tmp.length()>idx ? tmp.substr(idx+1) : string();
    }
    if ( !tmp.empty() )  {
      result.emplace_back(tmp);
    }
  }
  return result;
}
