/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#ifndef NS3_COMM_CONSTANTS_H
#define NS3_COMM_CONSTANTS_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef _WIN32
#include <stdint.h>
#endif

// ****************************************
// COMMANDS
// ****************************************

// command: simulation step
#define CMD_SIMSTEP	0x01

// command: update node position
#define CMD_UPDATENODE	0x02

//command: create a new CIU node
#define CMD_CREATECIU	0x03

//command: create a new vehicle node
#define CMD_CREATEVEHICLE	0x04

//command: start sending CAM
#define CMD_START_CAM	0x05

//command: start sending CAM
#define CMD_STOP_CAM	0x06

//command: get nodes that received messages
#define CMD_GET_RECEIVED_MESSAGES	0x07

//command: create a new node (vehicle or CIU)
#define CMD_CREATENODE	0x08

// Activates a topobroadcast transmission using WAVE and the C2C stack
#define CMD_START_TOPO_TXON 0x09

// Activates a transmission based on the destination node ID. This txon mode
// can be used to active a unicast or broadcast transmission in a vehicle or
// a RSU. It works with different radio access technologies (if a vehicle is
// the sender, e.g. WAVE, UMTS, etc.) and the C2C and IP stacks.
#define CMD_START_ID_BASED_TXON 0x0A

// Activates a transmission based on the destination node ID. This txon mode
// can be used to active a unicast, broadcast or multicast transmission in a IP-based
// base station or CIU (e.g. UMTS base stations, WiMAX base station, etc.)
#define CMD_START_IPCIU_TXON 0x0B

// Deactivates the txon of a service running in a vehicle or a RSU.
#define CMD_STOP_SERVICE_TXON 0x0C

// Deactivates the txon of a service running in IP-based base station or CIU.
#define CMD_STOP_IPCIU_SERVICE_TXON 0x0D

//command: get nodes that received messages
#define CMD_GET_RECEIVED_MESSAGES2 0x0E

// Activates a geobroadcast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined
// by its radius (Lat and Lon coordinates) and center point (meters).
#define CMD_START_GEO_BROAD_TXON 0x0F

// Activates a geoanycast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined
// by its radius (Lat and Lon coordinates) and center point (meters).
#define CMD_START_GEO_ANY_TXON 0x10

// Activate a node and all its communication modules, e.g. PHY layer
#define CMD_ACTIVATE_NODE 0x11

// Deactivate a node and all its communication modules, e.g. PHY layer
#define CMD_DEACTIVATE_NODE 0x12

// New 16-09-10
#define CMD_CREATENODE2	0x13
#define CMD_UPDATENODE2	0x14

// command: activate a MW transmision on the TMC
#define CMD_START_MW_TXON 0x15

// command: deactivate the MW txon of a service running in the TMC.
#define CMD_STOP_MW_SERVICE_TXON 0x16

// command: get all the messages from all the active nodes
#define CMD_GET_ALL_RECEIVED_MESSAGES 0x17

// command: close
#define CMD_CLOSE   0x7F

// ****************************************
// MESSAGE TYPES
// ****************************************

// message type: CAM
//#define CAM_TYPE	0x01

// message type: DNEM
//#define DNEM_TYPE	0x02

// ****************************************
// RESULT TYPES
// ****************************************

// result type: Ok
#define NS3_RTYPE_OK    0x00
// result type: not implemented
#define NS3_RTYPE_NOTIMPLEMENTED  0x01
// result type: error
#define NS3_RTYPE_ERR   0xFF

#define ID_BROADCAST 900000000;

#define ID_MULTICAST 700000000;

#define TMC_CONSTANT 800000000;

/**
 * @struct CircularGeoAddress
 * @todo To be implemented.
 */
struct CircularGeoAddress {
    uint32_t lat;
    uint32_t  lon;
    uint32_t areaSize;
} ;

#endif
