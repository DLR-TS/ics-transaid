/****************************************************************************************
 * Copyright (c) 2015 The Regents of the University of Bologna.
 * This code has been developed in the context of the
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software must display
 * the following acknowledgement: ''This product includes software developed by the
 * University of Bologna and its contributors''.
 * 4. Neither the name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************************/
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/

#ifndef APP_COMM_SUBS_CONSTANTS_H
#define APP_COMM_SUBS_CONSTANTS_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows-config.h>
#else
#include "config.h"
#endif

#define DEBUG false

// ****************************************
// COMMANDS & SUBSCRIPTIONS
// ****************************************

// command: close connection
#define CMD_APP_CLOSE 0xFF

// command create new node
#define CMD_CREATE_MOBILE_NODE 0x01

// subscription: Return cars in a zone
#define SUB_RETURNS_CARS_IN_ZONE 0x02

// command: Send a message to a station.
#define CMD_SEND_MESSAGE_TO_STATION 0x03

// subscription: Request received messages status
#define SUB_REQ_RECEIVED_MESSAGES_STATUS 0x05

// command: Notify Application to execute
#define CMD_NOTIFY_APP_EXECUTE 0x06

// command: End of Application Execution
#define CMD_NOTIFY_APP_END_EXECUTE 0x07

// command End of Subscription
#define CMD_END_SUBSCRIPTION 0x08

// command Checks Applications readiness
#define CMD_APP_READY 0x09

// command Ask for subscriptions
#define CMD_ASK_FOR_SUBSCRIPTION 0x0A	//10

// command End of subscription requests
#define CMD_END_SUBSCRIPTION_REQUEST 0x0B	//11

// command Drop the subscription
#define CMD_DROP_SUBSCRIPTION 0x0C	//12

// command Renew the subscription
#define CMD_RENEW_SUBSCRIPTION 0x0D	//13

// command Sends returns cars in zone subscription data
#define CMD_CARS_IN_ZONE 0x0E	//14

// subscription Set area to send CAM
#define SUB_SET_CAM_AREA 0x11	//17

// notify app of arrived messages
#define CMD_NOTIFY_APP_MESSAGE_STATUS 0x14	//20

// subscription start travel time estimation
#define SUB_TRAVEL_TIME_ESTIMATION_START 0x12	//18

// subscription end travel time estimation
#define SUB_TRAVEL_TIME_ESTIMATION_END 0x13	//19

// subscription to monitor the travel time estimation
#define SUB_TRAVEL_TIME_ESTIMATION 0x15	//21

// subscription to monitor the travel time estimation
#define CMD_TRAVEL_TIME_ESTIMATION 0x20	//32

// subscription get received cam info
#define SUB_RECEIVED_CAM_INFO 0x18	//24

// command Sends returns received cam info data
#define CMD_RECEIVED_CAM_INFO 0x19	//25

// subscription to get the facilities information about a station
#define SUB_FACILITIES_INFORMATION 0x1C	//28

// send to the application the facilities info data
#define CMD_FACILITIES_INFORMATION 0x1D	//29

// subscription to send an application message
#define SUB_APP_MSG_SEND 0x1E	//30

// send to the application the status of the application message scheduling process
#define CMD_APP_MSG_SEND 0x1F	//31

// subscription to push an application cmd to the traffic simulator
#define SUB_APP_CMD_TRAFF_SIM 0x2A	//42

// send to the application the traffic simulator data
#define CMD_APP_CMD_TRAFF_SIM 0x2B	//43

// subscription to an application pull result from the traffic simulator
#define SUB_APP_RESULT_TRAFF_SIM 0x2C	//44

// send to the application the status of the traffic simulator application CMD process
#define CMD_APP_RESULT_TRAFF_SIM 0x2D	//45

// subscription to cross-application data
#define SUB_X_APPLICATION_DATA 0x2E	//46

// send to the application the cross application data
#define CMD_X_APPLICATION_DATA 0x2F	//47

// subscription to send an application message
#define SUB_APP_MSG_RECEIVE 0x3A	//58

// send to the application the status of the application message scheduling process
#define CMD_APP_MSG_RECEIVE 0x3B	//59

// send to the application the mobility information
#define CMD_MOBILITY_INFORMATION 0x41   //65

// subscription to get the mobility information
#define SUB_MOBILITY_INFORMATION 0x42   //66

// send to the application the traffic light information
#define CMD_TRAFFIC_LIGHT_INFORMATION 0x43  //67

// subscription to get the traffic light information
#define SUB_TRAFFIC_LIGHT_INFORMATION 0x44  //68

// send to the application the traci sumo results
#define CMD_SUMO_TRACI_COMMAND 0x45 //69

// subscription to execute a traci command in sumo
#define SUB_SUMO_TRACI_COMMAND 0x46	//70

// ****************************************
// OUTPUT OF APPLICATIONS
// Where the result of the application should be applied
// ****************************************

// Set maximum speed for a vehicle
#define OUTPUT_SET_VEHICLE_MAX_SPEED 0x04

// Calculate the travel time
#define OUTPUT_TRAVEL_TIME_ESTIMATION 0x21

// Traffic jam detection
#define OUTPUT_TRAFFIC_JAM_DETECTION 0x23

// Application without result
#define OUTPUT_VOID 0x22

// reply: The application executed its algorithm //15
#define APP_RESULT_ON 0x0F

// reply: The application DID NOT execute //16
#define APP_RESULT_OFF 0x10

#define PROTOCOL_MESSAGE 11

// ****************************************
// RESULT TYPES
// ****************************************

// result type: Ok
#define APP_RTYPE_OK    0x01
// result type: not implemented
#define APP_RTYPE_NOTIMPLEMENTED  0x02
// result type: error
#define APP_RTYPE_ERR   0xFF

// ****************************************
// MESSAGE STATUS
// ****************************************

// result : to schedule in ns-3
#define APP_RESULT_TO_SCHEDULE    0x15
// result : to be applied in traffic simulator
#define APP_RESULT_TO_APPLY  0x16
// result : discard it (do not apply)
#define APP_RESULT_TO_DISCARD   0x17

// ****************************************
// APPLICATION-BASED SUBSCRIPTIONS
// ****************************************

// APP_MSG_SEND and APP_MSG_RECEIVE Subscriptions
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

#define TYPE_ERROR			0x00
#define TYPE_EDGE_ID			0x01
#define	TYPE_STATION_ID			0x02
#define TYPE_EDGE_TRAVELTIME       	0x03
#define TYPE_EDGES_ID_FROM_ROAD_ID      0x04

// ****************************************
// CROSS_APPLICATION SUBSCRIPTION TYPE
// ****************************************
#define TTE				0x01
#define JAM				0x02
#define EDGE_ID				0x03
#define ROUTE_ID			0x04
#define SPEED				0x05
#define TRAFFIC_INFO				0x06

#define VALUE_GET_DATA_BY_RESULT_CONTAINER_ID      0x01

#define VAR_SPEED 0x40

#define VALUE__ALL_ID        0x11
#define VALUE__LIST_ID       0x12
#define VALUE__NODE_POS      0x13
#define VALUE__SELECT_POS    0x14
#define VALUE__LIST_IDS    	 0x15

#define VALUE_SET_EDGE_TRAVELTIME       0x21
#define VALUE_GET_EDGE_TRAVELTIME       0x22
#define VALUE_RE_ROUTE   	    					0x23
#define VALUE_GET_ROUTE_VARIABLE	 			0x24
#define VALUE_GET_SPEED 								0x25
#define VALUE_SET_SPEED 								0x26
#define VALUE_SET_TRAFFIC_LIGHT					0X27
#define VALUE_GET_VEHICLE_CLASS     		0X29
#endif
