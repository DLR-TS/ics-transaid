/****************************************************************************/
/// @file    ToString.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Wed, 23 Sep 2002
/// @version $Id: ToString.h 13811 2013-05-01 20:31:43Z behrisch $
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ToString_h
#define ToString_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <sstream>
#include <string>
#include <iomanip>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/SUMOVehicleClass.h>
#include "StdDefs.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * Template for conversions from origin format to string representation
 * (when supplied by c++/the stl)
 */
template <class T>
inline std::string toString(const T& t, std::streamsize accuracy = OUTPUT_ACCURACY) {
    std::ostringstream oss;
    oss.setf(std::ios::fixed , std::ios::floatfield);
    oss << std::setprecision(accuracy);
    oss << t;
    return oss.str();
}


template <>
inline std::string toString<SumoXMLTag>(const SumoXMLTag& tag, std::streamsize accuracy) {
    UNUSED_PARAMETER(accuracy);
    return SUMOXMLDefinitions::Tags.getString(tag);
}


template <>
inline std::string toString<SumoXMLAttr>(const SumoXMLAttr& attr, std::streamsize accuracy) {
    UNUSED_PARAMETER(accuracy);
    return SUMOXMLDefinitions::Attrs.getString(attr);
}


template <>
inline std::string toString<SumoXMLNodeType>(const SumoXMLNodeType& nodeType, std::streamsize accuracy) {
    UNUSED_PARAMETER(accuracy);
    return SUMOXMLDefinitions::NodeTypes.getString(nodeType);
}


template <>
inline std::string toString<SumoXMLEdgeFunc>(const SumoXMLEdgeFunc& edgeFunc, std::streamsize accuracy) {
    UNUSED_PARAMETER(accuracy);
    return SUMOXMLDefinitions::EdgeFunctions.getString(edgeFunc);
}


template <>
inline std::string toString<SUMOVehicleClass>(const SUMOVehicleClass& vClass, std::streamsize accuracy) {
    UNUSED_PARAMETER(accuracy);
    return SumoVehicleClassStrings.getString(vClass);
}


template <>
inline std::string toString<LaneSpreadFunction>(const LaneSpreadFunction& lsf, std::streamsize accuracy) {
    UNUSED_PARAMETER(accuracy);
    return SUMOXMLDefinitions::LaneSpreadFunctions.getString(lsf);
}


template <>
inline std::string toString<LinkState>(const LinkState& linkState, std::streamsize accuracy) {
    UNUSED_PARAMETER(accuracy);
    return SUMOXMLDefinitions::LinkStates.getString(linkState);
}

template <>
inline std::string toString<LinkDirection>(const LinkDirection& linkDir, std::streamsize accuracy) {
    UNUSED_PARAMETER(accuracy);
    return SUMOXMLDefinitions::LinkDirections.getString(linkDir);
}

template <>
inline std::string toString<TrafficLightType>(const TrafficLightType& type, std::streamsize accuracy) {
    UNUSED_PARAMETER(accuracy);
    return SUMOXMLDefinitions::TrafficLightTypes.getString(type);
}


template <typename V>
inline std::string toString(const std::vector<V*>& v, std::streamsize accuracy = OUTPUT_ACCURACY) {
    UNUSED_PARAMETER(accuracy);
    std::ostringstream oss;
    for (typename std::vector<V*>::const_iterator it = v.begin(); it != v.end(); ++it) {
        if (it != v.begin()) {
            oss << " ";
        }
        oss << (*it)->getID();
    }
    return oss.str();
}


template <typename T, typename T_BETWEEN>
inline std::string joinToString(const std::vector<T>& v, const T_BETWEEN& between, std::streamsize accuracy = OUTPUT_ACCURACY) {
    std::ostringstream oss;
    bool connect = false;
    for (typename std::vector<T>::const_iterator it = v.begin(); it != v.end(); ++it) {
        if (connect) {
            oss << toString(between, accuracy);
        } else {
            connect = true;
        }
        oss << toString(*it, accuracy);
    }
    return oss.str();
}


template <>
inline std::string toString(const std::vector<int>& v, std::streamsize accuracy) {
    return joinToString(v, " ", accuracy);
}


template <>
inline std::string toString(const std::vector<SUMOReal>& v, std::streamsize accuracy) {
    return joinToString(v, " ", accuracy);
}


template <typename T, typename T_BETWEEN>
inline std::string joinToString(const std::set<T>& s, const T_BETWEEN& between, std::streamsize accuracy = OUTPUT_ACCURACY) {
    std::ostringstream oss;
    bool connect = false;
    for (typename std::set<T>::const_iterator it = s.begin(); it != s.end(); ++it) {
        if (connect) {
            oss << toString(between, accuracy);
        } else {
            connect = true;
        }
        oss << toString(*it, accuracy);
    }
    return oss.str();
}

#endif

/****************************************************************************/

