// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//  This is a special implementation for a FixedAnglePadLayout!
//  Ideally this will become a purely virtual interface from which
//  to inherit the special implementations of different PadLayouts.
//
//====================================================================
#include "DD4hep/LCDD.h"

#include "TPCModule.h"
#include "TPCModuleData.h"
#include "TGeoTube.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include <math.h>
#include <iostream>

using namespace std;

namespace DD4hep {
  
  using namespace Geometry;
  
  TPCModule::TPCModule(const string& name, const string& type, int id)
  {
    Value<TNamed,TPCModuleData>* p = new Value<TNamed,TPCModuleData>();
    assign(p,name, type);
    p->id = id;
  }
  
  int TPCModule::getID()const {
    return _data().id;
  }
 
  std::string TPCModule::getPadType()const {
    ProjectiveCylinder pads= readout().segmentation();
    return pads.type();
  }
 
  int TPCModule::getNPads()const {
    ProjectiveCylinder pads= readout().segmentation();
    return pads.thetaBins()* pads.phiBins();
  }
  
  int TPCModule::getNRows()const {
    ProjectiveCylinder pads= readout().segmentation();
    return pads.thetaBins();
  }

  int TPCModule::getNPadsInRow(int row)const {
    ProjectiveCylinder pads= readout().segmentation();
    return pads.phiBins();
  }
  
  double TPCModule::getRowHeight(int row)const {
    if(row>getNRows())
      throw OutsideGeometryException("getRowHeight: Requested row not on module querried!");
    //all rows are the same for FixedAnglePadLayout=ProjectiveCylinder 
    Tube       tube=volume().solid();
    double module_height= tube->GetRmax()-tube->GetRmin();
    return module_height/getNRows();
  }
  
  int TPCModule::getRowNumber(int pad)const {
    if(pad>getNPads())
      throw OutsideGeometryException("getRowNumber: Requested pad not on module querried!");
    return pad/getNPadsInRow(0);
  }
  
  double TPCModule::getPadPitch(int pad)const {
    if(pad>getNPads())
      throw OutsideGeometryException("getPadPitch: Requested pad not on module querried!");
    int row=getRowNumber(pad);
    Tube tube=volume().solid();
    double pad_radius=tube->GetRmin()+(row+0.5)*getRowHeight(0);
    double module_width= tube->GetPhi2()-tube->GetPhi1();
    double pad_angle=module_width/getNPadsInRow(row);
    return pad_radius*pad_angle*M_PI/180.;
  }
  
  int TPCModule::getPadNumber(int pad)const {
   if(pad>getNPads())
      throw OutsideGeometryException("getPadNumber: Requested pad not on module querried!");
    return pad % getNPadsInRow(0);
  }

  int TPCModule::getPadIndex(int row,int padNr)const {
    if(padNr>=getNPadsInRow(row))
      throw OutsideGeometryException("getPadIndex: Requested pad not on module querried!");
    if(row>=getNRows())
      throw OutsideGeometryException("getPadIndex: Requested row not on module querried!");
    return padNr + row*getNPadsInRow(row);
  }

  int TPCModule::getRightNeighbour(int pad)const {
    //if on edge their is no neighbour, should throw an exception
    int row=getRowNumber(pad);
    if(getPadNumber(pad)==getNPadsInRow(row)-1)
      throw OutsideGeometryException("getRightNeighbour: Requested pad is on right edge and has no right neighbour!");
    // if not on edge
    return pad + 1;
  }

  int TPCModule::getLeftNeighbour(int pad)const {
    //if on edge their is no neighbour, should throw an exception
    if(getPadNumber(pad)==0)
      throw OutsideGeometryException("getLeftNeighbour: Requested pad is on left edge and has no left neighbour!");
    // if not on edge
    return pad - 1;
  }

  void printPlace(const PlacedVolume& pl) {
    TGeoMatrix* m = pl->GetMatrix();
    if ( m ) {
      const Double_t* tr = m->GetTranslation();
      if ( tr ) {
	//cout << "\t\t Tr:" << tr[0] << " " << tr[1] << " " << tr[2] << endl;
      }
    }
  }
  void check_points(const char* prefix, const Position& global, const double* point) {
    cout << prefix << global.x   << " " << global.y   << " " << global.z   << endl;
    if ( global.x != point[0] || global.y != point[1] || global.z != point[2] ) {
      cout << "Points are different -- wrong transformation!!!!" << endl;
      cout << "Points are " << point[0]  << " " << point[1] << " " << point[2] << endl;
    }
  }

  std::vector<double>  TPCModule::getPadCenter (int pad) const {
    if(pad>getNPads())
      throw OutsideGeometryException("getPadCenter: Requested pad not on module querried!");
    int row=getRowNumber(pad);
    Tube tube=volume().solid();
    double pad_radius=tube->GetRmin()+(row+0.5)*getRowHeight(0);
    double module_width= tube->GetPhi2()-tube->GetPhi1();
    double pad_angle=(getPadNumber(pad)+0.5)*module_width/getNPadsInRow(row);
    //local center coordinates in module system
    double pad_x=pad_radius*cos(pad_angle*M_PI/180.);
    double pad_y=pad_radius*sin(pad_angle*M_PI/180.);
 
    //trafo to global coordinates
    Double_t point_local[3];
    Double_t point_global[3];
    Double_t point_global_m[3];
    Double_t point_global_t[3];
    point_local[0]=pad_x;
    point_local[1]=pad_y;
    point_local[2]=getModuleZPosition();


  

    Position global, global_w, global_r, global_p, local(point_local[0],point_local[1],point_local[2]);
    this->localToWorld(local,global_w);
    //    this->localToParent(local,global);
//     std::cout<<"Exp-Local: "<<point_local[0]<<" "<<point_local[1]<<" "<<point_local[2]<<std::endl;
//     std::cout<<"Det-Par: " << global.x   << " " << global.y   << " " << global.z   << std::endl;
//     std::cout<<"Det-Top: " << global_w.x   << " " << global_w.y   << " " << global_w.z   << std::endl;


    vector<double> center;
    center.push_back(global_w.x);
    center.push_back(global_w.y);
    return center;
  }
  
  int TPCModule::getNearestPad(double c0,double c1)const {
    //find z position of module in world coordinates
    Position fake_local(0,0,getModuleZPosition());
    Position fake_global;
     this->localToWorld(fake_local,fake_global);
    //trafo to local coordinates
    Position global(c0,c1,fake_global.z), local;
    this->worldToLocal(global,local);
    Double_t point_local[3]={local.x,local.y,local.z};
    //check if it is on that module
    bool onMod=volume().solid()->Contains(point_local);
    if(!onMod)
      throw OutsideGeometryException("getNearestPad: Requested point not on module querried!");
    Tube tube=volume().solid();
    double module_width= tube->GetPhi2()-tube->GetPhi1();
    double radius=sqrt(point_local[0]*point_local[0]+point_local[1]*point_local[1]);
    int row=(radius-tube->GetRmin())/getRowHeight(0);
    //outer edge of last row belongs to last row
    if(row==getNRows())
      row=getNRows()-1;
    double pad_width=module_width/getNPadsInRow(row);    
    double angle=atan2(point_local[1],point_local[0])/M_PI*180;
    int padNr=static_cast<int>(angle/pad_width);
    return getPadIndex(row,padNr);
  }
 
  double TPCModule::getModuleZPosition() const {
    //for internal use only, gives back coordinate in local system
    TGeoMatrix *nm=placement()->GetMatrix();
    const Double_t *trans=nm->GetTranslation();
    return trans[2];
  }
}
