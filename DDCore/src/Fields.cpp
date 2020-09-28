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

#include "DD4hep/Fields.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/Handle.inl"

using namespace std;
using namespace dd4hep;

typedef CartesianField::Object CartesianFieldObject;
DD4HEP_INSTANTIATE_HANDLE(CartesianFieldObject);

typedef OverlayedField::Object OverlayedFieldObject;
DD4HEP_INSTANTIATE_HANDLE(OverlayedFieldObject);

namespace {
  void calculate_combined_field(vector<CartesianField>& v, const double* pos, double* field) {
    for (const auto& i : v ) i.value(pos, field);
  }
}

/// Default constructor
CartesianField::Object::Object()
  : NamedObject(), type(UNKNOWN) {
  InstanceCount::increment(this);
}

/// Default destructor
CartesianField::Object::~Object() {
  InstanceCount::decrement(this);
}

/// Access the field type (string)
auto CartesianField::type() const -> const char* {
  return m_element->GetTitle();
}

/// Does the field change the energy of charged particles?
auto CartesianField::changesEnergy() const -> bool {
  return ELECTRIC == (fieldType() & ELECTRIC);
}

/// Access to properties container
auto CartesianField::properties() const -> CartesianField::Properties& {
  return data<Object>()->properties;
}

/// Returns the 3 field components (x, y, z).
void CartesianField::value(const Position& pos, Direction& field) const  {
  value(pos,(double*)&field);
}

/// Returns the 3 field components (x, y, z).
void CartesianField::value(const Position& pos, double* val) const   {
  value((double*)&pos,val);
}

/// Returns the 3 field components (x, y, z).
void CartesianField::value(const double* pos, double* val) const {
  data<Object>()->fieldComponents(pos, val);
}

/// Default constructor
OverlayedField::Object::Object()
  : type(0), electric(), magnetic() {
  InstanceCount::increment(this);
}

/// Default destructor
OverlayedField::Object::~Object() {
  InstanceCount::decrement(this);
}

/// Object constructor
OverlayedField::OverlayedField(const string& nam)
  : Ref_t() {
  assign(new Object(), nam, "overlay_field");
}

/// Access to properties container
auto OverlayedField::properties() const -> OverlayedField::Properties& {
  return data<Object>()->properties;
}

/// Does the field change the energy of charged particles?
auto OverlayedField::changesEnergy() const -> bool {
  int field = data<Object>()->type;
  return CartesianField::ELECTRIC == (field & CartesianField::ELECTRIC);
}

/// Add a new field component
void OverlayedField::add(CartesianField field) {
  if (field.isValid()) {
    auto* o = data<Object>();
    if (o) {
      int typ = field.fieldType();
      bool isEle = field.ELECTRIC == (typ & field.ELECTRIC);
      bool isMag = field.MAGNETIC == (typ & field.MAGNETIC);
      if (isEle) {
        vector < CartesianField > &v = o->electric_components;
        v.emplace_back(field);
        o->type |= field.ELECTRIC;
        o->electric = v.size() == 1 ? field : CartesianField();
      }
      if (isMag) {
        vector < CartesianField > &v = o->magnetic_components;
        v.emplace_back(field);
        o->type |= field.MAGNETIC;
        o->magnetic = v.size() == 1 ? field : CartesianField();
      }
      if (isMag || isEle)
        return;
      throw runtime_error("OverlayedField::add: Attempt to add an unknown field type.");
    }
    throw runtime_error("OverlayedField::add: Attempt to add to an invalid object.");
  }
  throw runtime_error("OverlayedField::add: Attempt to add an invalid field.");
}

/// Returns the 3 electric field components (x, y, z).
void OverlayedField::combinedElectric(const double* pos, double* field) const {
  field[0] = field[1] = field[2] = 0.;
  calculate_combined_field(data<Object>()->electric_components, pos, field);
}

/// Returns the 3  magnetic field components (x, y, z).
void OverlayedField::combinedMagnetic(const double* pos, double* field) const {
  field[0] = field[1] = field[2] = 0.;
  calculate_combined_field(data<Object>()->magnetic_components, pos, field);
}

/// Returns the 3 electric (val[0]-val[2]) and magnetic field components (val[3]-val[5]).
void OverlayedField::electromagneticField(const double* pos, double* field) const {
  auto* o = data<Object>();
  field[0] = field[1] = field[2] = 0.;
  calculate_combined_field(o->electric_components, pos, field);
  calculate_combined_field(o->magnetic_components, pos, field + 3);
}
