#include "ns3/Geocentric.hpp"
#include "ns3/LocalCartesian.hpp"
#include <iomanip>
#include <iostream>
#include "ns3/log.h"
#include <math.h>
#include "ns3/geo-utils.h"



NS_LOG_COMPONENT_DEFINE ("GeoUtils");

namespace ns3{
using namespace GeographicLib;
using namespace std;

int
ConvSUint16ToInt(uint16_t x)
{
if (x >> 15)	// Negative
	return (-1 * (int) ((x-1)^0xffff));
else		// Positive
	return (int) x;
}

vector <double> 
GeoCentrToGeoConvert (double x, double y, double z)
{
  vector<double> result;
  Math::real lat, lon, h;
  const GeographicLib::Geocentric& ec = GeographicLib::Geocentric::WGS84;

  ec.Reverse(x, y, z, lat, lon, h);
 //std::cout << lat << " " << lon << " " << h << "\n";
  result.push_back(lat);
  result.push_back(lon);
  result.push_back(h);
//  //std::cout <<"result [0] " <<result [0] << " " << "result [0] "<<result [1] << " " <<"result [0] "<< result [2] << "\n";
  return result;
}

vector <double>
LocToGeoConvert (double x, double y, double z)
{
  vector<double> result;
  Math::real lat, lon, h;

  const GeographicLib::LocalCartesian lc(43.580573,7.121054,0.0);
  lc.Reverse(x, y, z, lat, lon, h);

//  //std::cout << lat << " " << lon << " " << h << "\n";

  result.push_back(lat);
  result.push_back(lon);
  result.push_back(h);
//  //std::cout <<"result [0] " <<result [0] << " " << "result [0] "<<result [1] << " " <<"result [0] "<< result [2] << "\n";
  return result;
}


std::vector <double>
GeoToLocConvert(uint32_t lat, uint32_t lon, uint16_t h)
{
  vector<double> result;
  Math::real x, y, z;
  Math::real latitude = (Math::real)lat;
  Math::real longitude = (Math::real)lon;
  Math::real  altitude = (Math::real)h;
  const GeographicLib::LocalCartesian lc;
  lc.Forward(latitude, longitude, altitude, x, y, z);

  result.push_back(x);
  result.push_back(y);
  result.push_back(z);
//  //std::cout <<"result [0] " <<result [0] << " " << "result [0] "<<result [1] << " " <<"result [0] "<< result [2] << "\n";
  return result;
}

std::vector <double>
GeoToLocConvert(double lat, double lon, double h)
{
  vector<double> result;
  Math::real x, y, z;
  Math::real latitude = (Math::real)lat;
  Math::real longitude = (Math::real)lon;
  Math::real  altitude = (Math::real)h;
//   const GeographicLib::LocalCartesian lc;
  const GeographicLib::LocalCartesian lc(43.580573,7.121054,0.0);
  lc.Forward(latitude, longitude, altitude, x, y, z);

  result.push_back(x);
  result.push_back(y);
  result.push_back(z);
//  //std::cout <<"result [0] " <<result [0] << " " << "result [0] "<<result [1] << " " <<"result [0] "<< result [2] << "\n";
  return result;
}

std::vector <double>
GeoToGeoCentrConvert(uint32_t lat, uint32_t lon, uint16_t h)
{
  vector<double> result;
  Math::real x, y, z;
  const GeographicLib::Geocentric& ec = GeographicLib::Geocentric::WGS84;

  ec.Forward(lat, lon, h, x, y, z);
 //std::cout << lat << " " << lon << " " << h << "\n";
  result.push_back(x);
  result.push_back(y);
  result.push_back(z);
//  //std::cout <<"result [0] " <<result [0] << " " << "result [0] "<<result [1] << " " <<"result [0] "<< result [2] << "\n";
  return result;
}


struct GeoPosition 
 ToDMSConverionLat (double lat)
{
  struct GeoPosition latitudestruct;///Latitude structure 

  latitudestruct.degrees = (int) (lat /*/1000000*/);

  if (latitudestruct.degrees> 180)
  {
  latitudestruct.degrees = -((latitudestruct.degrees) - 180);
  }

  latitudestruct.minutes = (int) (((lat /*/1000000*/)-latitudestruct.degrees)*60);
  latitudestruct.seconds = (((((lat /*/1000000*/)-latitudestruct.degrees)*60)-(latitudestruct.minutes))*60);


  return latitudestruct;
}

struct GeoPosition 
 ToDMSConverionLon (double lon)
{
  struct GeoPosition longitudestruct;///Longitude structure

  longitudestruct.degrees = (int)(lon /*/1000000*/);

  if (longitudestruct.degrees> 180)
  {
  longitudestruct.degrees = -((longitudestruct.degrees) - 180);
  }

  longitudestruct.minutes = (int) (((lon /*/1000000*/)-longitudestruct.degrees)*60);
  longitudestruct.seconds = (((((lon/*/ 1000000*/)-longitudestruct.degrees)*60)-(longitudestruct.minutes))*60);

  return longitudestruct;
}



struct GeoPosition 
ToDMSConverionLat (uint32_t lat)
{

  struct GeoPosition latitudestruct;///Latitude structure 

  latitudestruct.degrees = (int) (lat /1000000);

  if (latitudestruct.degrees> 180)
  {
  latitudestruct.degrees = -((latitudestruct.degrees) - 180);
  }

  latitudestruct.minutes = (int) (((lat /1000000)-latitudestruct.degrees)*60);
  latitudestruct.seconds = (((((lat /1000000)-latitudestruct.degrees)*60)-(latitudestruct.minutes))*60);


  return latitudestruct;
}

struct GeoPosition 
 ToDMSConverionLon (uint32_t lon)
{


  struct GeoPosition longitudestruct;///Longitude structure

  longitudestruct.degrees = (int)(lon /8000000);

  if (longitudestruct.degrees> 180)
  {
  longitudestruct.degrees = -((longitudestruct.degrees) - 180);
  }

  longitudestruct.minutes = (int) (((lon /8000000)-longitudestruct.degrees)*60);
  longitudestruct.seconds = (((((lon/ 8000000)-longitudestruct.degrees)*60)-(longitudestruct.minutes))*60);

  return longitudestruct;
}

double 
CartesianDistanceCal (double lat2, double lon2, double lat1, double lon1)
{
  double distance;
  distance = sqrt(((lat1-lat2)*(lat1-lat2))+((lon1-lon2)*(lon1-lon2)));
  return distance;
}

double 
GeoDistanceCal (uint32_t lat2, uint32_t lon2, uint32_t lat1, uint32_t lon1)
{
  NS_LOG_FUNCTION_NOARGS ();

/**
le calcul de la distance ca va dependre de la formule "Haversine":

delta_lat = lat2 - lat1
delta_long = long2 - long1
a = sin(delata_lat/2) * sin(delata_lat/2) + cos (lat1) * cos (lat2) *sin(delata_long/2) * sin(delata_long/2)
c = 2 * arctangante (sqrt(a)/sqrt(1-a))
distance = R * c
*/
/*
cette formule de calcul de distance utilise les latitudes/longitudes converti en unites de 1/8 de micro degree
*/
  int R = 6371000; //Earth Radius
  double delta_lat, delta_long, a, c, distance;
  delta_lat = lat2 - lat1;
  delta_long = lon2 - lon1;
  a = (sin(delta_lat/2) * sin(delta_lat/2)) + cos (lat1) * cos (lat2) *(sin(delta_long/2) * sin(delta_long/2));
  c = 2 * atan (sqrt(a)/sqrt(1-a));
  distance = R * c;

  return distance;
}

}//namespace ns3
