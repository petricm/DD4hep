// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/ToStream.h"
#include "XML/Evaluator.h"

// C/C++ include files
#include <stdexcept>

namespace DD4hep {
  XmlTools::Evaluator& g4Evaluator();
}
namespace {
  XmlTools::Evaluator& eval(DD4hep::g4Evaluator());
}

//==============================================================================
namespace DD4hep {  namespace Parsers {
    template <typename T> T evaluate_string(const std::string& /* value */)   {
      throw "Bad undefined call";
    }

    template <> double evaluate_string<double>(const std::string& value)   {
      double result = eval.evaluate(value.c_str());
      if (eval.status() != XmlTools::Evaluator::OK) {
        std::cerr << value << ": ";
        eval.print_error();
        throw std::runtime_error("DD4hep::Properties: Severe error during expression evaluation of " + value);
      }
      return result;
    }
    template <> float evaluate_string<float>(const std::string& value)   {
      double result = eval.evaluate(value.c_str());
      if (eval.status() != XmlTools::Evaluator::OK) {
        std::cerr << value << ": ";
        eval.print_error();
        throw std::runtime_error("DD4hep::Properties: Severe error during expression evaluation of " + value);
      }
      return (float) result;
    }
  }
}

// ============================================================================
// print XYZ-point
std::ostream& DD4hep::Utils::toStream(const ROOT::Math::XYZPoint&  obj, std::ostream& s)  {
  s << "( ";
  toStream(obj.X () , s );
  s << " , ";
  toStream(obj.Y () , s );
  s << " , ";
  toStream(obj.Z () , s );
  s << " )";
  return s;
}
// ============================================================================
// print XYZ-vector
std::ostream& DD4hep::Utils::toStream(const ROOT::Math::XYZVector& obj, std::ostream& s)  {
  s << "( ";
  toStream(obj.X () , s );
  s << " , ";
  toStream(obj.Y () , s );
  s << " , ";
  toStream(obj.Z () , s );
  s << " )";
  return s;
}
// ============================================================================
// print LorentzVector
std::ostream& DD4hep::Utils::toStream(const ROOT::Math::PxPyPzEVector& obj, std::ostream& s){
  s << "( ";
  toStream(obj.Px () , s , 12 );
  s << " , ";
  toStream(obj.Py () , s , 12 );
  s << " , ";
  toStream(obj.Pz () , s , 13 );
  s << " , ";
  toStream(obj.E  () , s , 14 );
  s << " )";
  return s;
}
