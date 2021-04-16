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
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez
 *                          EURECOM (www.eurecom.fr), EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Ramon Bauza <rbauza@umh.es>, Michele Rondinone <mrondinone@umh.es>, Jerome Haerri <Jerome.Haerri@eurecom.fr>
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#include "ns3/node.h"

#ifndef ITETRIS_TYPES_H
#define ITETRIS_TYPES_H
#include <list>
#include "itetris-technologies.h"

namespace ns3 {

// EU FP7 COLOMBO extensions

// Downlink Generic Container Types Constants (from Application to ns-3)
// used for the storage container which is decoded at the Facilities

//Txon Type
#define TYPE_TOPO            0x00
#define TYPE_RECEIVED_CAM    0x01
#define TYPE_COMMUNICATION   0x02

//Field Type
#define VALUE__POS_X         0x01
#define VALUE__POS_Y         0x02
#define VALUE__SPEED         0x03
#define VALUE__DIRECTION     0x04
#define VALUE__ACCELERATION  0x05
#define VALUE__LANE_ID       0x06
#define VALUE__EDGE_ID       0x07
#define VALUE__JUNCTION_ID   0x08

// Uplink Generic Container Types Constants (from ns-3 to Application)
// used for the container which is encoded at the iTETRIS Application
#define TAG_RSSI 		0x01
#define TAG_SNR 		0x02
#define TAG_TXPOWER 	0x03
#define TAG_MSGID 	0x04

// End of EU FP7 COLOMBO extensions

const uint32_t ID_BROADCAST =  900000000;

const uint32_t TOPO_BROADCAST =  900000001;

const uint32_t GEO_UNICAST =  900000002;

const uint32_t ID_MULTICAST =  900000003;

const uint32_t TMC_CONSTANT =  900000004;

enum STACK { IPv4, IPv6, C2C };

enum TransmissionMode { IP_BROADCAST, IP_MULTICAST, C2C_GEOBROADCAST, C2C_GEOANYCAST, C2C_TOPOBROADCAST };

enum ServiceProfile {CAM, DEMN, APP, APP_ITSG5};

//enum TrafficClass {TC1, TC2, TC3};

typedef std::vector< std::string > TechnologyList;

typedef struct CircularGeoAddress {
    uint32_t lat;
    uint32_t  lon;
    uint32_t areaSize;
} CircularGeoAddress;

typedef struct DissProfile {
    STACK stack;
    NetDeviceType tech;
    Ptr<Node> disseminator;
} disseminationProfile;

typedef struct StackToDestination {
    STACK stack;
    uint32_t destination;
    char* tech; /*TrafficClass tclass;*/
} stacktodestination;

}

#endif
