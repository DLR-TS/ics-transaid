/****************************************************************************/
/// @file    subscriptions-type-contants.h
/// @author  Jerome Haerri, Pasquale Cataldi (EURECOM)
/// @date    December 15th, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

#ifndef SUBSCRIPTIONS_TYPE_CONSTANTS_H_
#define SUBSCRIPTIONS_TYPE_CONSTANTS_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

// ****************************************
// APPLICATION SUBSCRIPTION TYPES
// ****************************************

#define TYPE_ERROR			       			  0x00

#define EXT_HEADER_TYPE_TOPOBROADCAST       0x00
#define EXT_HEADER_TYPE_UNICAST             0x01
#define EXT_HEADER_TYPE_MULTICAST           0x02
#define EXT_HEADER_TYPE_GEOBROADCAST        0x03
#define EXT_HEADER_TYPE_GEOUNICAST          0x04
#define EXT_HEADER_TYPE_GEOANYCAST          0x05

#define EXT_HEADER__VALUE_BLOCK_IDs         0x00
#define EXT_HEADER__VALUE_BLOCK_AREAs       0x01
#define EXT_HEADER__VALUE_BLOCK_HOPS_No     0x02

#define EXT_HEADER__CIRCLE                  0x00
#define EXT_HEADER__ELLIPSE                 0x01
#define EXT_HEADER__CONV_POLYGON            0x02
#define EXT_HEADER__LANE_ID                 0x03
#define EXT_HEADER__EDGE_ID                 0x04
#define EXT_HEADER__JUNCTION_ID             0x05

#define EXT_HEADER__EXTRA                   0xAA

// ****************************************
// CROSS_APPLICATION SUBSCRIPTION TYPE
// ****************************************
#define TTE									0x01
#define JAM									0x02
#define EDGE_ID							0x03
#define ROUTE_ID						0x04
#define SPEED								0x05
#define TRAFFIC_INFO				0x06

#define VALUE_GET_DATA_BY_RESULT_CONTAINER_ID      0x01

// ***************************************
// FACILITIES SUBSCRIPTION TYPES
// ***************************************
#define TYPE_TOPO            0x00
#define TYPE_RECEIVED_CAM    0x01
#define TYPE_COMMUNICATION	 0x02

#define VALUE__POS_X         0x01
#define VALUE__POS_Y         0x02
#define VALUE__SPEED         0x03
#define VALUE__DIRECTION     0x04
#define VALUE__ACCELERATION  0x05
#define VALUE__LANE_ID       0x06
#define VALUE__EDGE_ID       0x07
#define VALUE__JUNCTION_ID   0x08

#define VALUE__ALL_ID        0x11
#define VALUE__LIST_ID       0x12
#define VALUE__NODE_POS      0x13
#define VALUE__SELECT_POS    0x14
#define VALUE__LIST_IDS    	 0x15
#define VALUE__ZONE_ID       0x16


#define VALUE_SET_EDGE_TRAVELTIME       0x21
#define VALUE_GET_EDGE_TRAVELTIME       0x22
#define VALUE_RE_ROUTE                  0x23
#define VALUE_GET_ROUTE_VARIABLE        0x24
#define VALUE_GET_SPEED                 0x25
#define VALUE_SET_SPEED                 0x26
#define VALUE_SET_TRAFFIC_LIGHT         0X27
#define VALUE_GET_MAX_SPEED        			0x28
#define VALUE_GET_VEHICLE_CLASS     		0X29
#endif /* SUBSCRIPTIONS_TYPE_CONSTANTS_H_ */
