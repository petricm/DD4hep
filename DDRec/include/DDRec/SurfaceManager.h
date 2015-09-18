#ifndef DDRec_SurfaceManager_H_
#define DDRec_SurfaceManager_H_

#include <map>
#include <string>
#include "DDSurfaces/ISurface.h"

namespace DD4hep {
  namespace DDRec {

    /// typedef for surface maps, keyed by the cellID
    typedef std::multimap<unsigned long, DDSurfaces::ISurface*> SurfaceMap;

    /** Surface manager class that holds maps of surfaces for all known 
     *  sensitive detector types and  individual sub detectors. 
     *  Maps can be retrieved via detector name.
     * 
     * @author F.Gaede, DESY
     * @date May, 11 2015
     * @version $Id$
     */
    class SurfaceManager {
      typedef std::map<std::string, SurfaceMap> SurfaceMapsMap;

    public:
      SurfaceManager();

      ~SurfaceManager();

      /** Get the maps of all surfaces associated to the given detector or
       *  type of detectors, e.g. map("tracker") returns a map with all surfaces
       *  assigned to tracking detectors. Returns 0 if no map exists.
       */
      const SurfaceMap* map(const std::string name) const;

      ///create a string with all available maps and their size (number of surfaces)
      std::string toString() const;

    protected:
      /// initialize all known surface maps
      void initialize();

      SurfaceMapsMap _map;
    };

  } /* namespace DDRec */
} /* namespace DD4hep */

#endif  // DDRec_SurfaceManager_H_
