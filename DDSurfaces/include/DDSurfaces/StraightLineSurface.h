/*
 * StraightLineSurface.h
 *
 *  Created on: Mar 7, 2014
 *      Author: cgrefe
 */

#ifndef DDSurfaces_STRAIGHTLINESURFACE_H_
#define DDSurfaces_STRAIGHTLINESURFACE_H_

#include "DDSurfaces/ISurface.h"

namespace DDSurfaces {

  struct StraightLineBoundary {
    ;
  };

  class StraightLineSurface : public ISurface {
  public:
    /// Default constructor
    StraightLineSurface();

    /// Destructor
    virtual ~StraightLineSurface();

    /// Checks if the given point lies within the surface
    virtual bool isInsideBoundaries(const Vector3D& point) const;

    /// Access to the normal direction at the given point
    virtual Vector3D getNormal(const Vector3D& point) const;

    /// Access to the measurement directions at the given point
    virtual Measurement measurement(const Vector3D& point) const;

  protected:
    StraightLineBoundary m_boundary;
  };

} /* namespace DDSurfaces */

#endif /* DDSurfaces_STRAIGHTLINESURFACE_H_ */
