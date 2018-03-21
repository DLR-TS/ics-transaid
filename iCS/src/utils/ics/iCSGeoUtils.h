/****************************************************************************/
/// @file    iCSGeoUtils.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    jun 3, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef ICSGEOUTILS_H_
#define ICSGEOUTILS_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>

/*! \fn vector <double> GeoCentrToGeoConvert (double x, double y, double z)
 \brief Convert geocentric coordinates to geodetic coordinates.

 Convert from geocentric coordinates \e x, \e y, \e z (meters) to
 geodetic \e lat, \e lon (degrees), \e h (meters).  In general there are
 multiple solutions and the result which minimizes the absolute value of
 \e h is returned.  If there are still multiple solutions with different
 latitutes (applies only if \e z = 0), then the solution with \e lat > 0
 is returned.  If there are still multiple solutions with different
 longitudes (applies only if \e x = \e y = 0) then \e lon = 0 is
 returned.  The value of \e h returned satisfies \e h >= - \e a (1 - \e
 e<sup>2</sup>) / sqrt(1 - \e e<sup>2</sup> sin<sup>2</sup>\e lat).  The
 value of \e lon returned is in the range [-180, 180).

 \param[in] local coordinates values (x, y, z).
 \param[out] vector containing the latitude, longitude and altitude coordinates, respectively.
 */
std::vector <double> GeoCentrToGeoConvert (double x, double y, double z);

/*! \fn std::vector <double> LocToGeoConvert (double x, double y, double z, double localLat, double localLon, double localAlt)
 \brief Convert local coordinates to geodetic coordinates.

 Convert from local cartesian \e x, \e y, \e z (meters) to geodetic
 coordinates \e lat, \e lon (degrees), \e h (meters).  The value of \e
 lon returned is in the range [-180, 180).

 \param[in] local coordinates values (x, y, z) and local geodetic coordinates (lat, lon, alt) of the origin point (x,y,z)=(0,0,0).
 \param[out] std::vector containing the latitude, longitude and altitude coordinates, respectively.
 */
std::vector <double> LocToGeoConvert (double x, double y, double z, double localLat, double localLon, double localAlt);

/*! \fn std::vector <double> GeoToLocConvert(double lat, double lon, double h)
 \brief Convert geodetic coordinates to local coordinates.

 Convert from geodetic coordinates \e lat, \e lon (degrees), \e h
 (meters) to local cartesian coordinates \e x, \e y, \e z (meters).  \e
 lat should be in the range [-90, 90]; \e lon and \e lon0 should be in
 the range [-180, 360].

 \param[in] local geodetic coordinates values (lat, lon, alt).
 \param[out] std::vector containing the local coordinates x, y and z, respectively.
 */
std::vector <double> GeoToLocConvert(double lat, double lon, double h,  double localLat, double localLon, double localAlt);

/*! \fn std::vector <double> GeoToGeoCentrConvert(double lat, double lon, double h)
 \brief Convert geodetic coordinates to geocentric coordinates.

 Convert from geodetic coordinates \e lat, \e lon (degrees), \e h
 (meters) to geocentric coordinates \e x, \e y, \e z (meters).  \e lat
 should be in the range [-90, 90]; \e lon and \e lon0 should be in the
 range [-180, 360].

 \param[in] local geodetic coordinates values (lat, lon, alt).
 \param[out] std::vector containing the geo centric coordinates x, y and z, respectively.
 */
std::vector <double> GeoToGeoCentrConvert(double lat, double lon, double h);
#endif /* ICSGEOUTILS_H_ */
