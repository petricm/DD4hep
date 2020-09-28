/*
 * MultiSegmentation.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/MultiSegmentation.h"
#include <iomanip>
#include <stdexcept>
#include <iostream>

using namespace std;

namespace dd4hep::DDSegmentation {

    /// default constructor using an encoding string
    MultiSegmentation::MultiSegmentation(const string& cellEncoding)
      :	Segmentation(cellEncoding), m_discriminator(nullptr), m_debug(0)
    {
      // define type and description
      _type        = "MultiSegmentation";
      _description = "Multi-segmenation wrapper segmentation";
      //registerParameter<int>("debug", "Debug flag", m_debug, 0);
      registerParameter<string>("key",   "Diskriminating field", m_discriminatorId, "");
    }

    /// Default constructor used by derived classes passing an existing decoder
    MultiSegmentation::MultiSegmentation(const BitFieldCoder* decode)
      :	Segmentation(decode), m_discriminator(nullptr), m_debug(0)
    {
      // define type and description
      _type        = "MultiSegmentation";
      _description = "Multi-segmenation wrapper segmentation";
      //registerParameter<int>("debug", "Debug flag", m_debug, 0);
      registerParameter<string>("key",   "Diskriminating field", m_discriminatorId, "");
    }

    /// destructor
    MultiSegmentation::~MultiSegmentation() {
      for(auto & m_segmentation : m_segmentations)
        delete m_segmentation.segmentation;
      m_segmentations.clear();
    }

    /// Add subsegmentation. Call only valid for Multi-segmentations. Default implementation throws an exception
    void MultiSegmentation::addSubsegmentation(long key_min, long key_max, Segmentation* entry)    {
      Entry e;
      e.key_min = key_min;
      e.key_max = key_max;
      e.segmentation = entry;
      m_segmentations.emplace_back(e);
    }

    /// Set the underlying decoder
    void MultiSegmentation::setDecoder(const BitFieldCoder* newDecoder) {
      this->Segmentation::setDecoder(newDecoder);
      for(auto & m_segmentation : m_segmentations)
        m_segmentation.segmentation->setDecoder(newDecoder);
      m_discriminator = &((*_decoder)[m_discriminatorId]);
    }

    /// 
    auto MultiSegmentation::subsegmentation(const CellID& cID)   const -> const Segmentation&  {
      if ( m_discriminator )  {
        long seg_id = m_discriminator->value(cID);
        for(const auto & e : m_segmentations)  {
          if ( e.key_min<= seg_id && e.key_max >= seg_id )   {
            Segmentation* s = e.segmentation;
            if ( m_debug > 0 )   {
              cout << "MultiSegmentation: id:" << setw(4) << hex << seg_id << dec << "  " << s->name();
              const Parameters& pars = s->parameters();
              for(auto par : pars)  {
                cout << " " << par->name() << "=" << par->value();
              }
              cout << endl;
            }
            return *s;
          }
        }
      }
      throw runtime_error("MultiSegmentation: Invalid sub-segmentation identifier!");;
    }
     
    /// determine the position based on the cell ID
    auto MultiSegmentation::position(const CellID& cID) const -> Vector3D {
      return subsegmentation(cID).position(cID);
    }

    /// determine the cell ID based on the position
    auto MultiSegmentation::cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& vID) const -> CellID {
      return subsegmentation(vID).cellID(localPosition, globalPosition, vID);
    }

    auto MultiSegmentation::cellDimensions(const CellID& cID) const -> vector<double> {
      return subsegmentation(cID).cellDimensions(cID);
    }

    REGISTER_SEGMENTATION(MultiSegmentation)

  } /* namespace dd4hep */
