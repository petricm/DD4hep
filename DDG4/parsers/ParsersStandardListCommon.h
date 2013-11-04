#ifndef PARSERS_STANDARD_LIST_COMMON_H
#define PARSERS_STANDARD_LIST_COMMON_H 1
// ============================================================================
// Include files
// ============================================================================
#include "DDG4/Parsers.h"
#include "ParsersFactory.h"
// ============================================================================
#define PARSERS_DEF_FOR_LIST(InnerType)                                                  \
  int DD4hep::Parsers::parse(std::vector<InnerType>& result, const std::string& input) { \
    return DD4hep::Parsers::parse_(result, input);                                       \
  }                                                                                      \
  int DD4hep::Parsers::parse(std::list<InnerType>& result, const std::string& input) {   \
    return DD4hep::Parsers::parse_(result, input);                                       \
  }                                                                                      \
  int DD4hep::Parsers::parse(std::set<InnerType>& result, const std::string& input) {    \
    return DD4hep::Parsers::parse_(result, input);                                       \
  }

// ============================================================================
#endif /* PARSERS_STANDARD_LIST_COMMON_H */
