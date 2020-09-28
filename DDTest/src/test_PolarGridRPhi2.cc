#include "DDSegmentation/PolarGridRPhi2.h"
#include "DD4hep/DDTest.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <exception>


static dd4hep::DDTest test( "PolarGridRPhi2" ) ;

class TestTuple {
public:
  double    _r;
  double    _p; //phi
  long long _rB;
  long long _pB;
  TestTuple( double r, double p, long long rB, long long pB): _r(r), _p(p), _rB(rB), _pB(pB) {}
};

int main() {
  try{

    dd4hep::DDSegmentation::PolarGridRPhi2 seg("system:8,layer:8,barrel:3,layer:8,slice:5,r:16,phi:16");
    const int rBitOffset   = 32; //sum of all the others before
    const int phiBitOffset = 16+32; //sum of all the others before

    std::vector<double> rValues, phiValues;

    rValues.push_back( 10);//  0
    rValues.push_back( 20);//  1
    rValues.push_back( 30);//  2
    rValues.push_back( 40);//  3
    rValues.push_back( 50);//  4
    rValues.push_back( 60);//  5
    rValues.push_back( 70);//  6
    rValues.push_back( 80);//  7
    rValues.push_back( 90);//  8
    rValues.push_back(100);//  9
    rValues.push_back(110);// 10
    rValues.push_back(120);// 11
    rValues.push_back(130);// 12
    rValues.push_back(140);// 13
    rValues.push_back(150);// 14
    rValues.push_back(160);// 15

    const double DegToRad(M_PI/180.0);

    phiValues.push_back( 10*DegToRad);//  0
    phiValues.push_back( 20*DegToRad);//  1
    phiValues.push_back( 30*DegToRad);//  2
    phiValues.push_back( 40*DegToRad);//  3
    phiValues.push_back( 50*DegToRad);//  4
    phiValues.push_back( 60*DegToRad);//  5
    phiValues.push_back( 70*DegToRad);//  6
    phiValues.push_back( 80*DegToRad);//  7
    phiValues.push_back( 90*DegToRad);//  8
    phiValues.push_back(100*DegToRad);//  9
    phiValues.push_back(110*DegToRad);// 10
    phiValues.push_back(120*DegToRad);// 11
    phiValues.push_back(130*DegToRad);// 12
    phiValues.push_back(140*DegToRad);// 13
    phiValues.push_back(150*DegToRad);// 14
    //need one less phiValue than radial segments
    //phiValues.push_back(160*DegToRad);// 15

    seg.setGridRValues(rValues);
    seg.setGridPhiValues(phiValues);
    seg.setOffsetPhi(-M_PI);

    std::vector<TestTuple> tests;
    tests.emplace_back( 10.0, -180*DegToRad,   0,   0 );
    tests.emplace_back( 10.1,  175*DegToRad,   0,  35 );
    tests.emplace_back( 20.0,   22*DegToRad,   1,  10 );
    tests.emplace_back( 20.1,   40*DegToRad,   1,  11 );
    tests.emplace_back( 15.0,  359*DegToRad,   0,  17 );
    tests.emplace_back( 42.0,   42*DegToRad,   3,   5 );
    tests.emplace_back( 22.0,    0*DegToRad,   1,   9 );
    tests.emplace_back( 22.0,  180*DegToRad,   1,  18 );
    tests.emplace_back( 22.0,  -180*DegToRad,   1,  0 );
    tests.emplace_back(  10.1, -179*DegToRad,     0,   0 );
    tests.emplace_back(  20.1, -179*DegToRad,     1,   0 );
    tests.emplace_back(  30.1, -179*DegToRad,     2,   0 );
    tests.emplace_back(  40.1, -179*DegToRad,     3,   0 );
    tests.emplace_back(  50.1, -179*DegToRad,     4,   0 );
    tests.emplace_back(  60.1, -179*DegToRad,     5,   0 );
    tests.emplace_back(  70.1, -179*DegToRad,     6,   0 );
    tests.emplace_back(  80.1, -179*DegToRad,     7,   0 );
    tests.emplace_back(  90.1, -179*DegToRad,     8,   0 );
    tests.emplace_back( 100.1, -179*DegToRad,     9,   0 );
    tests.emplace_back( 110.1, -179*DegToRad,    10,   0 );
    tests.emplace_back( 120.1, -179*DegToRad,    11,   0 );
    tests.emplace_back( 130.1, -179*DegToRad,    12,   0 );
    tests.emplace_back( 140.1, -179*DegToRad,    13,   0 );
    tests.emplace_back( 150.1, -179*DegToRad,    14,   0 );
    tests.emplace_back( 160.0, -179*DegToRad,    14,   0 );


    dd4hep::DDSegmentation::VolumeID volID = 0;

    //Test from position to cellID
    for(const auto & it : tests) {

      const double r     = it._r;
      const double phi   = it._p;
      const long long rB = it._rB;
      const long long pB = it._pB;

      dd4hep::DDSegmentation::Vector3D locPos ( r*cos(phi), r*sin(phi), 0.0);
      dd4hep::DDSegmentation::Vector3D globPos( r*cos(phi), r*sin(phi), 0.0);

      dd4hep::DDSegmentation::CellID cid = seg.cellID(locPos, globPos, volID);

      const long long phiShifted(pB << phiBitOffset);
      const long long rShifted  (rB << rBitOffset);
      const long long expectedID(rShifted + phiShifted);

      test( expectedID , cid , " Test get ID From Position" );

      std::cout << std::setw(20) <<  " "
		<< std::setw(20) <<  "rBin     "
		<< std::setw(20) <<  "pBin     "
		<< std::endl;

      std::cout << std::setw(20) <<  "Expected"
		<< std::setw(20) <<  rB
		<< std::setw(20) <<  pB
		<< std::endl;

      std::cout << std::setw(20) <<  "Calculated"
		<< std::setw(20) <<  seg.decoder()->get(cid,"r")
		<< std::setw(20) <<  seg.decoder()->get(cid,"phi")
		<< std::endl;

    }



    std::vector<TestTuple> testPositions;
    testPositions.emplace_back( 15.0, -175*DegToRad,   0,   0 );
    testPositions.emplace_back( 15.0,  175*DegToRad,   0,  35 );
    testPositions.emplace_back( 25.0, -150*DegToRad,   1,  1 );
    testPositions.emplace_back( 35.0, -135*DegToRad,   2,  1 );
    testPositions.emplace_back( 45.0, -120*DegToRad,   3,  1 );
    testPositions.emplace_back( 55.0, -105*DegToRad,   4,  1 );
    testPositions.emplace_back( 65.0,  -90*DegToRad,   5,  1 );
    testPositions.emplace_back( 75.0,  -75*DegToRad,   6,  1 );
    testPositions.emplace_back( 85.0,  -60*DegToRad,   7,  1 );
    testPositions.emplace_back( 95.0,  -45*DegToRad,   8,  1 );
    testPositions.emplace_back( 105.0, -30*DegToRad,   9,  1 );
    testPositions.emplace_back( 115.0, -15*DegToRad,  10,  1 );
    testPositions.emplace_back( 125.0,  -0*DegToRad,  11,  1 );
    testPositions.emplace_back( 135.0,  15*DegToRad,  12,  1 );
    testPositions.emplace_back( 145.0,  30*DegToRad,  13,  1 );
    testPositions.emplace_back( 155.0,  45*DegToRad,  14,  1 );
    testPositions.emplace_back(155.0, -105*DegToRad,  14,   0 );

    //Test from cellID to position
    for(const auto & testPosition : testPositions) {

      const double r     = testPosition._r;
      const double phi   = testPosition._p;
      const long long rB = testPosition._rB;
      const long long pB = testPosition._pB;

      dd4hep::DDSegmentation::CellID cellID  ;

      seg.decoder()->set(cellID,"r"  , rB);
      seg.decoder()->set(cellID,"phi", pB);

      std::cout << "CellID: " << cellID  << std::endl;

      dd4hep::DDSegmentation::Vector3D expectedPosition( r*cos(phi), r*sin(phi), 0.0);
      dd4hep::DDSegmentation::Vector3D calculatedPosition = seg.position(cellID);

      test( fabs(expectedPosition.x() - calculatedPosition.x())  < 1e-11, " Test get Position from ID: X" );
      test( fabs(expectedPosition.y() - calculatedPosition.y())  < 1e-11, " Test get Position from ID: Y" );
      test( fabs(expectedPosition.z() - calculatedPosition.z())  < 1e-11, " Test get Position from ID: Z" );

      std::cout << std::setw(20) <<  " "
		<< std::setw(20) <<  "r     "
		<< std::setw(20) <<  "phi     "
		<< std::endl;

      std::cout << std::setw(20) <<  "Expected"
		<< std::setw(20) <<  r
		<< std::setw(20) <<  phi/DegToRad
		<< std::endl;

      const double rCalc =
	sqrt( calculatedPosition.x() * calculatedPosition.x() +
	      calculatedPosition.y() * calculatedPosition.y() );
      const double pCalc = atan2( calculatedPosition.y(), calculatedPosition.x() );

      std::cout << std::setw(20) <<  "Calculated"
		<< std::setw(20) <<  rCalc
		<< std::setw(20) <<  pCalc/DegToRad
		<< std::endl;

    }



  } catch( std::exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }
  return 0;

}
