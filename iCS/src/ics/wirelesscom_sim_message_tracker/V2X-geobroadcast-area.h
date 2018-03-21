/****************************************************************************/
/// @file    V2X-geobroadcast-area.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef V2X_GEOBROADCAST_AREA_H
#define V2X_GEOBROADCAST_AREA_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>

namespace ics
{

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class V2xGeobroadcastArea
 * @brief Defines a geobroadcast area in which messages are going to be sent.
 */
class V2xGeobroadcastArea
{
public:

    V2xGeobroadcastArea();
    /**
    * @brief Constructor.
    * @param[in] subscriptionId Subscription identifier that requested the area.
    * @param[in] frequency Frequency of message transmission within the area.
    * @param[in] payloadLength Length of the message payload.
    */
    V2xGeobroadcastArea(int subscriptionId, float frequency, unsigned int payloadLength);

    /// @brief Destructor.
    ~V2xGeobroadcastArea();

    /// @brief Subscription identifier that requested the area.
    int m_subscriptionId;

    /**
     * @brief Frequency of CAM beaconing in the area.
     * @note Set to 2Hz by default.
     */
    float m_frequency;

    /**
     * @brief Length of the message payload.
     * @note Set to 60 (bytes) by default.
     */
    unsigned int m_payloadLength;
};

}

#endif