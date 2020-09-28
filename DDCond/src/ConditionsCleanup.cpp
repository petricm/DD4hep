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
#include "DDCond/ConditionsCleanup.h"

using namespace dd4hep::cond;

/// Request cleanup operation of IOV POOL
auto ConditionsCleanup::operator()(const ConditionsIOVPool & /* iov_pool */) const -> bool
{
  return false;
}

/// Request cleanup operation of regular conditiions pool
auto ConditionsCleanup::operator()(const ConditionsPool & /* iov_pool */) const -> bool
{
  return true;
}

/// Request cleanup operation of IOV POOL
auto ConditionsFullCleanup::operator()(const ConditionsIOVPool & /* iov_pool */) const -> bool
{
  return true;
}

/// Request cleanup operation of regular conditiions pool
auto ConditionsFullCleanup::operator()(const ConditionsPool & /* iov_pool */) const -> bool
{
  return true;
}
