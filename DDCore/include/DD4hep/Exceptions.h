// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDCORE_EXCEPTIONS_H
#define DD4HEP_DDCORE_EXCEPTIONS_H

// C/C++ include files
#include <stdexcept>
#include <string>
#include <typeinfo>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class unrelated_type_error
   *
   *   @author  M.Frank
   *   @date    13.08.2013
   */
  struct unrelated_type_error : public std::runtime_error {
    static std::string msg(const std::type_info& typ1, const std::type_info& typ2, const std::string& text);
    unrelated_type_error(const std::type_info& typ1, const std::type_info& typ2, const std::string& text = "")
        : std::runtime_error(msg(typ1, typ2, text)) {}
  };

  /** @class unrelated_value_error
   *
   *   @author  M.Frank
   *   @date    13.08.2013
   */
  struct unrelated_value_error : public std::runtime_error {
    static std::string msg(const std::type_info& typ, const std::string& text);
    unrelated_value_error(const std::type_info& typ, const std::string& text = "") : std::runtime_error(msg(typ, text)) {}
  };

}  // End namespace DD4hep

#endif /* DD4HEP_DDCORE_EXCEPTIONS_H */
