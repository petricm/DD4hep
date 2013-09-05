// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Generic ROOT based geometry display program
// 
//  Author     : M.Frank
//
//====================================================================
#include "run_plugin.h"
#include "TRint.h"

//______________________________________________________________________________
namespace {
  void usage() {
    cout << "displayGeo -opt [-opt]                                                  \n"
      "        -compact       <file>       Specify the compact geometry file         \n"
      "                     [REQUIRED]     At least one compact geo file is required!\n"
      "        -load_only   [OPTIONAL]     Dry-run to only load geometry without     \n"
      "                                    starting the dispay.                      \n"
      "        -destroy     [OPTIONAL]     Force destruction of the LCDD instance    \n"
      "                                    before exiting the application            \n"
      "        -volmgr      [OPTIONAL]     Load and populate phys.volume manager to  \n"
      "                                    check the volume ids for duplicates etc.  \n"
	 << endl;
    exit(EINVAL);
  }
}

//______________________________________________________________________________
int main(int argc,char** argv)  {
  bool volmgr = false;
  bool dry_run = false, destroy = false;
  vector<char*> geo_files;
  for(int i=1; i<argc;++i) {
    if ( argv[i][0]=='-' ) {
      if ( strncmp(argv[i],"-compact",2)==0 )
	geo_files.push_back(argv[++i]);
      else if ( strncmp(argv[i],"-load_only",2)==0 )
        dry_run = true;
      else if ( strncmp(argv[i],"-destroy",2)==0 )
        destroy = true;
      else if ( strncmp(argv[i],"-volmgr",2)==0 )
        volmgr = true;
      else
	usage();
    }
    else {  // This is the default
      geo_files.push_back(argv[i]);
    }
  }
  if ( geo_files.empty() )
    usage();

  LCDD& lcdd = dd4hep_instance();
  // Load all compact files
  run_plugin(lcdd,"DD4hepCompactLoader",int(geo_files.size()),&geo_files[0]);
  // Create volume manager and populate it required
  if ( volmgr  ) run_plugin(lcdd,"DD4hepVolumeManager",0,0);

  // Create an interactive ROOT application
  if ( !dry_run ) {
    pair<int, char**> args(0,0);
    TRint app("DD4hepGeometryDisplay", &args.first, args.second);
    run_plugin(lcdd,"DD4hepGeometryDisplay",args.first,args.second);
    app.Run();
  }
  else {
    cout << "The geometry was loaded. Application now exiting." << endl;
  }
  if ( destroy ) delete &lcdd;
  return 0;
}
