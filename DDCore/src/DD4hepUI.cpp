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

// Framework includes
#include "DD4hep/DD4hepUI.h"
#include "DD4hep/Printout.h"
#include "TRint.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

namespace {
  auto _visLevel(int lvl) -> string    {
    char text[32];
    ::snprintf(text,sizeof(text),"%d",lvl);
    return text;
  }
}

/// Default constructor
DD4hepUI::DD4hepUI(Detector& instance) : m_detDesc(instance)  {
}

/// Default destructor
DD4hepUI::~DD4hepUI()   = default;

/// Access to the Detector instance
auto DD4hepUI::instance()  const -> Detector*   {
  return &m_detDesc;
}

/// Access to the Detector instance
auto DD4hepUI::detectorDescription()  const -> Detector*   {
  return &m_detDesc;
}

/// Set the printout level from the interactive prompt
auto DD4hepUI::setPrintLevel(PrintLevel level)   const -> PrintLevel   {
  return dd4hep::setPrintLevel(level);
}

/// Set the visualization level when invoking the display
auto DD4hepUI::setVisLevel(int value) -> int     {
  int old_value = visLevel;
  visLevel = value;
  return old_value;
}

/// Install the dd4hep conditions manager object
auto DD4hepUI::conditionsMgr()  const -> Handle<NamedObject>  {
  if ( !m_condMgr.isValid() )  {
    const void* argv[] = {"-handle",&m_condMgr,nullptr};
    if ( 1 != apply("DD4hep_ConditionsManagerInstaller",2,(char**)argv) )  {
      except("DD4hepUI","Failed to install the conditions manager object.");
    }
    if ( !m_condMgr.isValid() )  {
      except("DD4hepUI","Failed to access the conditions manager object.");
    }
  }
  return m_condMgr;
}

/// Load conditions from file
auto DD4hepUI::loadConditions(const std::string& fname)  const -> long  {
  Handle<NamedObject> h = conditionsMgr();
  if ( h.isValid() )  {
    m_detDesc.fromXML(fname, BUILD_DEFAULT);
    return 1;
  }
  return 0;
}

/// Install the dd4hep alignment manager object
auto DD4hepUI::alignmentMgr()  const -> Handle<NamedObject>  {
  if ( !m_alignMgr.isValid() )  {
    const void* argv[] = {"-handle",&m_alignMgr,nullptr};
    if ( 1 != apply("DD4hep_AlignmentsManagerInstaller",2,(char**)argv) )  {
      except("DD4hepUI","Failed to install the alignment manager object.");
    }
    if ( !m_alignMgr.isValid() )  {
      except("DD4hepUI","Failed to access the alignment manager object.");
    }
  }
  return m_alignMgr;
}

/// Detector interface: Manipulate geometry using facroy converter
auto DD4hepUI::apply(const char* factory, int argc, char** argv) const -> long   {
  return m_detDesc.apply(factory, argc, argv);
}

/// Detector interface: Read any geometry description or alignment file
void DD4hepUI::fromXML(const std::string& fname, DetectorBuildType type) const  {
  return m_detDesc.fromXML(fname, type);
}

/// Detector interface: Draw the scene on a OpenGL pane
void DD4hepUI::draw() const   {
  drawSubtree("/world");
}

/// Detector interface: Re-draw the entire scene
void DD4hepUI::redraw() const   {
  redrawSubtree("/world");
}

/// Detector interface: Draw detector sub-tree the scene on a OpenGL pane
void DD4hepUI::drawSubtree(const char* path) const    {
  string vis = _visLevel(visLevel);
  const void* av[] = {"-detector", path, "-option", "ogl", "-level", vis.c_str(), nullptr};
  m_detDesc.apply("DD4hep_GeometryDisplay", 2, (char**)av);
}

/// Detector interface: Re-draw the entire sub-tree scene
void DD4hepUI::redrawSubtree(const char* path) const    {
  string vis = _visLevel(visLevel);
  const void* av[] = {"-detector", path, "-option", "oglsame", "-level", vis.c_str(), nullptr};
  m_detDesc.apply("DD4hep_GeometryDisplay", 4, (char**)av);
}

/// Dump the volume tree
auto DD4hepUI::dumpVols(int argc, char** argv)  const -> long   {
  if ( argc==0 )  {
    const void* av[] = {"-positions","-pointers",nullptr};
    return m_detDesc.apply("DD4hep_VolumeDump",2,(char**)av);
  }
  return m_detDesc.apply("DD4hep_VolumeDump",argc,argv);
}

/// Dump the DetElement tree with placements
auto DD4hepUI::dumpDet(const char* path)  const -> long   {
  const void* args[] = {"--detector", path ? path : "/world", nullptr};
  return m_detDesc.apply("DD4hep_DetectorVolumeDump",2,(char**)args);
}

/// Dump the DetElement tree with placements
auto DD4hepUI::dumpDetMaterials(const char* path)  const -> long   {
  const void* args[] = {"--detector", path ? path : "/world", "--materials", "--shapes", nullptr};
  return m_detDesc.apply("DD4hep_DetectorVolumeDump",4,(char**)args);
}

/// Dump the DetElement tree with placements
auto DD4hepUI::dumpStructure(const char* path)  const -> long   {
  const void* args[] = {"--detector", path ? path : "/world", nullptr};
  return m_detDesc.apply("DD4hep_DetectorDump",2,(char**)args);
}

/// Dump the entire detector description object to a root file
auto DD4hepUI::saveROOT(const char* file_name)    const -> long     {
  if ( file_name )  {
    const void* av[] = {"-output",file_name,nullptr};
    return m_detDesc.apply("DD4hep_Geometry2ROOT",2,(char**)av);
  }
  printout(WARNING,"DD4hepUI","++ saveROOT: No valid output file name supplied");
  return 0;
}

/// Import the entire detector description object from a root file
auto DD4hepUI::importROOT(const char* file_name)    const -> long    {
  if ( file_name )  {
    const void* av[] = {"-input",file_name,nullptr};
    return m_detDesc.apply("DD4hep_RootLoader",2,(char**)av);
  }
  printout(WARNING,"DD4hepUI","++ importROOT: No valid output file name supplied");
  return 0;
}

/// Create ROOT interpreter instance
auto DD4hepUI::createInterpreter(int argc, char** argv) -> long  {
  if ( nullptr == gApplication )  {
    pair<int, char**> a(argc,argv);
    gApplication = new TRint("DD4hepUI", &a.first, a.second);
    printout(INFO,"DD4hepUI","++ Created ROOT interpreter instance for DD4hepUI.");
    return 1;
  }
  printout(WARNING,"DD4hepUI",
           "++ Another ROOT application instance already exists. Keep existing instance.");
  return 1;
}

/// Execute ROOT interpreter instance
auto DD4hepUI::runInterpreter()  const -> long   {
  if ( nullptr != gApplication )  {
    if ( !gApplication->IsRunning() )  {
      gApplication->Run();
      return 1;
    }
    except("DD4hepUI","++ The ROOT application is already running.");
  }
  except("DD4hepUI","++ No ROOT interpreter instance present!");
  return 0;
}

