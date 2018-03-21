#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <ics/configfile_parsers/sumoMapParser/SUMOdigital-map.h>
#include <utils/ics/geometric/Point2D.h>
#include <utils/xml/XMLSubSys.h>


namespace {
  class SUMOdigitalMapTest : public ::testing::Test {
  protected:
    virtual void SetUp () {
      XMLSubSys::init();
    }
  };

}

TEST_F(SUMOdigitalMapTest, testOneEdge) {
  std::string fileName = "singleEdge.xml";
  std::ifstream infile;

  infile.open(fileName.c_str());
  if (!infile.is_open()) {
    std::cerr  << " not found. Map loading terminated." << std::endl;
  }
 
  sumo_map::SUMODigitalMap sDigiMap;
  sDigiMap.loadMap(fileName);

  ASSERT_TRUE( (sDigiMap.edges).size() == 1);
  
}

TEST_F(SUMOdigitalMapTest, testLocation ) {
  std::string fileName = "oldStyle_locationOnly.xml";
  std::ifstream infile;

  infile.open(fileName.c_str());
  if (!infile.is_open()) {
    std::cerr  << " not found. Map loading terminated." << std::endl;
  }
 
  sumo_map::SUMODigitalMap sDigiMap;
  sDigiMap.loadMap(fileName);

  sumo_map::SUMOLocation testLocation;                       
  testLocation = sDigiMap.location;

  //  <SUMOLocation netOffset="501.00,251.00" convBoundary="0.00,0.00,1002.00,502.00" origBoundary="-501.00,-251.00,501.00,251.00" projParameter="!"/>
  
  EXPECT_TRUE( testLocation.netOffset.x() == 501)  ;
  EXPECT_TRUE( testLocation.netOffset.y() == 251)  ;

  EXPECT_TRUE( testLocation.convBoundaryMin.x() == 0 )  ;
  EXPECT_TRUE( testLocation.convBoundaryMin.y() == 0 )  ;
  
  EXPECT_TRUE( testLocation.convBoundaryMax.x() == 1002 )  ;
  EXPECT_TRUE( testLocation.convBoundaryMax.y() == 502 )  ;
  
  EXPECT_TRUE( testLocation.origBoundaryMin.x() == -501 )  ;
  EXPECT_TRUE( testLocation.origBoundaryMin.y() == -251 )  ;
  
  EXPECT_TRUE( testLocation.origBoundaryMax.x() == 501 )  ;
  EXPECT_TRUE( testLocation.origBoundaryMax.y() == 251 )  ;
  
  std::string s ;
  s = testLocation.projParameter;
  EXPECT_STREQ( s.c_str(), "!" ) ;
}

/*TEST_F(SUMOdigitalMapTest, testLocation ) {
  string fileName = "newStyle_locationOnly.xml";
  ifstream infile;

  infile.open(fileName.c_str());
  if (!infile.is_open()) {
    std::cerr  << " not found. Map loading terminated." << endl;
  }
 
  sumo_map::SUMODigitalMap sDigiMap (fileName) ;
  sDigiMap.loadMap();

  sumo_map::SUMOLocation testLocation;                       
  testLocation = sDigiMap.location;

  //  <SUMOLocation netOffset="501.00,251.00" convBoundary="0.00,0.00,1002.00,502.00" origBoundary="-501.00,-251.00,501.00,251.00" projParameter="!"/>
  
  EXPECT_TRUE( testLocation.netOffset.x() == 501)  ;
  EXPECT_TRUE( testLocation.netOffset.y() == 251)  ;

  EXPECT_TRUE( testLocation.convBoundaryMin.x() == 0 )  ;
  EXPECT_TRUE( testLocation.convBoundaryMin.y() == 0 )  ;
  
  EXPECT_TRUE( testLocation.convBoundaryMax.x() == 1002 )  ;
  EXPECT_TRUE( testLocation.convBoundaryMax.y() == 502 )  ;
  
  EXPECT_TRUE( testLocation.origBoundaryMin.x() == -501 )  ;
  EXPECT_TRUE( testLocation.origBoundaryMin.y() == -251 )  ;
  
  EXPECT_TRUE( testLocation.origBoundaryMax.x() == 501 )  ;
  EXPECT_TRUE( testLocation.origBoundaryMax.y() == 251 )  ;
  
  string s ;
  s = testLocation.projParameter;
  EXPECT_STREQ( s.c_str(), "!" ) ;
  

}
*/

// get number edges

// get Egde Id


