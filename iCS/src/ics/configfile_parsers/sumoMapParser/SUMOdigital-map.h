/****************************************************************************/
/// @file    SUMOdigital-map.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Jan 8, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

/*! \file SUMOdigital-map.h
 \brief Data Structure and classes for the iTetris DigitalMap block.
*/

#ifndef SUMODIGITALMAP_H
#define SUMODIGITALMAP_H

// ===========================================================================
// included modules
// ===========================================================================


#ifdef SUMO_ON

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <utils/xml/SUMOSAXHandler.h>
#include "../../../utils/ics/geometric/Point2D.h"

using namespace std;



/*! \namespace sumo_map
 \brief Namespace for the DigitalMap block

 The namespace "sumo_map" contains the data structure of the DigitalMap block and the methods to load it from a SUMO net.xml file.
 */
namespace sumo_map {

using namespace ics_types;

/*
 * ****************
 *    STRUCTURES
 * ****************
 */

/*! \struct SUMOLocation
 \brief General information about the digital map (size and offset).

 The structure reflects the SUMO's one.
*/
struct SUMOLocation {
    Point2D netOffset;                              /**< Offset of the map */
    Point2D convBoundaryMin;                        /**< Farther South-West point on the map */
    Point2D convBoundaryMax;                        /**< Farther North-East point on the map */
    Point2D origBoundaryMin;                        /**< Farther South-West point on the original map */
    Point2D origBoundaryMax;                        /**< Farther North-East point on the original map */
    std::string projParameter;                      /**< optional parameter for proj. */

    SUMOLocation() {}                               /**< Initializer */
};

struct SUMOConnection {
    std::string fromEdge;
    std::string toEdge;
    unsigned int fromLane;
    unsigned int toLane;
    //string lane;                                    /**< ID of the lane that is connected after the junction. */
    string via;                                     /**< ID of the lane (internal of the junction) that connects the reference lane to the one with ID 'lane' */
    string tl;                                      /**< Traffic light program used to manage the reference lane. */
    short linkIndex;                                /**< Index of the signal within this traffic light. */
    //bool yield;                                     /**< Application of the yield rule. */
    char dir;                                       /**< direction: {'s' = STRAIGHT, 'l' = LEFT, 'r' = RIGHT, 't' = TURN, 'L' = PARTLEFT, 'R' = PARTRIGHT}. */
    char state;                                     /**< connection: {'M' = MAYOR, 'm' = MINOR}. */
    //char int_end;                                   /**<  */

    SUMOConnection() {}                             /**< Initializer */
};


/*! \struct SUMOPhase
 \brief Structure that gives information about the SUMOPhase of the reference traffic light program.
*/
struct SUMOPhase {
    float duration;                                    /**< Duration of the traffic light program's SUMOPhase. */
    float minDuration;
    float maxDuration;
    string state;                                    /**< Traffic light program's state. */

    SUMOPhase():                                         /**< Initializer */
            duration(0), state("") {};
};

/*! \struct SUMOLogicitem
 \brief Structure that gives information about the traffic light logic.
*/
struct SUMOLogicitem {
    short request;                                    /**<  */
    string response;                                /**<  */
    string foes;                                    /**<  */
    bool cont;                                        /**<  */

    SUMOLogicitem():                                    /**< Initializer. */
            request(0), response(""), foes(""), cont(false) {};
};



/*
 * ****************
 *  COMMON CLASSES
 * ****************
 */

/*! \class SUMOLane
 \brief Class that contains the information about a lane and its previous and next ones.
*/
class SUMOLane {
public:
    SUMOLane() {}                                            /**< Constructor. */
    virtual ~SUMOLane() {}                                /**< Destructor. */

    // Variables
    string id;                                        /**< ID of the lane. */
    //float depart;                                    /**< Starting time for the lane to be active [s]. */
    float maxspeed;                                    /**< Max speed in [m/s]. */
    float length;                                    /**< Total length of the lane. */
    float width;
    SVCPermissions permissions;
    vector<Point2D> shape;                            /**< Shape of the lane, defined by a set of points. */
    vector<SUMOLane *> nextSUMOLanes;                        /**< Vector of pointers to the next lanes. */
    vector<SUMOLane *> prevSUMOLanes;                        /**< Vector of pointers to the previous lanes. */
};

/*! \class SUMOEdge
 \brief Class that contains the information about an edge (i.e. road).
*/
class SUMOEdge {
public:
    SUMOEdge() {}                                           /**< Constructor. */
    virtual ~SUMOEdge() {}                                /**< Destructor. */

    // Variables
    string id;                                  /**< ID of the edge. */
    string stringFrom;                          /**< ID of the previous junction. */
    string stringTo;                            /**< ID of the next junction. */
    SumoXMLEdgeFunc funct;                      /**< function of the edge. */
    bool inner;                               /**< indicates that the edge is inside of a junction. */
    map<string, SUMOLane> lanes;                /**< Set of lanes contained by the edge.*/
}; 

/*! \class SUMOJunction
 \brief Class that contains the information about a junction.
*/
class SUMOJunction {
public:
    SUMOJunction() {};                                    /**< Constructor. */
    virtual ~SUMOJunction() {};                            /**< Destructor. */

    // Variables
    string id;
    SumoXMLNodeType type;                                    /**< Type of the junction {priority, internal, DEAD_END}. */
    Point2D center;                                    /**< Geometric center of the junction. */
    vector<string> stringIncSUMOLanes;                    /**< IDs of the incoming lanes. */
    vector<string> stringIntSUMOLanes;                    /**< IDs of the internal lanes. */
    vector<Point2D> shape;                            /**< Shape of the junction, defined by a set of points. */
};

/*! \class SUMOTLlogic
 \brief Class that contains the information about a traffic light logic.
*/
class SUMOTLlogic {
public:
    SUMOTLlogic() {};                                    /**< Constructor */
    virtual ~SUMOTLlogic() {};                            /**< Destructor */

    // Variables
    string id;                                        /**< ID of traffic light logic. */
    string type;                                    /**<  */
    string programID;                                /**< ID of the traffic light program. */
    float offset;
    vector<SUMOPhase> phases;                            /**< Definition of the SUMOPhases (sequence of red and green). */
};

///*! \class RowLogic
// \brief This class reflects the SUMO's one.
//*/
//class RowLogic {
//public:
//    RowLogic() {};                                    /**< Constructor */
//    virtual ~RowLogic() {};                            /**< Destructor */
//
//    // Variables
//    string id;                                        /**<  */
//    short requestSize;                                /**<  */
//    short laneNumber;                                /**<  */
//    vector<SUMOLogicitem> logics;                        /**<  */
//};


/*
 * ****************
 *   EXTRAS
 * ****************
 */

/*! \struct SUMOTrafficlight
 \brief Structure that contains all the information about a traffic light.

 This structure does not exist in SUMO.
*/
struct SUMOTrafficlight {
    short icsID;                   /**< ID of the traffic light in the map. */
    string tlID;                   /**< Traffic light ID for this signal. */
    short linkIndex;               /**< ID of the traffic light in the map. */
    Point2D pos;                   /**< 2D-position of the traffic light. */
    SUMOLane *controlled;          /**< Pointer to the controlled lane. */
    SUMOLane *via;                 /**< Pointer to the lane of the junction that connects the controlled lane to the next one (succ). */
    SUMOLane *succ;                /**< Pointer to the next lane. */
    char direction;                /**< direction indicated by the traffic light. */

    SUMOTrafficlight():                                 /**< Initializer. */
            icsID(-1), direction(' ') {};
};



/*
 * ****************
 *    DIGITAL MAP
 * ****************
 */

/*! \class SUMODigitalMap
 \brief This class contains all the information about a digital map.
*/
class SUMODigitalMap : public SUMOSAXHandler {
public:
    // Constructors/Destructor
    SUMODigitalMap();                              /**< Void constructor. */
    ~SUMODigitalMap();                             /**< Destructor. */

    // Public variables
    SUMOLocation location;                         /**< general parameters of the map. */
    std::vector<SUMOConnection> connections;
    map<string, SUMOEdge> edges;                   /**< edges (roads or links) of the topology map. */
    map<string, SUMOJunction> junctions;           /**< junctions (usually intersections) of the topology map. */
    map<string, SUMOTLlogic> tllogics;             /**< traffic light logics. */
    vector<SUMOTrafficlight> trafficlights;        /**< vector containing the traffic lights positions and lanes. */

    // Public functions
    bool loadMap(string filename);                 /**< load the topology map contained in the file 'filename'. */
    SUMOLane *findSUMOLane(const std::string &laneID);                /**< returns the pointer to the SUMOLane object given the lane id. */


protected:
    /**
     * @brief Callback method for an opening tag to implement by derived classes
     *
     * Called by "startElement" (see there).
     * @param[in] element The element that contains the characters, given as a int
     * @param[in] attrs The SAX-attributes, wrapped as SUMOSAXAttributes
     * @exceptions ProcessError These method may throw a ProcessError if something fails
     */
    void myStartElement(int element, const SUMOSAXAttributes& attrs);


    /** @brief Callback method for a closing tag to implement by derived classes
     *
     * Called by "endElement" (see there).
     * @param[in] element The closed element, given as a int
     * @exceptions ProcessError These method may throw a ProcessError if something fails
     */
    void myEndElement(int element);


private:
    // Private variables
    SUMOEdge myCurrentEdge;
    SUMOTLlogic myCurrentTLLogic;


    void connectSUMOLanes();
    void createTrafficLightList();
};

}

#endif /*SUMO_ON*/

#endif /*SUMODIGITALMAP_H_*/
