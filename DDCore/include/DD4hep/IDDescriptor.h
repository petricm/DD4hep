// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_IDDESCRIPTOR_H
#define DD4hep_IDDESCRIPTOR_H

// Framework include files
#include "DD4hep/Handle.h"

// C++ include files
#include <map>
#include <string>
#include <vector>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry sub-namespace declaration
   */
  namespace Geometry {

    /** @class IDDescriptor IDDescriptor.h DDCore/IDDescriptor.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     *  @date    2012/07/31
     */
    struct IDDescriptor : public Ref_t {
    public:
      typedef std::pair<int, int>                        Field;
      typedef std::vector<std::pair<std::string, Field>> FieldMap;
      typedef std::vector<std::pair<int, std::string>>   FieldIDs;
      struct Object {
        FieldMap fieldMap;
        FieldIDs fieldIDs;
        int      maxBit;
        Object() : maxBit(0) {}
      };

    public:
      /// Default constructor
      IDDescriptor() : Ref_t() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> IDDescriptor(const Handle<Q>& e) : Ref_t(e) {}

      /// Initializing constructor
      IDDescriptor(const std::string& description);
      int             maxBit() const;
      const FieldIDs& ids() const;
      const FieldMap& fields() const;
    };
  } /* End namespace Geometry    */
} /* End namespace DD4hep      */
#endif /* DD4hep_IDDESCRIPTOR_H     */
