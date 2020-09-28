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
#include "DD4hep/DetectorSelector.h"
#include "DD4hep/Detector.h"

using namespace std;
using namespace dd4hep;

/// Access a set of subdetectors according to the sensitive type.
auto
DetectorSelector::detectors(const string& type) -> const DetectorSelector::Result&
{
  return description.detectors(type);
}

/// Access a set of subdetectors according to several sensitive types.
auto
DetectorSelector::detectors(const string& type1,
                            const string& type2,
                            const string& type3,
                            const string& type4,
                            const string& type5) -> DetectorSelector::Result  {
  const string* types[] = { &type1, &type2, &type3, &type4, &type5 };
  Result result;
  for(auto & type : types)  {
    try  {
      if ( !type->empty() )  {
        const vector<DetElement>& v = description.detectors(*type);
        result.insert(end(result),begin(v),end(v));
      }
    }
    catch(...)   {}
  }
  return result;
}

/** return a vector with all detectors that have all the type properties in
 *  includeFlag set but none of the properties given in excludeFlag
 */
auto
DetectorSelector::detectors(unsigned int includeFlag, unsigned int excludeFlag ) const -> DetectorSelector::Result  {
  Result result;
  const Detector::HandleMap& entries = description.detectors();
  result.reserve( entries.size() ) ;
  description.detectors(""); // Just to ensure the geometry is closed....
  for(const auto& i : entries )  {
    DetElement det(i.second);
    if ( det.parent().isValid() )  { // Exclude 'world'
      //fixme: what to do with compounds - add their daughters  ?
      // ...
      if( ( det.typeFlag() &  includeFlag ) == includeFlag &&
          ( det.typeFlag() &  excludeFlag ) ==  0 )
        result.emplace_back( det ) ;
    }
  }
  return result;
}
