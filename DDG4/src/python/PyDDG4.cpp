//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author Markus Frank
//  \date   2015-11-03
//
//==========================================================================

// Framework include files
#include "DD4hep/Detector.h"
#include "DDG4/Python/PyDDG4.h"
#include "DDG4/Python/DDPython.h"
#include "DDG4/Geant4Kernel.h"

auto PyDDG4::run(Kernel& kernel) -> int  {
  int ret;
  dd4hep::DDPython::AllowThreads allow(0);
  if ( 1 != (ret=kernel.configure()) )
    return ret;
  else if ( 1 != (ret=kernel.initialize()) )
    return ret;
  else if ( 1 != (ret=kernel.run()) )
    return ret;
  else if ( 1 != (ret=kernel.terminate()) )
    return ret;
    //dd4hep::DDPython::shutdown();
  return ret;
}

auto PyDDG4::execute() -> int  {
  Kernel& k = Kernel::instance(dd4hep::Detector::getInstance());
  return run(k);
}

auto PyDDG4::process(const char* fname) -> int  {
  return dd4hep::DDPython::instance().runFile(fname);
}

auto PyDDG4::run(const char* fname) -> int  {
  int ret = dd4hep::DDPython::instance().runFile(fname);
  if ( 1 != ret ) return ret;
  return execute();
}
