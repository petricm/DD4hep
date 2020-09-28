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

#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"
#include "DD4hep/detail/Handle.inl"
#include "Evaluator/Evaluator.h"
#include <iostream>
#include <iomanip>
#include <climits>
#include <cstring>
#include <cstdio>

#if !defined(WIN32) && !defined(__ICC)
#include "cxxabi.h"
#endif

namespace dd4hep {
  auto evaluator() -> dd4hep::tools::Evaluator&;
}

namespace {
  dd4hep::tools::Evaluator& eval(dd4hep::evaluator());
}

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

namespace   {
  void check_evaluation(const string& value, int status)   {
    if (status != tools::Evaluator::OK) {
      stringstream str;
      eval.print_error(str);
      throw runtime_error("dd4hep: "+str.str()+" : value="+value+" [Evaluation error]");
    }
  }
}

namespace dd4hep  {
  
  auto _toShort(const string& value) -> short {
    string s(value);
    size_t idx = s.find("(int)");
    if (idx != string::npos)
      s.erase(idx, 5);
    while (s[0] == ' ')
      s.erase(0, 1);
    double result = eval.evaluate(s.c_str());
    check_evaluation(value, eval.status());
    return (short) result;
  }

  auto _toInt(const string& value) -> int {
    string s(value);
    size_t idx = s.find("(int)");
    if (idx != string::npos)
      s.erase(idx, 5);
    while (s[0] == ' ')
      s.erase(0, 1);
    double result = eval.evaluate(s.c_str());
    check_evaluation(value, eval.status());
    return (int) result;
  }

  auto _toLong(const string& value) -> long {
    string s(value);
    size_t idx = s.find("(int)");
    if (idx != string::npos)
      s.erase(idx, 5);
    while (s[0] == ' ')
      s.erase(0, 1);
    double result = eval.evaluate(s.c_str());
    check_evaluation(value, eval.status());
    return (long) result;
  }

  auto _toBool(const string& value) -> bool {
    return value == "true" || value == "yes";
  }

  /// String conversions: string to float value
  auto _toFloat(const string& value) -> float {
    double result = eval.evaluate(value.c_str());
    check_evaluation(value, eval.status());
    return (float) result;
  }

  /// String conversions: string to double value
  auto _toDouble(const string& value) -> double {
    double result = eval.evaluate(value.c_str());
    check_evaluation(value, eval.status());
    return result;
  }

  /// Generic type conversion from string to primitive value  \ingroup DD4HEP_CORE
  template <typename T> auto _toType(const string& value) -> T    {
    notImplemented("Value "+value+" cannot be converted to type "+typeName(typeid(T)));
    return T();
  }

  /// Generic type conversion from string to primitive value
  template <> auto _toType<bool>(const string& value) -> bool  {
    return _toBool(value);
  }

  /// Generic type conversion from string to primitive value
  template <> auto _toType<short>(const string& value) -> short  {
    return _toShort(value);
  }

  /// Generic type conversion from string to primitive value
  template <> auto _toType<unsigned short>(const string& value) -> unsigned short  {
    return (unsigned short)_toShort(value);
  }

  /// Generic type conversion from string to primitive value
  template <> auto _toType<int>(const string& value) -> int  {
    return _toInt(value);
  }

  /// Generic type conversion from string to primitive value
  template <> auto _toType<unsigned int>(const string& value) -> unsigned int  {
    return (unsigned int)_toInt(value);
  }

  /// Generic type conversion from string to primitive value
  template <> auto _toType<long>(const string& value) -> long  {
    return _toLong(value);
  }

  /// Generic type conversion from string to primitive value
  template <> auto _toType<unsigned long>(const string& value) -> unsigned long  {
    return (unsigned long)_toLong(value);
  }

  /// Generic type conversion from string to primitive value
  template <> auto _toType<float>(const string& value) -> float  {
    return _toFloat(value);
  }

  /// Generic type conversion from string to primitive value
  template <> auto _toType<double>(const string& value) -> double  {
    return _toDouble(value);
  }

  /// Generic type conversion from string to primitive value
  template <> auto _toType<string>(const string& value) -> string  {
    return value;
  }

  template <> auto _multiply<char>(const string& left, const string& right) -> char {
    double val = _toDouble(left + "*" + right);
    if ( val >= double(SCHAR_MIN) && val <= double(SCHAR_MAX) )
      return (char) (int)val;
    except("_multiply<char>",
           "Multiplication %e = %s * %s out of bounds for conversion to char.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> auto _multiply<unsigned char>(const string& left, const string& right) -> unsigned char {
    double val = _toDouble(left + "*" + right);
    if ( val >= 0 && val <= double(UCHAR_MAX) )
      return (unsigned char) (int)val;
    except("_multiply<char>",
           "Multiplication %e = %s * %s out of bounds for conversion to unsigned char.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> auto _multiply<short>(const string& left, const string& right) -> short {
    double val = _toDouble(left + "*" + right);
    if ( val >= double(SHRT_MIN) && val <= double(SHRT_MAX) )
      return (short) val;
    except("_multiply<char>",
           "Multiplication %e = %s * %s out of bounds for conversion to short.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> auto _multiply<unsigned short>(const string& left, const string& right) -> unsigned short {
    double val = _toDouble(left + "*" + right);
    if ( val >= 0 && val <= double(USHRT_MAX) )
      return (unsigned short)val;
    except("_multiply<char>",
           "Multiplication %e = %s * %s out of bounds for conversion to unsigned short.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> auto _multiply<int>(const string& left, const string& right) -> int {
    return (int) _toDouble(left + "*" + right);
  }

  template <> auto _multiply<unsigned int>(const string& left, const string& right) -> unsigned int {
    return (unsigned int) _toDouble(left + "*" + right);
  }

  template <> auto _multiply<long>(const string& left, const string& right) -> long {
    return (long) _toDouble(left + "*" + right);
  }

  template <> auto _multiply<unsigned long>(const string& left, const string& right) -> unsigned long {
    return (unsigned long) _toDouble(left + "*" + right);
  }

  template <> auto _multiply<float>(const string& left, const string& right) -> float {
    return _toFloat(left + "*" + right);
  }

  template <> auto _multiply<double>(const string& left, const string& right) -> double {
    return _toDouble(left + "*" + right);
  }

  void _toDictionary(const string& name, const string& value) {
    _toDictionary(name, value, "number");
  }

  /// Enter name value pair to the dictionary.  \ingroup DD4HEP_CORE
  void _toDictionary(const string& name, const string& value, const string& typ)   {
    if ( typ == "string" )  {
      eval.setEnviron(name.c_str(),value.c_str());
      return;
    }
    else  {
      string n = name, v = value;
      size_t idx = v.find("(int)");
      if (idx != string::npos)
        v.erase(idx, 5);
      idx = v.find("(float)");
      if (idx != string::npos)
        v.erase(idx, 7);
      while (v[0] == ' ')
        v.erase(0, 1);
      double result = eval.evaluate(v.c_str());
      check_evaluation(v, eval.status());
      eval.setVariable(n.c_str(), result);
    }
  }

  /// String manipulations: Remove unconditionally all white spaces
  auto remove_whitespace(const string& v) -> string    {
    string value;
    value.reserve(v.length()+1);
    for(const char* p = v.c_str(); *p; ++p)   {
      if ( !::isspace(*p) ) value += *p;
    }
    return value;
  }

  template <typename T> static inline auto __to_string(T value, const char* fmt) -> string {
    char text[128];
    ::snprintf(text, sizeof(text), fmt, value);
    return text;
  }

  auto _toString(bool value) -> string {
    return value ? "true" : "false";
  }

  auto _toString(short value, const char* fmt) -> string {
    return __to_string((int)value, fmt);
  }

  auto _toString(int value, const char* fmt) -> string {
    return __to_string(value, fmt);
  }

  auto _toString(float value, const char* fmt) -> string {
    return __to_string(value, fmt);
  }

  auto _toString(double value, const char* fmt) -> string {
    return __to_string(value, fmt);
  }

  auto _ptrToString(const void* value, const char* fmt) -> string {
    return __to_string(value, fmt);
  }

  // ==================================================================================
  static long s_numVerifies = 0;

  auto num_object_validations() -> long {
    return s_numVerifies;
  }
  void increment_object_validations() {
    ++s_numVerifies;
  }
  void warning_deprecated_xml_factory(const char* name)   {
    const char* edge = "++++++++++++++++++++++++++++++++++++++++++";
    size_t len = ::strlen(name);
    cerr << edge << edge << edge << endl;
    cerr << "++  The usage of the factory: \"" << name << "\" is DEPRECATED due to naming conventions."
         << setw(53-len) << right << "++" << endl;
    cerr << "++  Please use \"DD4hep_" << name << "\" instead." << setw(93-len) << right << "++" << endl;
    cerr << edge << edge << edge << endl;
  }
}

#include "DDSegmentation/Segmentation.h"
typedef DDSegmentation::Segmentation _Segmentation;
namespace dd4hep {
  template <> void Handle<_Segmentation>::assign(_Segmentation* s, const string& n, const string&) {
    this->m_element = s;
    s->setName(n);
  }
  template <> auto Handle<_Segmentation>::name() const -> const char* {
    return this->m_element ? this->m_element->name().c_str() : "";
  }
  template class dd4hep::Handle<_Segmentation>;
}

#include "DD4hep/Detector.h"
#include "TMap.h"
#include "TColor.h"


template class dd4hep::Handle<NamedObject>;
DD4HEP_SAFE_CAST_IMPLEMENTATION(NamedObject,NamedObject)

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(Detector);
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TObject,NamedObject);
DD4HEP_INSTANTIATE_HANDLE_CODE(NONE,TNamed,TObject,NamedObject);

#include "TGeoMedium.h"
#include "TGeoMaterial.h"
#include "TGeoElement.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoElement);
DD4HEP_INSTANTIATE_HANDLE(TGeoMaterial);
DD4HEP_INSTANTIATE_HANDLE(TGeoMedium);

#include "TGeoMatrix.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoMatrix);
DD4HEP_INSTANTIATE_HANDLE(TGeoRotation);
DD4HEP_INSTANTIATE_HANDLE(TGeoTranslation);
DD4HEP_INSTANTIATE_HANDLE(TGeoIdentity);
DD4HEP_INSTANTIATE_HANDLE(TGeoCombiTrans);
DD4HEP_INSTANTIATE_HANDLE(TGeoGenTrans);

#include "TGeoNode.h"
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TAtt3D);
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TAttLine);
DD4HEP_INSTANTIATE_HANDLE(TGeoNode,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE(TGeoNodeMatrix);
DD4HEP_INSTANTIATE_HANDLE(TGeoNodeOffset);

// Shapes (needed by "Shapes.cpp")
#include "TGeoBBox.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TGeoCone.h"
#include "TGeoArb8.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoParaboloid.h"
#include "TGeoSphere.h"
#include "TGeoTorus.h"
#include "TGeoBoolNode.h"
#include "TGeoVolume.h"
#include "TGeoCompositeShape.h"
#include "TGeoShapeAssembly.h"
#include "DD4hep/detail/ShapesInterna.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoVolumeAssembly,TGeoVolume,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE(TGeoVolumeMulti,TGeoVolume,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE(TGeoVolume,TGeoAtt,TAttLine,TAtt3D);
DD4HEP_INSTANTIATE_HANDLE(TGeoShape);
DD4HEP_INSTANTIATE_HANDLE(TGeoBBox,TGeoShape);

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoCone);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoArb8);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoConeSeg);
//DD4HEP_INSTANTIATE_SHAPE_HANDLE(MyConeSeg);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoParaboloid);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoPcon);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoPgon,TGeoPcon);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoXtru);

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoHype,TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoEltu,TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTubeSeg,TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoCtub,TGeoTubeSeg,TGeoTube);
using dd4hep::TwistedTubeObject;
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TwistedTubeObject,TGeoTubeSeg);

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTrap,TGeoArb8);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoGtra,TGeoArb8);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTrd1);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTrd2);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoSphere);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTorus);

#if ROOT_VERSION_CODE > ROOT_VERSION(6,21,0)
#include "TGeoTessellated.h"
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTessellated);
#endif

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoHalfSpace);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoShapeAssembly);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoCompositeShape);

// Volume Placements (needed by "Volumes.cpp")
#include "TGeoPhysicalNode.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoPhysicalNode);

#include "TGeoBoolNode.h"
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoUnion);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoIntersection);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoSubtraction);

// Replicated Volumes (needed by "Volumes.cpp")
#include "TGeoPatternFinder.h"
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternFinder);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternX);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternY);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternZ);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternParaX);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternParaY);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternParaZ);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternTrapZ);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternCylR);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternCylPhi);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternSphR);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternSphTheta);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternSphPhi);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternHoneycomb);
