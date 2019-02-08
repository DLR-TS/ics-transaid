/****************************************************************************/
/// @file    SUMOdigital-map.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Jan 8, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

/*! \file SUMOdigital-map.cpp
 \brief Parsing and loading of the DigitalMap information.
*/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "SUMOdigital-map.h"
#include <utils/xml/XMLSubSys.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>

#ifdef SUMO_ON

namespace sumo_map {

#include <stdlib.h>
using namespace std;


/*
 *
 * SUMODigitalMap
 *
 */
SUMODigitalMap::SUMODigitalMap() {
}


SUMODigitalMap::~SUMODigitalMap() {
}

/*! \fn bool SUMODigitalMap::loadMap(string filename)
 \brief Load the data stored in filename (that will be stored as NETfilename).
 It uses loadMap().
 \param[in] filename string with the name (and the path) of the file to be parsed.
 \param[out] true if the loading process succeeded, false otherwise (i.e. file not found).
 */
bool SUMODigitalMap::loadMap(string filename) {
    if(!XMLSubSys::runParser(*this, filename)) {
        return false;
    }
    connectSUMOLanes();
    createTrafficLightList();
    return true;
}


void 
SUMODigitalMap::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    bool ok = true;
    switch(element) {
    case SUMO_TAG_LOCATION: {
        PositionVector s = attrs.get<PositionVector>(SUMO_ATTR_NET_OFFSET, 0, ok);
        location.netOffset.setx(s[0].x());
        location.netOffset.sety(s[0].y());

        Boundary convBoundary = attrs.get<Boundary>(SUMO_ATTR_CONV_BOUNDARY, 0, ok);
        location.convBoundaryMin.setx(convBoundary.xmin());
        location.convBoundaryMin.sety(convBoundary.ymin());
        location.convBoundaryMax.setx(convBoundary.xmax());
        location.convBoundaryMax.sety(convBoundary.ymax());

        Boundary origBoundary = attrs.get<Boundary>(SUMO_ATTR_ORIG_BOUNDARY, 0, ok);
        location.origBoundaryMin.setx(convBoundary.xmin());
        location.origBoundaryMin.sety(convBoundary.ymin());
        location.origBoundaryMax.setx(convBoundary.xmax());
        location.origBoundaryMax.sety(convBoundary.ymax());

        location.projParameter = attrs.get<std::string>(SUMO_ATTR_ORIG_PROJ, 0, ok);
                            }
        break;
    case SUMO_TAG_EDGE: {
        myCurrentEdge = SUMOEdge();
        myCurrentEdge.id = attrs.get<std::string>(SUMO_ATTR_ID, myCurrentEdge.id.c_str(), ok);
        myCurrentEdge.stringFrom = attrs.get<std::string>(SUMO_ATTR_FROM, myCurrentEdge.id.c_str(), ok);
        myCurrentEdge.stringTo = attrs.get<std::string>(SUMO_ATTR_TO, myCurrentEdge.id.c_str(), ok);
        myCurrentEdge.funct = attrs.getEdgeFunc(ok);
        myCurrentEdge.inner = myCurrentEdge.funct == EDGEFUNC_INTERNAL;
                            }
        break;
    case SUMO_TAG_LANE: {
        SUMOLane l;
        l.id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
        l.maxspeed = attrs.get<SUMOReal>(SUMO_ATTR_SPEED, l.id.c_str(), ok);
        l.length = attrs.get<SUMOReal>(SUMO_ATTR_LENGTH, l.id.c_str(), ok);
        std::string allow;
        try {
            bool dummy;
            allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, l.id.c_str(), dummy, "", false);
        } catch (EmptyData e) {
            // !!! deprecated
        }
        std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, l.id.c_str(), ok, "");
        l.width = attrs.getOpt<SUMOReal>(SUMO_ATTR_WIDTH, l.id.c_str(), ok, SUMO_const_laneWidth);
        PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, l.id.c_str(), ok);
        if (shape.size() < 2) {
            //WRITE_ERROR("Shape of lane '" + l.id + "' is broken.");
            ok = false;
        }
        for(std::vector<Position>::const_iterator i=shape.begin(); i!=shape.end(); ++i) {
            l.shape.push_back(Point2D((*i).x(), (*i).y()));
        }
        l.permissions = parseVehicleClasses(allow, disallow);
        myCurrentEdge.lanes.insert(pair<string, SUMOLane> (l.id, l));
                            }
        break;

    case SUMO_TAG_TLLOGIC: {
        myCurrentTLLogic = SUMOTLlogic();
        myCurrentTLLogic.id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
        myCurrentTLLogic.type = attrs.get<std::string>(SUMO_ATTR_TYPE, myCurrentTLLogic.id.c_str(), ok);
        myCurrentTLLogic.offset = attrs.getOpt<SUMOReal>(SUMO_ATTR_OFFSET, myCurrentTLLogic.id.c_str(), ok, 0);
        myCurrentTLLogic.programID = attrs.getOpt<std::string>(SUMO_ATTR_PROGRAMID, myCurrentTLLogic.id.c_str(), ok, "<unknown>");
                           }
        break;

    case SUMO_TAG_PHASE: {
        SUMOPhase p;
        p.state = attrs.get<std::string>(SUMO_ATTR_STATE, myCurrentTLLogic.id.c_str(), ok);
        p.duration = attrs.get<SUMOReal>(SUMO_ATTR_DURATION, myCurrentTLLogic.id.c_str(), ok);
        p.minDuration = attrs.get<SUMOReal>(SUMO_ATTR_MINDURATION, myCurrentTLLogic.id.c_str(), ok);
        p.maxDuration = attrs.get<SUMOReal>(SUMO_ATTR_MAXDURATION, myCurrentTLLogic.id.c_str(), ok);
        myCurrentTLLogic.phases.push_back(p);
                           }
        break;

    case SUMO_TAG_JUNCTION: {
        SUMOJunction j;
        j.id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
        j.type = attrs.getNodeType(ok);
        SUMOReal x = attrs.get<SUMOReal>(SUMO_ATTR_X, j.id.c_str(), ok);
        SUMOReal y = attrs.get<SUMOReal>(SUMO_ATTR_Y, j.id.c_str(), ok);
        j.center = Point2D(x, y);
        j.stringIntSUMOLanes = attrs.getOptStringVector(SUMO_ATTR_INTLANES, j.id.c_str(), ok);
        j.stringIncSUMOLanes = attrs.getOptStringVector(SUMO_ATTR_INCLANES, j.id.c_str(), ok);
        PositionVector shape;
        if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
            // inner junctions have no shape
            shape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, j.id.c_str(), ok, PositionVector());
        }
        for(std::vector<Position>::const_iterator i=shape.begin(); i!=shape.end(); ++i) {
            j.shape.push_back(Point2D((*i).x(), (*i).y()));
        }
        junctions.insert(pair<string, SUMOJunction> (j.id, j));
                           }
        break;


    case SUMO_TAG_CONNECTION: {
        SUMOConnection c;
        c.fromEdge = attrs.get<std::string>(SUMO_ATTR_FROM, 0, ok);
        c.toEdge = attrs.get<std::string>(SUMO_ATTR_TO, 0, ok);
        c.fromLane = attrs.get<int>(SUMO_ATTR_FROM_LANE, 0, ok);
        c.toLane = attrs.get<int>(SUMO_ATTR_TO_LANE, 0, ok);
        c.dir = attrs.get<std::string>(SUMO_ATTR_DIR, 0, ok)[0];
        c.state = attrs.get<std::string>(SUMO_ATTR_STATE, 0, ok)[0];
        c.tl = attrs.getOpt<std::string>(SUMO_ATTR_TLID, 0, ok, "");
        c.via = attrs.getOpt<std::string>(SUMO_ATTR_VIA, 0, ok, "");
        if(c.tl!="") {
            c.linkIndex = (short) attrs.get<int>(SUMO_ATTR_TLLINKINDEX, 0, ok);
        }
        connections.push_back(c);
                           }
        break;


    default:
        break;
    }
}



void 
SUMODigitalMap::myEndElement(int element) {
    switch(element) {
    case SUMO_TAG_EDGE:
        edges.insert(pair<string, SUMOEdge> (myCurrentEdge.id, myCurrentEdge));
        break;
    case SUMO_TAG_TLLOGIC:
        tllogics.insert(pair<string, SUMOTLlogic> (myCurrentTLLogic.id, myCurrentTLLogic));
        break;
    }
}


void SUMODigitalMap::connectSUMOLanes() {
    for (std::vector<SUMOConnection>::const_iterator i=connections.begin(); i!=connections.end(); ++i) {
        string nextSUMOLaneId = "";
        if ((*i).via!="")
            nextSUMOLaneId = (*i).via;
        else
            nextSUMOLaneId = (*i).toEdge + "_" + toString((*i).toLane);

        // Find the lane called nextSUMOLaneId
        SUMOLane* nextSUMOLane = findSUMOLane(nextSUMOLaneId);
        if (nextSUMOLane != 0) {
            // Connect the currSUMOLane to nextSUMOLaneId by pointer
            std::string fromLaneID = (*i).fromEdge + "_" + toString((*i).fromLane);
            edges[(*i).fromEdge].lanes[fromLaneID].nextSUMOLanes.push_back(nextSUMOLane);
            // Connect the nextSUMOLaneId to currSUMOLane by pointer
            (*nextSUMOLane).prevSUMOLanes.push_back(&(edges[(*i).fromEdge].lanes[fromLaneID]));
        }
    }
}


void SUMODigitalMap::createTrafficLightList() {
    for (std::vector<SUMOConnection>::const_iterator i=connections.begin(); i!=connections.end(); ++i) {
        if ((*i).tl=="") 
            continue;
        std::string fromLaneID = (*i).fromEdge + "_" + toString((*i).fromLane);
        SUMOLane* currSUMOLane = findSUMOLane(fromLaneID);
        if (currSUMOLane == 0) 
            continue;
        Point2D currTLpos = (*currSUMOLane).shape.back();
        std::string toLaneID = (*i).toEdge + "_" + toString((*i).toLane);
        
        SUMOTrafficlight currTL;
        currTL.icsID = (short) trafficlights.size();
        currTL.pos = currTLpos;
        currTL.tlID = (*i).tl;
        currTL.linkIndex = (*i).linkIndex;
        currTL.controlled = currSUMOLane;
        currTL.succ = findSUMOLane(toLaneID);
        currTL.via = findSUMOLane((*i).via);
        currTL.direction = (*i).dir;
        // Add the TL to the vector
        trafficlights.push_back(currTL);
    }
    return;
}


SUMOLane* SUMODigitalMap::findSUMOLane(const std::string &laneID) {
    std::string edgeID = laneID.substr(0, laneID.rfind('_'));
    map<string, SUMOEdge>::iterator it = edges.find(edgeID);
    if(it==edges.end()) { throw 1; } // !!!
    map<string, SUMOLane>::iterator it2 = (*it).second.lanes.find(laneID);
    if (it2 != (*it).second.lanes.end())
        return &((*it2).second);
    else
        return 0;
}


}

#endif /*SUMO_ON*/
