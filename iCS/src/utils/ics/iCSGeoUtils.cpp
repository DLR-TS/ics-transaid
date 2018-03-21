/****************************************************************************/
/// @file    iCSGeoUtils.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    jun 3, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "iCSGeoUtils.h"

#include <iostream>

/*  ===========================================================================
 *  USE NOTE:
 *  ---------------------------------------------------------------------------
 *  After the installation of the GeographicLib library
 *  (http://geographiclib.sourceforge.net)
 *  it is needed to update the list of the links and the cache
 *  to the most recent shared libraries in the GeographicLib installation path
 *  folder by running "sudo ldconfig /usr/local/lib"
 *  ===========================================================================
 */

#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/LocalCartesian.hpp>
using namespace GeographicLib;

/*! \fn vector <double> GeoCentrToGeoConvert (double x, double y, double z)
 \brief Convert geocentric coordinates to geodetic coordinates.
 \param[in] local coordinates values (x, y, z).
 \param[out] vector containing the latitude, longitude and altitude coordinates, respectively.
 */
std::vector <double>
GeoCentrToGeoConvert (double x, double y, double z) {
    std::vector<double> result;
    Math::real lat, lon, h;
    const Geocentric& ec = Geocentric::WGS84;

    ec.Reverse(x, y, z, lat, lon, h);
    result.push_back(lat);
    result.push_back(lon);
    result.push_back(h);
    return result;
}

/*! \fn std::vector <double> LocToGeoConvert (double x, double y, double z, double localLat, double localLon, double localAlt)
 \brief Convert local coordinates to geodetic coordinates.
 \param[in] local coordinates values (x, y, z) and local geodetic coordinates (lat, lon, alt) of the origin point (x,y,z)=(0,0,0).
 \param[out] std::vector containing the latitude, longitude and altitude coordinates, respectively.
 */
std::vector <double>
LocToGeoConvert (double x, double y, double z, double localLat, double localLon, double localAlt) {
    std::vector<double> result;
    Math::real lat, lon, h;

    const LocalCartesian lc(localLat, localLon, localAlt);;
    lc.Reverse(x, y, z, lat, lon, h);

    result.push_back(lat);
    result.push_back(lon);
    result.push_back(h);
    return result;
}

/*! \fn std::vector <double> GeoToLocConvert(double lat, double lon, double h)
 \brief Convert geodetic coordinates to local coordinates.
 \param[in] local geodetic coordinates values (lat, lon, alt).
 \param[out] std::vector containing the local coordinates x, y and z, respectively.
 */
std::vector <double>
GeoToLocConvert(double lat, double lon, double h,  double localLat, double localLon, double localAlt) {
  std::vector<double> result;
  double x, y, z;
  const LocalCartesian lc(localLat, localLon, localAlt);
  lc.Forward(lat, lon, h, x, y, z);

  result.push_back(x);
  result.push_back(y);
  result.push_back(z);
  return result;
}

/*! \fn std::vector <double> GeoToGeoCentrConvert(double lat, double lon, double h)
 \brief Convert geodetic coordinates to geocentric coordinates.
 \param[in] local geodetic coordinates values (lat, lon, alt).
 \param[out] std::vector containing the geo centric coordinates x, y and z, respectively.
 */
std::vector <double>
GeoToGeoCentrConvert(double lat, double lon, double h) {
  std::vector<double> result;
  double x, y, z;
  const Geocentric& ec = Geocentric::WGS84;

  ec.Forward(lat, lon, h, x, y, z);
  result.push_back(x);
  result.push_back(y);
  result.push_back(z);
  return result;
}
