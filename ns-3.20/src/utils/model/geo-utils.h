#ifndef GEO_UTILS_H
#define GEO_UTILS_H
#include "ns3/ptr.h"
#include <iostream>
#include "ns3/simulator.h"
#include <stdlib.h>
#include <vector>

#include "ns3/object.h"
namespace ns3 {

  struct GeoPosition {
    int degrees;
    int minutes;
    double seconds;
  };

  std::vector <double> GeoCentrToGeoConvert (double x, double y, double z);
  std::vector <double> LocToGeoConvert (double x, double y, double z);
  std::vector <double> GeoToGeoCentrConvert(uint32_t lat, uint32_t lon, uint16_t h);
  std::vector <double> GeoToLocConvert(uint32_t lat, uint32_t lon, uint16_t h);
  std::vector <double> GeoToLocConvert(double lat, double lon, double h);

  struct GeoPosition ToDMSConverionLat (double lat);
  struct GeoPosition ToDMSConverionLat (uint32_t lat);
  struct GeoPosition ToDMSConverionLon (double lon);
  struct GeoPosition ToDMSConverionLon (uint32_t lon);
  double CartesianDistanceCal (double lat2, double lon2, double lat1, double lon1);
  double GeoDistanceCal (uint32_t lat2, uint32_t lon2, uint32_t lat1, uint32_t lon1);

  int ConvSUint16ToInt(uint16_t x);

  typedef struct GeoPosition GeoPosStruct;

}; // namespace ns3

#endif /* GEO_UTILS_H */
 

