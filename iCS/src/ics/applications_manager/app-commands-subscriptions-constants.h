/****************************************************************************/
/// @file    app-commands-subscriptions-constants.h
/// @author
/// Constant values to be shared between the iCS and the Applications.
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
/****************************************************************************************
 * Edited by Jerome Harri and Tien-Thinh Nguyen (jerome.haerri, tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2015-2016
 * SINETIC project under the Framework Programme,
 * .....
***************************************************************************************/
#ifndef APP_COMM_SUBS_CONSTANTS_H
#define APP_COMM_SUBS_CONSTANTS_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

// ****************************************
// COMMANDS & SUBSCRIPTIONS
// ****************************************

// command: close connection
#define CMD_APP_CLOSE 0xFF

// command: inform app about SUMO simulation step length
#define CMD_SUMO_STEPLENGTH 0x17 // 23

// command: inform app about current simulation step
#define CMD_NEW_SIMSTEP 0x16 // 22

// command create new node
#define CMD_CREATE_MOBILE_NODE 0x01

// command remove node
#define CMD_REMOVE_MOBILE_NODE 0xEE

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
#define CMD_ASK_FOR_SUBSCRIPTION 0x0A

// command End of subscription requests
#define CMD_END_SUBSCRIPTION_REQUEST 0x0B

// command Drop the subscription
#define CMD_DROP_SUBSCRIPTION 0x0C

// command Renew the subscription
#define CMD_RENEW_SUBSCRIPTION 0x0D

// command Sends returns cars in zone subscription data
#define CMD_CARS_IN_ZONE 0x0E

// subscription Set area to send CAM
#define SUB_SET_CAM_AREA 0x11

// notify app of arrived messages
#define CMD_NOTIFY_APP_MESSAGE_STATUS 0x14

// subscription start travel time estimation
#define SUB_TRAVEL_TIME_ESTIMATION_START 0x12

// subscription end travel time estimation
#define SUB_TRAVEL_TIME_ESTIMATION_END 0x13

// subscription to monitor the travel time estimation
#define SUB_TRAVEL_TIME_ESTIMATION 0x15

// subscription to monitor the travel time estimation
#define CMD_TRAVEL_TIME_ESTIMATION 0x20

// subscription get received cam info
#define SUB_RECEIVED_CAM_INFO 0x18

// send to the application the received cam info data
#define CMD_RECEIVED_CAM_INFO 0x19

// subscription to get the facilities information about a station
#define SUB_FACILITIES_INFORMATION 0x1C

// send to the application the facilities info data
#define CMD_FACILITIES_INFORMATION 0x1D

// subscription to send an application message
#define SUB_APP_MSG_SEND 0x1E

// send to the application the status of the application message scheduling process
#define CMD_APP_MSG_SEND 0x1F

// subscription to push an application cmd to the traffic simulator
#define SUB_APP_CMD_TRAFF_SIM 0x2A

// send to the application the traffic simulator data
#define CMD_APP_CMD_TRAFF_SIM 0x2B

// subscription to an application pull result from the traffic simulator
#define SUB_APP_RESULT_TRAFF_SIM 0x2C

// send to the application the status of the traffic simulator application CMD process
#define CMD_APP_RESULT_TRAFF_SIM 0x2D

// subscription to cross-application data
#define SUB_X_APPLICATION_DATA 0x2E

// send to the application the cross application data
#define CMD_X_APPLICATION_DATA 0x2F

// subscription to send an application message
#define SUB_APP_MSG_RECEIVE 0x3A

// send to the application the status of the application message scheduling process
#define CMD_APP_MSG_RECEIVE 0x3B

// send to the application the mobility information
#define CMD_MOBILITY_INFORMATION 0x41

// subscription to get the mobility information
#define SUB_MOBILITY_INFORMATION 0x42

// send to the application the traffic light information
#define CMD_TRAFFIC_LIGHT_INFORMATION 0x43

// subscription to get the traffic light information
#define SUB_TRAFFIC_LIGHT_INFORMATION 0x44

// send to the application the traci sumo results
#define CMD_SUMO_TRACI_COMMAND 0x45

// subscription to execute a traci command in sumo
#define SUB_SUMO_TRACI_COMMAND 0x46


// subscription to request data from TRACI
#define SUB_CONTROL_TRACI 0x4f //79

// subscription to the application to send TRACI results
#define CMD_CONTROL_TRACI 0x50

// move vehicle, VTD version (set: vehicle)
#define VAR_MOVE_TO_VTD 0xb4
// command: set vehicle variable
#define CMD_SET_VEHICLE_VARIABLE 0xc4


// ****************************************
// OUTPUT OF APPLICATIONS
// Where the result of the application should be applied
// ****************************************

// DEMO set maximum speed for a vehicle
#define OUTPUT_SET_SPEED_ADVICE_DEMO 0x25

// Set maximum speed for a vehicle
#define OUTPUT_SET_VEHICLE_MAX_SPEED 0x04

// Calculate the travel time
#define OUTPUT_TRAVEL_TIME_ESTIMATION 0x21

// Traffic jam detection
#define OUTPUT_TRAFFIC_JAM_DETECTION 0x23

// Traffic jam detection
#define OUTPUT_OPEN_BUSLANES 0x24

// Application without result
#define OUTPUT_VOID 0x22

// Application using the generic subscriptions
#define OUTPUT_GENERIC 0x26

// reply: The application executed its algorithm
#define APP_RESULT_ON 0x0F

// reply: The application DID NOT execute
#define APP_RESULT_OFF 0x10

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

#endif
