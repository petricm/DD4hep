//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author  Markus Frank
//  \date    2016-02-02
//  \version 1.0
//
//==========================================================================
#ifndef DD4HEP_CONDITIONS_XMLCONDITONSLOADER_H
#define DD4HEP_CONDITIONS_XMLCONDITONSLOADER_H

// Framework include files
#include "DDCond/ConditionsDataLoader.h"
#include "DD4hep/Printout.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep::cond  {

    /// Implementation of a stack of conditions assembled before application
    /** 
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_CONDITIONS
     */
    class ConditionsXmlLoader : public ConditionsDataLoader   {
      typedef std::vector<Condition> Buffer;
      Buffer m_buffer;

      size_t load_source  (const std::string& nam,
                           key_type key,
                           const IOV& req_validity,
                           RangeConditions& conditions);
    public:
      /// Default constructor
      ConditionsXmlLoader(Detector& description, ConditionsManager mgr, const std::string& nam);
      /// Default destructor
      ~ConditionsXmlLoader() override;
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_single(key_type key,
                                 const IOV& req_validity,
                                 RangeConditions& conditions);
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_range( key_type key,
                                 const IOV& req_validity,
                                 RangeConditions& conditions);
      /// Optimized update using conditions slice data
      size_t load_many(  const IOV& /* req_validity */,
                                 RequiredItems&  /* work         */,
                                 LoadedItems&    /* loaded       */,
                                 IOV&       /* conditions_validity */) override
      {
        except("ConditionsLoader","+++ update: Invalid call!");
        return 0;
      }
    };
  }      /* End namespace dd4hep                    */
#endif /* DD4HEP_CONDITIONS_XMLCONDITONSLOADER_H  */

//#include "ConditionsXmlLoader.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/PluginCreators.h"
#include "DD4hep/detail/ConditionsInterna.h"

#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"
#include "DDCond/ConditionsEntry.h"

// C/C++ include files
#include <string>

// Forward declartions
using std::string;
using namespace dd4hep;
using namespace dd4hep::cond;

namespace {
  void* create_loader(Detector& description, int argc, char** argv)   {
    const char* name = argc>0 ? argv[0] : "XMLLoader";
    auto* mgr = (ConditionsManagerObject*)(argc>0 ? argv[1] : nullptr);
    return new ConditionsXmlLoader(description,ConditionsManager(mgr),name);
  }
}
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_Conditions_xml_Loader,create_loader)

/// Standard constructor, initializes variables
ConditionsXmlLoader::ConditionsXmlLoader(Detector& description, ConditionsManager mgr, const std::string& nam) 
: ConditionsDataLoader(description, mgr, nam)
{
}

/// Default Destructor
ConditionsXmlLoader::~ConditionsXmlLoader() = default;

size_t ConditionsXmlLoader::load_source(const std::string& nam,
                                        key_type key,
                                        const IOV& req_validity,
                                        RangeConditions& conditions)
{
  size_t len = conditions.size();
  string fac = "XMLConditionsParser";
  xml::DocumentHolder doc(xml::DocumentHandler().load(nam));
  xml::Handle_t       handle = doc.root();
  ConditionsStack     stack;
  char* argv[] = { (char*)handle.ptr(), (char*)&stack, nullptr};
  void* result = dd4hep::createPlugin(fac, m_detector, 2, argv, nullptr);
  if ( result == &m_detector )  { // All OK.
    for (auto e : stack)  {
      Condition condition;/// = queueUpdate(e);
      except("ConditionsXmlLoader","Fix me: queueUpdate(e) not implemented");
      delete e;
      if ( condition.isValid() )   {
        if ( key == condition->hash )  {
          if ( req_validity.contains(condition.iov()) )   {
            conditions.emplace_back(condition);
            continue;
          }
        }
        m_buffer.emplace_back(condition);
      }
    }
  }
  m_sources.erase(m_sources.begin());
  stack.clear();
  return conditions.size()-len;
}

size_t ConditionsXmlLoader::load_single(key_type key,
                                        const IOV& req_validity,
                                        RangeConditions& conditions)
{
  size_t len = conditions.size();
  if ( m_buffer.empty() && !m_sources.empty() )  {
    return load_source(m_sources.begin()->first, key, req_validity, conditions);
  }
  for (auto j=m_buffer.begin(); j!=m_buffer.end(); ++j)  {
    Condition condition = *j;
    const IOV* iov = condition->iov;
    if ( IOV::partial_match(req_validity,*iov) )  {
      if ( key == condition->hash )  {
        conditions.emplace_back(condition);
        m_buffer.erase(j);
        return conditions.size()-len;
      }
    }
  }
  return conditions.size()-len;
}

size_t ConditionsXmlLoader::load_range(key_type key,
                                       const IOV& req_validity,
                                       RangeConditions& conditions)
{
  size_t len = conditions.size();
  while ( !m_sources.empty() )  {
    load_source(m_sources.begin()->first, key, req_validity, conditions);
  }
  std::vector<Condition> keep;
  for (auto condition : m_buffer)  {
    const IOV* iov = condition->iov;
    if ( IOV::partial_match(req_validity,*iov) )  {
      if ( key == condition->hash )  {
        conditions.emplace_back(condition);
      }
    }
    keep.emplace_back(condition);
  }
  m_buffer = keep;
  return conditions.size()-len;
}

