//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
//==========================================================================
// Include files
//==========================================================================
#ifndef DD4HEP_PARSERS_NO_ROOT
#include "Parsers/spirit/ParsersStandardListCommon.h"

namespace ROOT::Math {
    auto operator<(const XYZVector& a, const XYZVector& b) -> bool  {
      if ( a.X() < b.X() ) return true;
      if ( a.Y() < b.Y() ) return true;
      if ( a.Z() < b.Z() ) return true;
      return false;
    }
  }

template struct std::less<ROOT::Math::XYZVector>;

// ============================================================================
namespace dd4hep::Parsers {

    // ==========================================================================
    /*  parse 3D-vector
     *  @param result (output) the parsed vector
     *  @param input  (input)  the input string
     *  @return status code
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    template <> auto parse(ROOT::Math::XYZVector& result,const std::string& input) -> int {
      ROOT::Math::XYZPoint point;
      int sc = parse(point,input);
      if ( 0 == sc ){ return sc; }                               // RETURN
      result = point;
      return 1;
    }

    // ==========================================================================
    /*  parse the vector of vectors
     *  @param resut (OUTPUT) the parser vector
     *  @param input (INPIUT) the string to be parsed
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    // ==========================================================================
    IMPLEMENT_STL_PARSERS(ROOT::Math::XYZVector)
    IMPLEMENT_MAPPED_PARSERS(pair,ROOT::Math::XYZVector)
  }
#endif
