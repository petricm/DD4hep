//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : F.Gaede
//
//==========================================================================
#include "DDRec/SurfaceManager.h"

#include "DDRec/SurfaceHelper.h"
#include "DD4hep/VolumeManager.h"
#include "DD4hep/Detector.h"

#include <sstream>

namespace dd4hep {
  
  using namespace detail ;

  namespace rec {
    

    SurfaceManager::SurfaceManager(Detector& theDetector){

      // have to make sure the volume manager is populated once in order to have
      // the volumeIDs attached to the DetElements

      VolumeManager::getVolumeManager(theDetector);

      initialize(theDetector) ;
    }
    
    SurfaceManager::~SurfaceManager(){
      // nothing to do
    }
    
    
    const SurfaceMap* SurfaceManager::map( const std::string name ) const {

      auto it = _map.find( name ) ;

      if( it != _map.end() ){

        return & it->second ;
      }

      return 0 ;
    }

    void SurfaceManager::initialize(Detector& description) {
      
      const std::vector<std::string>& types = description.detectorTypes() ;

      for(const auto & type : types){

        const std::vector<DetElement>& dets = description.detectors( type ) ;

        for(auto det : dets){

          std::string name = det.name() ;

          SurfaceHelper surfH( det ) ;
	  
          const SurfaceList& detSL = surfH.surfaceList() ;
  
          // add an empty map for this detector in case there are no surfaces attached 
          _map.emplace(name , SurfaceMap());

          for(auto surf : detSL){
            // enter surface into map for this detector
            _map[ name ].emplace(surf->id(), surf );

            // enter surface into map for detector type
            _map[ type ].emplace(surf->id(), surf );

            // enter surface into world map 
            _map[ "world" ].emplace(surf->id(), surf );

          }
        }
      }

    }

    std::string SurfaceManager::toString() const {
      
      std::stringstream sstr ;
       
      sstr << "--------  SurfaceManager contains the following maps : --------- " << std::endl ;
 
      for(const auto & mi : _map) {
	
        sstr << "  key: " <<  mi.first << " \t number of surfaces : " << mi.second.size() << std::endl ;
      }
      sstr << "---------------------------------------------------------------- " << std::endl ;

      return sstr.str() ;
    }


  } // namespace
}// namespace
