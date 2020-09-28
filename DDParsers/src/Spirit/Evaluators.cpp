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

#include "Parsers/config.h"

// Framework include files
#if defined(DD4HEP_PARSER_HEADER)

#define DD4HEP_NEED_EVALUATOR
// This is the case, if the parsers are externalized
// and the dd4hep namespace is renamed!
#include DD4HEP_PARSER_HEADER

#else

// Standard dd4hep parser handling
#include "Parsers/spirit/ToStream.h"

#endif
#include "Evaluator/Evaluator.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>

namespace dd4hep {
  auto g4Evaluator() -> dd4hep::tools::Evaluator&;
}
namespace {
  dd4hep::tools::Evaluator& eval(dd4hep::g4Evaluator());
}

//==============================================================================
namespace dd4hep::Parsers {
    template <typename T> auto evaluate_string(const std::string& /* value */) -> T   {
      throw "Bad undefined call";
    }

    template <> auto evaluate_string<double>(const std::string& value) -> double   {
      double result = eval.evaluate(value.c_str());
      if (eval.status() != tools::Evaluator::OK) {
        std::cerr << value << ": ";
        eval.print_error();
        throw std::runtime_error("dd4hep::Properties: Severe error during expression evaluation of " + value);
      }
      return result;
    }
    template <> auto evaluate_string<float>(const std::string& value) -> float   {
      double result = eval.evaluate(value.c_str());
      if (eval.status() != tools::Evaluator::OK) {
        std::cerr << value << ": ";
        eval.print_error();
        throw std::runtime_error("dd4hep::Properties: Severe error during expression evaluation of " + value);
      }
      return (float) result;
    }
  }
