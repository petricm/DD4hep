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
#include "DD4hep/AlignmentData.h"
#include "DD4hep/detail/AlignmentsInterna.h"
#include "DD4hep/detail/ConditionsInterna.h"

// C/C++ include files
#include <sstream>


using namespace std;
using namespace dd4hep;

const string dd4hep::align::Keys::deltaName("alignment_delta");
const dd4hep::Condition::itemkey_type  dd4hep::align::Keys::deltaKey =
  dd4hep::ConditionKey::itemCode("alignment_delta");

const string dd4hep::align::Keys::alignmentName("alignment");
const dd4hep::Condition::itemkey_type dd4hep::align::Keys::alignmentKey =
  dd4hep::ConditionKey::itemCode("alignment");

/// Default constructor
Alignment::Processor::Processor() = default;

/// Initializing constructor to create a new object (Specialized for AlignmentNamedObject)
Alignment::Alignment(const string& nam)  {
  char*   p = (char*)::operator new(sizeof(Object)+sizeof(AlignmentData));
  auto* o = new(p) Object(nam, "alignment", p+sizeof(Object), sizeof(AlignmentData));
  assign(o, nam, "alignment");
  o->hash   = 0;
}

/// Initializing constructor to create a new object (Specialized for AlignmentObject)
AlignmentCondition::AlignmentCondition(const string& nam)   {
  char*   p = (char*)::operator new(sizeof(Object)+sizeof(AlignmentData));
  auto* o = new(p) Object(nam, "alignment", p+sizeof(Object), sizeof(AlignmentData));
  assign(o, nam, "alignment");
  o->hash   = 0;
}

/// Data accessor for the use of decorators
auto Alignment::data() -> AlignmentData&              {
  return access()->values();
}

/// Data accessor for the use of decorators
auto Alignment::data() const -> const AlignmentData&  {
  return access()->values();
}

/// Access the delta value of the object
auto Alignment::delta() const -> const Delta&   {
  return access()->values().delta;
}

/// Create cached matrix to transform to world coordinates
auto Alignment::worldTransformation()  const -> const TGeoHMatrix&  {
  return access()->values().worldTransformation();
}

/// Access the alignment/placement matrix with respect to the world
auto Alignment::detectorTransformation() const -> const TGeoHMatrix&   {
  return access()->values().detectorTransformation();
}

/// Access to the node list
auto Alignment::nodes() const -> const vector<PlacedVolume>&   {
  return access()->values().nodes;
}

/// Transformation from local coordinates of the placed volume to the world system
void Alignment::localToWorld(const Position& local, Position& global) const  {
  return access()->values().localToWorld(local,global);
}

/// Transformation from local coordinates of the placed volume to the world system
void Alignment::localToWorld(const Double_t local[3], Double_t global[3]) const  {
  return access()->values().localToWorld(local,global);
}
/// Transformation from local coordinates of the placed volume to the world system
auto Alignment::localToWorld(const Position& local) const -> Position  {
  return access()->values().localToWorld(local);
}

/// Transformation from world coordinates of the local placed volume coordinates
void Alignment::worldToLocal(const Position& global, Position& local) const  {
  return access()->values().worldToLocal(global,local);
}

/// Transformation from world coordinates of the local placed volume coordinates
void Alignment::worldToLocal(const Double_t global[3], Double_t local[3]) const  {
  return access()->values().worldToLocal(global,local);
}

/// Transformation from local coordinates of the placed volume to the world system
auto Alignment::worldToLocal(const Position& global) const -> Position  {
  return access()->values().worldToLocal(global);
}

/// Transformation from local coordinates of the placed volume to the detector system
void Alignment::localToDetector(const Position& local, Position& detector) const  {
  return access()->values().localToDetector(local,detector);
}

/// Transformation from local coordinates of the placed volume to the detector system
void Alignment::localToDetector(const Double_t local[3], Double_t detector[3]) const  {
  return access()->values().localToDetector(local,detector);
}

/// Transformation from local coordinates of the placed volume to the world system
auto Alignment::localToDetector(const Position& local) const -> Position  {
  return access()->values().localToDetector(local);
}

/// Transformation from detector element coordinates to the local placed volume coordinates
void Alignment::detectorToLocal(const Position& detector, Position& local) const  {
  return access()->values().detectorToLocal(detector,local);
}

/// Transformation from detector element coordinates to the local placed volume coordinates
void Alignment::detectorToLocal(const Double_t detector[3], Double_t local[3]) const  {
  return access()->values().detectorToLocal(detector,local);
}

/// Transformation from detector element coordinates to the local placed volume coordinates
auto Alignment::detectorToLocal(const Position& detector) const -> Position  {
  return access()->values().detectorToLocal(detector);
}

/// Access the IOV type
auto AlignmentCondition::iovType() const -> const dd4hep::IOVType&   {
  return *(access()->iovType());
}

/// Access the IOV block
auto AlignmentCondition::iov() const -> const dd4hep::IOV&   {
  return *(access()->iovData());
}

/// Access the hash identifier
auto AlignmentCondition::key() const -> AlignmentCondition::key_type   {
  return access()->hash;
}

/// Data accessor for the use of decorators
auto AlignmentCondition::data() -> AlignmentData&              {
  return *(access()->alignment_data);
}

/// Data accessor for the use of decorators
auto AlignmentCondition::data() const -> const AlignmentData&  {
  return *(access()->alignment_data);
}

/// Access the delta value of the object
auto AlignmentCondition::delta() const -> const Delta&   {
  return access()->alignment_data->delta;
}

/// Check if object is already bound....
auto AlignmentCondition::is_bound()  const -> bool  {
  return isValid() ? ptr()->data.is_bound() : false;
}

/// Create cached matrix to transform to world coordinates
auto AlignmentCondition::worldTransformation()  const -> const TGeoHMatrix&  {
  return data().worldTransformation();
}

/// Access the alignment/placement matrix with respect to the world
auto AlignmentCondition::detectorTransformation() const -> const TGeoHMatrix&   {
  return data().detectorTransformation();
}
