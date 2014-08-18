// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================

// Framework include files
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"

#include "DDEve/View.h"
#include "DDEve/ViewMenu.h"
#include "DDEve/PopupMenu.h"

// ROOT include files
#include "TEveBrowser.h"
#include "TEveManager.h"
#include "TEveElement.h"
#include "TEveWindow.h"
#include "TEveViewer.h"
#include "TGLViewer.h"
#include "TGClient.h"
#include "TGTab.h"

// Forward declarations
class TEveWindowSlot;

using namespace std;
using namespace DD4hep;

ClassImp(ViewMenu)

/// Initializing constructor
ViewMenu::ViewMenu(Display* display, const std::string& title) 
: PopupMenu(display->client().GetRoot()), m_display(display), m_title(title)
{
  InstanceCount::increment(this);
}

/// Default destructor
ViewMenu::~ViewMenu()  {
  InstanceCount::decrement(this);
}

/// Add the menu to the menu bar
void ViewMenu::Build(TGMenuBar* bar, int hints)    {
  pair<string,string>* p = 0;
  PopupMenu* menu = this;
  menu->AddEntry("3&D View", this, &ViewMenu::CreateView, p=new pair<string,string>("DD4hep__View3D","3D"));
  menu->AddEntry("Rho-&Z Projection", this, &ViewMenu::CreateView, p=new pair<string,string>("DD4hep__RhoZProjection","Rho-Z"));
  menu->AddEntry("Rho-&Phi Projection",this, &ViewMenu::CreateView, p=new pair<string,string>("DD4hep__RhoPhiProjection","Rho-Phi"));
  const Display::ViewConfigurations& vc = m_display->viewConfigurations();
  for(Display::ViewConfigurations::const_iterator i=vc.begin(); i!=vc.end(); ++i)  {
    const Display::ViewConfig& v = (*i).second;
    menu->AddEntry(v.name.c_str(), this, &ViewMenu::CreateView,p=new pair<string,string>("DD4hep__"+v.type,v.name));
  }
  bar->AddPopup(m_title.c_str(),*menu, new TGLayoutHints(hints, 0, 4, 0, 0));
}

/// Create a new generic view
void ViewMenu::CreateView(TGMenuEntry*, void* ud)   {
  pair<string,string>* args = (pair<string,string>*)ud;
  CreateView(args->first,args->second);
}

View* ViewMenu::CreateView(const std::string& type, const std::string& title)   {
  pair<string,string> args("DD4hep__"+type,title);
  View* v = PluginService::Create<View*>(type.c_str(),m_display,title.c_str());
  BuildView(v);
  return v;
}

/// Create a new 3D view
View* ViewMenu::CreateView3D(const std::string& title)   {
  return CreateView("DD4hep__View3D",title.c_str());
}

/// Create a new R-Z view
View* ViewMenu::CreateRhoZProjection(const std::string& title )  {
  return CreateView("DD4hep__RhoZProjection",title.c_str());
}

/// Create a new R-Phi view
View* ViewMenu::CreateRhoPhiProjection(const std::string& title )  {
  return CreateView("DD4hep__RhoPhiProjection",title.c_str());
}

/// Import Geometry data
void ViewMenu::BuildView(View* v)  const  {
  TEveManager& m = m_display->manager();
  TEveBrowser    *browser = m.GetBrowser();
  TGTab          *right = browser->GetTabRight();
  TEveWindowSlot *slot = TEveWindow::CreateWindowInTab(right);
  v->Build(slot);
  m_display->RegisterEvents(v);
  v->ConfigureGeometry();
  v->ConfigureEvent();
  v->Initialize();
  right->SetTab(right->GetNumberOfTabs()-1);
}
