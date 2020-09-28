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
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DD4hep/InstanceCount.h"

using namespace std;
using namespace dd4hep;

/// Standard constructor
SegmentationObject::SegmentationObject(DDSegmentation::Segmentation* s)
  : magic(magic_word()), useForHitPosition(0),
    detector(nullptr), sensitive(nullptr), segmentation(s)
{
  InstanceCount::increment(this);
}

/// Default destructor
SegmentationObject::~SegmentationObject() {
  InstanceCount::decrement(this);
  if (segmentation) {
    delete segmentation;
  }
}

/// Access the encoding string
auto SegmentationObject::fieldDescription() const -> string {
  return segmentation->fieldDescription();
}

/// Access the segmentation name
auto SegmentationObject::name() const -> const string& {
  return segmentation->name();
}
/// Set the segmentation name
void SegmentationObject::setName(const string& value) {
  segmentation->setName(value);
}

/// Access the segmentation type
auto SegmentationObject::type() const -> const string& {
  return segmentation->type();
}

/// Access the description of the segmentation
auto SegmentationObject::description() const -> const string& {
  return segmentation->description();
}

/// Access the underlying decoder
auto SegmentationObject::decoder() const -> const BitFieldCoder* {
  return segmentation->decoder();
}

/// Set the underlying decoder
void SegmentationObject::setDecoder(const BitFieldCoder* ptr_decoder) const {
  segmentation->setDecoder(ptr_decoder);
}

/// Access to parameter by name
auto SegmentationObject::parameter(const string& parameterName) const -> DDSegmentation::Parameter {
  return segmentation->parameter(parameterName);
}

/// Access to all parameters
auto SegmentationObject::parameters() const -> DDSegmentation::Parameters {
  return segmentation->parameters();
}

/// Set all parameters from an existing set of parameters
void SegmentationObject::setParameters(const DDSegmentation::Parameters& params) {
  segmentation->setParameters(params);
}

/// Determine the local position based on the cell ID
auto SegmentationObject::position(const CellID& cell) const -> Position  {
  return Position(segmentation->position(cell));
}

/// Determine the cell ID based on the position
auto SegmentationObject::cellID(const Position& local,
                                  const Position& global,
                                  const VolumeID& volID) const -> CellID  {
  return segmentation->cellID(local, global, volID);
}

/// Determine the volume ID from the full cell ID by removing all local fields
auto SegmentationObject::volumeID(const CellID& cell) const -> VolumeID   {
  return segmentation->volumeID(cell);
}
