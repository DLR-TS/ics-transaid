/****************************************************************************/
/// @file    StationFacilities.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 15, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef STATIONFACILITIES_H_
#define STATIONFACILITIES_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <random>
using namespace std;

#include "../mapFacilities/MapFacilities.h"
#include "MobileStation.h"
#include "FixedStation.h"


namespace ics_facilities {

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class StationFacilities
* @brief Contains the data structure containing the stations ever entered in the scenario and provides methods to get and set information about these stations.
*/
class StationFacilities {
public:

    /**
    * @brief Constructor.
    * @param[in] mapFac Pointer to the MapFacilities class.
    */
    StationFacilities(MapFacilities* mapFac);

    /**
    * @brief Destructor.
    */
    virtual ~StationFacilities();

    /**
    * @brief Configures given the information provided in the input file.
    * @param[in] stationConfigFilename Name and path of the file to be consider for the configuration.
    * @return True if the configuration succeeds. False otherwise.
    */
    bool configure(string stationConfigFilename);

    /**
    * @brief Returns the pointer to a given station (NULL if the station does not exist).
    * @param[in] stationID ID of the station.
    */
    const Station* getStation(stationID_t stationID) const;

    /**
    * @brief Returns the pointer to a given mobile station (NULL if the station does not exist).
    * @param[in] mobileStationID ID of the mobile station.
    */
    const MobileStation* getMobileStation(stationID_t mobileStationID) const;

    /**
    * @brief Returns the pointer to a given fixed station (NULL if the station does not exist).
    * @param[in] fixedStationID ID of the fixed station.
    */
    const FixedStation* getFixedStation(stationID_t fixedStationID) const;

    /**
    * @brief Returns a map structure with the ids of the stations (keys) and the pointer to them (values).
    */
    const map<stationID_t, Station*>& getAllStations() const;

    /**
    * @brief Returns a pointer to a map structure with the ids of the mobile stations (keys) and the pointer to them (values).
    */
    map<stationID_t, const MobileStation*>* getAllMobileStations();

    /**
    * @brief Returns a pointer to a map structure with the ids of the fixed stations (keys) and the pointer to them (values).
    */
    map<stationID_t, const FixedStation*>* getAllFixedStations();

    /**
    * @brief Given a vector of areas, it returns the pointers to the stations inside.
    * @param[in] Area expressed as geometric shape (circle, rectangle, ellipse, or convex polygon)
    * @return Pointer to the vector of pointers to the stations
    */
    map<stationID_t, const Station*>* getStationsInArea(GeometricShape &area);

    /**
    * @brief Given a vector of areas, it returns the pointers to the stations inside.
    * @param[in] Vector of area pointers expressed as road elements (i.e., lane, edge or junctions).
    * @return Pointer to the vector of pointers to the stations
    */
    map<stationID_t, const Station*>* getStationsInArea(vector<RoadElement*> &area);

    /**
    * @brief Given a vector of areas, it returns the pointers to the mobile stations inside.
    * @param[in] Area expressed as geometric shape (circle, rectangle, ellipse, or convex polygon)
    * @return Pointer to the vector of pointers to the stations
    */
    map<stationID_t, const MobileStation*>* getMobileStationsInArea(GeometricShape &area);

    /**
    * @brief Given a vector of areas, it returns the pointers to the mobile stations inside.
    * @param[in] Vector of area pointers expressed as road elements (i.e., lane, edge or junctions).
    * @return Pointer to the vector of pointers to the stations
    */
    map<stationID_t, const MobileStation*>* getMobileStationsInArea(vector<RoadElement*> &area);

    /**
    * @brief Given a vector of areas, it returns the pointers to the fixed stations inside.
    * @param[in] Area expressed as geometric shape (circle, rectangle, ellipse, or convex polygon)
    * @return Pointer to the vector of pointers to the stations
    */
    map<stationID_t, const FixedStation*>* getFixedStationsInArea(GeometricShape &area);

    /**
    * @brief Given a vector of areas, it returns the pointers to the fixed stations inside.
    * @param[in] Vector of area pointers expressed as road elements (i.e., lane, edge or junctions).
    * @return Pointer to the vector of pointers to the stations
    */
    map<stationID_t, const FixedStation*>* getFixedStationsInArea(vector<RoadElement*> &area);

    /**
    * @brief Check if a given station is inside a geometric area.
    * @param[in] stationID ID of the station.
    * @param[in] area Reference to the area.
    * @return True if the station is found. False otherwise.
    */
    bool isStationInArea(stationID_t stationID, GeometricShape &area);

    /**
    * @brief Check if a given station is inside an area defined by a given road element.
    * @param[in] stationID ID of the station.
    * @param[in] area Reference to the road element.
    * @return True if the station is found. False otherwise.
    */
    bool isStationInArea(stationID_t stationID, RoadElement &area);

    /**
    * @brief Check if a given station is inside an area defined by a vector of geometric shapes and road elements.
    * @param[in] stationID ID of the station.
    * @param[in] area Vector of areas.
    * @return True if the station is found. False otherwise.
    */
    bool isStationInArea(stationID_t stationID, vector<Area2D*> &area);

    /**
    * @brief Updates the dynamic information (position, speed, etc...) related to mobile stations.
    * @param[in] ID of the station and information to update.
    * @return void.
    */
    bool updateMobileStationDynamicInformation(stationID_t stationId, TMobileStationDynamicInfo info);

    /**
    * @brief Enables the radio access technology of a given station, if the technology is available.
    * @param[in] ID of the station and RAT ID to be enabled.
    * @return true if enabled, false if not available.
    */
    bool enableRAT(stationID_t stationID, RATID RATtoBeEnabled);

    /**
    * @brief Disables the radio access technology of a given station, if the technology is available.
    * @param[in] ID of the station and RAT ID to be disabled.
    * @return true if disabled, false if not available.
    */
    bool disableRAT(stationID_t stationID, RATID RATtoBeDisabled);

    /**
    * @brief Enables the radio access technology in all the stations, if the technology is available.
    * @param[in] RAT ID to be enabled.
    * @return true if all the stations have enabled it, false otherwise.
    */
    bool enableRATAllStations(RATID RATtoBeEnabled);

    /**
    * @brief Disables the radio access technology in all the stations, if the technology is available.
    * @param[in] RAT ID to be disabled.
    * @return true if all the stations have disabled it, false otherwise.
    */
    bool disableRATAllStations(RATID RATtoBeDisabled);

    /**
    * @brief Returns the default penetration rates for all the radio access technologies.
    * @return a map containing the radio access technology (key) and the penetration rate (value).
    */
    const map<RATID, float>& getDefaultPenetrationRates() const;

    /**
    * @brief Returns the default communication profiles (used by the network simulator) for all the radio access technologies.
    * @return a map containing the radio access technology (key) and the associated communication profile (value).
    */
    const map<RATID, string>& getDefaultCommunicationProfiles() const;

    bool isStationOfType(stationID_t stationID, icsstationtype_t type);

    //map<stationID_t, Point2D > getPositionFromMobilityHistory(); //TTN (18/06/2016)
    Point2D getNodePositionFromMobilityHistory(icstime_t time, stationID_t stationId);

private:

    ///@brief Dictionary of Station  pointers (the key is the Station ID)
    map<stationID_t, Station*> stations;

    ///@brief Pointer to the MapFacilities
    MapFacilities* mapFac;

    ///@brief Penetration rates for mobile stations are between 0 and 100
    map<RATID, float> defaultPenetrationRates;

    ///@brief Communication profiles for the mobile stations given the RATs
    map<RATID, string> defaultCommunicationProfiles;

    ///@brief Random generator used to assign the radio access technologies to mobile nodes entering the scenario.
    std::mt19937 staFacRand;

    ///@brief This variable allows to generate a trace file containing the position history of all the vehicles.
    bool recordMobilityHistory;

    ///@brief This data structure stores for all the vehicles their position history. This data structure is used ONLY 'recordMobilityHistory' is true.
    map<icstime_t, vector<pair<stationID_t, Point2D> > > mobilityHistory;

    ///@brief This variable contains the trace file containing the position history of all the vehicles. The file is generated when the class destructor is called.
    string mobilityHistoryFilename;

    /**
    * @brief Given a pointer to a Lane object, it returns which stations are on that lane.
    * @param[in] lane Pointer to a Lane object.
    * @param[out] stationsOnLane Dictionary of Station  pointers (the key is the Station ID)
    */
    void getStationsOnLane(const Lane* lane, map<stationID_t, const Station*>* stationsOnLane);

    /**
    * @brief Given a pointer to a Lane object, it returns which mobile stations are traveling over that lane.
    * @param[in] lane Pointer to a Lane object.
    * @param[out] mobileStationsOnLane Dictionary of MobileStation  pointers (the key is the Station ID)
    */
    void getMobileStationsOnLane(const Lane* lane, map<stationID_t, const MobileStation*>* mobileStationsOnLane);

    /**
    * @brief Given a pointer to a Lane object, it returns which fixed stations are on that lane.
    * @param[in] lane Pointer to a Lane object.
    * @param[out] fixedStationsOnLane Dictionary of FixedStation  pointers (the key is the Station ID)
    */
    void getFixedStationsOnLane(const Lane* lane, map<stationID_t, const FixedStation*>* fixedStationsOnLane);

    /**
    * @brief Update the history of the vehicle's position. This method is called only if 'recordMobilityHistory' is true.
    */
    void updateMobilityHistory(stationID_t vehicle, icstime_t time, Point2D pos);

    /**
    * @brief Create a file with the position history of the vehicles per each time step.
    */
    void createMobilityHistoryFile();

};

}

#endif /* STATIONFACILITIES_H_ */
