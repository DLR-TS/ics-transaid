/****************************************************************************/
/// @file    StationFacilities.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 15, 2010
/// @version $Id:
///
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
/****************************************************************************************
 * Added functionalities for SINETIC
 * Author: Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "StationFacilities.h"
#include "../../configfile_parsers/stations-configfile-parser.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>

using namespace std;

namespace ics_facilities {

StationFacilities::StationFacilities(MapFacilities* mapFac) {
	this->mapFac = mapFac;

	recordMobilityHistory = true;
	mobilityHistoryFilename = "position-log.txt";

}

StationFacilities::~StationFacilities() {
#ifdef _DEBUG_STATIONS
    cout << "[facilities] Write the mobility history file??????" << endl;
#endif
	if (recordMobilityHistory)
		createMobilityHistoryFile();

	if (!stations.empty()) {
		for (map<stationID_t, Station*>::iterator it = stations.begin(); it != stations.end(); it++) {
			delete(it->second);
		}
	}
	stations.clear();
}

bool StationFacilities::configure(string stationConfigFilename) {
	bool configFlag = false;

#ifdef _DEBUG_STATIONS
	cout << endl << "------------------------------------------------------" << endl;
	cout << "[facilities] StationFacilities: station settings going to be loaded from " << stationConfigFilename << endl;
#endif

	ics_parsing::StationsGetConfig staConfig;
	staConfig.readConfigFile(stationConfigFilename);

	RATseed = staConfig.getRATseed();
	staFacRand.seed(RATseed);

	map<int, float> penRates = staConfig.getDefaultPenetrationRates();
	map<int, float>::iterator itPenRates;
	for (itPenRates = penRates.begin(); itPenRates != penRates.end(); itPenRates++) {
		defaultPenetrationRates.insert(pair<RATID, float> ((RATID) itPenRates->first, itPenRates->second));
	}
	penRates.clear();

	map<int, string> commProfiles = staConfig.getMobileCommunicationProfiles();
	map<int, string>::iterator itCommProfile;
	for (itCommProfile = commProfiles.begin(); itCommProfile != commProfiles.end(); itCommProfile++)
		defaultCommunicationProfiles.insert(pair<RATID, string> ((RATID) itCommProfile->first, itCommProfile->second));
	commProfiles.clear();

	vector<ics_parsing::FixedStationStr> fixedStationStrVec = staConfig.getFixedStationCollection();
#ifdef _DEBUG_STATIONS
    cout << "[facilities] - Number of Fixed stations to be created: " << fixedStationStrVec.size() << endl;
#endif
vector<ics_parsing::FixedStationStr>::iterator itFixStaStrCol;
for (itFixStaStrCol = fixedStationStrVec.begin(); itFixStaStrCol
!= fixedStationStrVec.end(); itFixStaStrCol++) {

	stationID_t ID = itFixStaStrCol->m_ID;

	if (stations.find(ID) != stations.end()) {
		cerr << "[facilities] - ERROR: A station with ID " << ID << " was already inserted in the stations list. StationFacilities configuration failed!" << endl;
		return false;
	}

	Point2D position(itFixStaStrCol->m_X, itFixStaStrCol->m_Y);

	// NOTE: Fixed stations can have just ONE RAT
	vector< pair<RATID, bool> > RATs;
	pair<RATID, bool> RAT((RATID) itFixStaStrCol->m_RATtype, (bool) itFixStaStrCol->m_enabledRAT);
	RATs.push_back(RAT);

	string commProfile = itFixStaStrCol->m_communicationprofile;

	FixedStation* sta = new FixedStation(ID);
	sta->setPosition(position);
	sta->setRATs(RATs);
	sta->setCommunicationProfile(commProfile);

	vector<RATID>* testRATs = sta->getRATs();
	vector<RATID>::iterator it_testRATs = testRATs->begin();
	pair<map<stationID_t, Station*>::iterator, bool> check;
	check = stations.insert(pair<stationID_t, FixedStation*> (ID, sta));
	if (!check.second) {
		cerr << "[facilities] - Fixed Station not inserted in the facilities." << endl;
		return false;
	}

#ifdef _DEBUG_STATIONS
	cout << "[facilities] - FixedStation: ID = " << sta->getID() << " --> pos: " << sta->getPosition() << endl;
#endif
}
#ifdef _DEBUG_STATIONS
for (map<stationID_t, Station*>::iterator it = stations.begin(); it != stations.end(); it++)
	cout << "[facilities] - check all stations - FixedStation: ID = " << it->first << " --> pos: " << it->second->getPosition() << endl;
cout << "------------------------------------------------------" << endl << endl;
#endif

fixedStationStrVec.clear();

configFlag = true;
return configFlag;
}


const Station* StationFacilities::getStation(stationID_t stationID) const {
	map<stationID_t, Station*>::const_iterator it = stations.find(stationID);
	if (it == stations.end()) {
		cerr << "[facilities] Station " << stationID << " not found." << endl;
		return 0;
	} else
		return it->second;
}

const MobileStation* StationFacilities::getMobileStation(stationID_t mobileStationID) const {
	map<stationID_t, Station*>::const_iterator it = stations.find(mobileStationID);
	if (it == stations.end()) {
		cerr << "[facilities] Station " << mobileStationID << " not found." << endl;
		return 0;
	}
	const Station *sta = it->second;
	const MobileStation *curr = dynamic_cast<const MobileStation*>(sta);
	return curr;
}

const FixedStation* StationFacilities::getFixedStation(stationID_t fixedStationID) const {
	map<stationID_t, Station*>::const_iterator it = stations.find(fixedStationID);
	if (it == stations.end()) {
		cerr << "[facilities] Station " << fixedStationID << " not found." << endl;
		return 0;
	}
	const Station *sta = it->second;
	const FixedStation *curr = dynamic_cast<const FixedStation*>(sta);
	return curr;
}

const map<stationID_t, Station*>& StationFacilities::getAllStations() const {
	return stations;
}

map<stationID_t, const MobileStation*>* StationFacilities::getAllMobileStations() {
	map<stationID_t, const MobileStation*> *mapMobile = new map<stationID_t, const MobileStation*>();

	map<stationID_t, Station*>::iterator it;
	for (it = stations.begin(); it != stations.end(); it++) {
		Station *sta = it->second;
		MobileStation *curr = dynamic_cast<MobileStation*>(sta);
		if (curr != NULL)
			mapMobile->insert(pair<stationID_t, const MobileStation*>(curr->getID(), curr));
	}
	return mapMobile;
}

map<stationID_t, const FixedStation*>* StationFacilities::getAllFixedStations() {
	map<stationID_t, const FixedStation*> *mapFixed = new map<stationID_t, const FixedStation*>();

	map<stationID_t, Station*>::iterator it;
	for (it = stations.begin(); it != stations.end(); it++) {
		Station *sta = it->second;
		FixedStation *curr = static_cast<FixedStation*>(sta); // originally dynamic cast but returns segfault
		if (curr != NULL)
			mapFixed->insert(pair<stationID_t, const FixedStation*>(curr->getID(), curr));
	}
	return mapFixed;
}

bool StationFacilities::updateMobileStationDynamicInformation(stationID_t stationId, TMobileStationDynamicInfo info) {
	map<stationID_t, Station*>::iterator it = stations.find(stationId);
	MobileStation *curr = NULL;

	// Update the Mobile Station, if it exists
	if (it != stations.end()) {
		// check that the station is a MobileStation
		Station *sta = it->second;
		curr = dynamic_cast<MobileStation*>(sta);

		if (curr == NULL) {
			cerr << "[facilities] ERROR: It is not possible to update the information of the "
					"fixed station " << stationId << " by using the method StationFacilities::updateMobileStationDynamicInformation()." << endl;
			return false;
		}

	}
	// Create the mobile station, if it does not exist
	else {
		// create new mobile station
		curr = new MobileStation(stationId);

		// set the RATs (all of them will be available)
		map<RATID, float>::iterator RATit;
		vector< pair<RATID, bool> > RATs;
		for (RATit = defaultPenetrationRates.begin(); RATit != defaultPenetrationRates.end(); RATit++) {
			float randomValue = staFacRand.rand((float) 100.0);
			if (RATit->second > randomValue) {
				RATs.push_back(pair<RATID, bool>(RATit->first, true));
			}
		}
		curr->setRATs(RATs);

		// insert the new mobile station in the map
		stations.insert(pair<stationID_t, Station*>(stationId, curr));
	}

	// Update the dynamic information
	if (curr != NULL) {
		curr->setSpeed(info.speed);
		curr->setAcceleration(info.acceleration);
		curr->setDirection(info.direction);
		curr->setExteriorLights(info.exteriorLights);
		curr->setPosition(Point2D(info.positionX, info.positionY));
		curr->setVehicleLegth(info.length);
		curr->setVehicleWidth(info.width);
		curr->setVehicleHeight(info.height);
		curr->setLaneID(info.lane);
		curr->isActive = true;
		// Update the position history of the node, if it is required
		if (recordMobilityHistory){
			updateMobilityHistory(curr->getID(), info.timeStep, Point2D(info.positionX, info.positionY));
#ifdef LOG_ON
			std::cout<<"updateMobilityHistory::info.positionX"<<info.positionX<<":Y:"<<info.positionY<<std::endl;
#endif

		}
	}
	return true;
}

map<stationID_t, const Station*>* StationFacilities::getStationsInArea(GeometricShape &area) {
	map<stationID_t, const Station*> *mapStations = new map<stationID_t, const Station*>();

	map<stationID_t, Station*>::iterator it;
	for (it = stations.begin(); it != stations.end(); it++) {
		Station *sta = it->second;
		if (area.isInternal(sta->getPosition()))
			mapStations->insert(pair<stationID_t, const Station*>(sta->getID(), sta));
	}
	return mapStations;
}

map<stationID_t, const Station*>* StationFacilities::getStationsInArea(vector<RoadElement*> &area) {
	map<stationID_t, const Station*> *mapStations = new map<stationID_t, const Station*>();
	for (unsigned int i=0; i<area.size(); i++) {
		switch (area[i]->getRoadElementType()) {
		case LANE: {
			Lane *curr = dynamic_cast<Lane*>(area[i]);
			getStationsOnLane(curr, mapStations);
			break;
		}
		case EDGE: {
			Edge *curr = dynamic_cast<Edge*>(area[i]);
			vector<roadElementID_t> vlanes = curr->getLaneIDs();
			for (unsigned j=0; j<vlanes.size(); j++) {
				Lane *currLane = (Lane*) mapFac->getLane(vlanes[j]);
				if (currLane != 0)
					getStationsOnLane(currLane, mapStations);
			}
			break;
		}
		case JUNCTION: {
			Junction *curr = dynamic_cast<Junction*>(area[i]);
			vector<roadElementID_t> vlanes = curr->getInternalLaneIDs();
			for (unsigned j=0; j<vlanes.size(); j++) {
				Lane *currLane = (Lane*) mapFac->getLane(vlanes[j]);
				if (currLane != 0)
					getStationsOnLane(currLane, mapStations);
			}
			break;
		}
		default: {
			cerr << "[facilities] ERROR: The area is not defined properly. Only lanes, edges and junctions can describe an area." << endl;
			abort();
		}
		}
	}
	return mapStations;
}

map<stationID_t, const MobileStation*>* StationFacilities::getMobileStationsInArea(GeometricShape &area) {
	map<stationID_t, const MobileStation*> *mapMobileStations = new map<stationID_t, const MobileStation*>();

	map<stationID_t, Station*>::iterator it;
	for (it = stations.begin(); it != stations.end(); it++) {
		Station *sta = it->second;

		MobileStation *curr = static_cast<MobileStation*>(sta);
		if (curr == NULL) // Check if there is a problem with the casting process
			cerr << "[facilites] WARNING: Casting is not correct." << endl;
		else
		{

			if ( !area.isInternal(curr->getPosition()) ) { // Check if the position is whithin the shape

			}
			else
			{
				// Add to the collection
				mapMobileStations->insert(pair<stationID_t, const MobileStation*>(curr->getID(), curr));
			}
		}

	}
	return mapMobileStations;
}

map<stationID_t, const MobileStation*>* StationFacilities::getMobileStationsInArea(vector<RoadElement*> &area) {
	map<stationID_t, const MobileStation*> *mapMobileStations = new map<stationID_t, const MobileStation*>();

	for (unsigned int i=0; i<area.size(); i++) {
		switch (area[i]->getRoadElementType()) {
		case LANE: {
			Lane *curr = dynamic_cast<Lane*>(area[i]);
			getMobileStationsOnLane(curr, mapMobileStations);
			break;
		}
		case EDGE: {
			Edge *curr = dynamic_cast<Edge*>(area[i]);
			vector<roadElementID_t> vlanes = curr->getLaneIDs();
			for (unsigned j=0; j<vlanes.size(); j++) {
				Lane *currLane = (Lane*) mapFac->getLane(vlanes[j]);
				if (currLane != 0)
					getMobileStationsOnLane(currLane, mapMobileStations);
			}
			break;
		}
		case JUNCTION: {
			Junction *curr = dynamic_cast<Junction*>(area[i]);
			vector<roadElementID_t> vlanes = curr->getInternalLaneIDs();
			for (unsigned j=0; j<vlanes.size(); j++) {
				Lane *currLane = (Lane*) mapFac->getLane(vlanes[j]);
				if (currLane != 0)
					getMobileStationsOnLane(currLane, mapMobileStations);
			}
			break;
		}
		default: {
			cerr << "[facilities] ERROR: The area is not defined properly." << endl;
			abort();
		}
		}
	}
	return mapMobileStations;
}

map<stationID_t, const FixedStation*>* StationFacilities::getFixedStationsInArea(GeometricShape &area) {
	map<stationID_t, const FixedStation*> *mapFixedStations = new map<stationID_t, const FixedStation*>();

	map<stationID_t, Station*>::iterator it;
	for (it = stations.begin(); it != stations.end(); it++) {
		Station *sta = it->second;
		FixedStation *curr = dynamic_cast<FixedStation*>(sta);
		if ((curr != NULL) && (area.isInternal(curr->getPosition())))
			mapFixedStations->insert(pair<stationID_t, FixedStation*>(curr->getID(), curr));
	}
	return mapFixedStations;
}

map<stationID_t, const FixedStation*>* StationFacilities::getFixedStationsInArea(vector<RoadElement*> &area) {
	map<stationID_t, const FixedStation*> *mapFixedStations = new map<stationID_t, const FixedStation*>();

	for (unsigned int i=0; i<area.size(); i++) {
		switch (area[i]->getRoadElementType()) {
		case LANE: {
			Lane *curr = dynamic_cast<Lane*>(area[i]);
			getFixedStationsOnLane(curr, mapFixedStations);
			break;
		}
		case EDGE: {
			Edge *curr = dynamic_cast<Edge*>(area[i]);
			vector<roadElementID_t> vlanes = curr->getLaneIDs();
			for (unsigned j=0; j<vlanes.size(); j++) {
				Lane *currLane = (Lane*) mapFac->getLane(vlanes[j]);
				if (currLane != 0)
					getFixedStationsOnLane(currLane, mapFixedStations);
			}
			break;
		}
		case JUNCTION: {
			Junction *curr = dynamic_cast<Junction*>(area[i]);
			vector<roadElementID_t> vlanes = curr->getInternalLaneIDs();
			for (unsigned j=0; j<vlanes.size(); j++) {
				Lane *currLane = (Lane*) mapFac->getLane(vlanes[j]);
				if (currLane != 0)
					getFixedStationsOnLane(currLane, mapFixedStations);
			}
			break;
		}
		default: {
			cerr << "[facilities] ERROR: The area is not defined properly." << endl;
			abort();
		}
		}
	}
	return mapFixedStations;
}

void StationFacilities::getStationsOnLane(const Lane* lane, map<stationID_t, const Station*>* stationsOnLane) {
	map<stationID_t, Station*>::iterator it;
	for (it = stations.begin(); it != stations.end(); it++) {
		Lane* tmp = mapFac->convertPoint2Map((Point2D&) it->second->getPosition());
		if (tmp->getID() == lane->getID())
			stationsOnLane->insert(pair<stationID_t, const Station*>(it->second->getID(), it->second));
	}
}

void StationFacilities::getMobileStationsOnLane(const Lane* lane, map<stationID_t, const MobileStation*>* mobileStationsOnLane) {
	map<stationID_t, Station*>::iterator it;
	for (it = stations.begin(); it != stations.end(); it++) {
		MobileStation *sta = dynamic_cast<MobileStation*>(it->second);
		if (sta) {
			Lane* tmp = mapFac->convertPoint2Map((Point2D&) it->second->getPosition());
			if (tmp->getID() == lane->getID())
				mobileStationsOnLane->insert(pair<stationID_t, MobileStation*>(sta->getID(), sta));
		}
	}
}

void StationFacilities::getFixedStationsOnLane(const Lane* lane, map<stationID_t, const FixedStation*>* fixedStationsOnLane) {
	map<stationID_t, Station*>::iterator it;
	for (it = stations.begin(); it != stations.end(); it++) {
		FixedStation *sta = dynamic_cast<FixedStation*>(it->second);
		if (sta) {
			Lane* tmp = mapFac->convertPoint2Map((Point2D&) it->second->getPosition());
			if (tmp->getID() == lane->getID())
				fixedStationsOnLane->insert(pair<stationID_t, FixedStation*>(sta->getID(), sta));
		}
	}
}

bool StationFacilities::isStationInArea(stationID_t stationID, GeometricShape &area) {
	const Station *sta = getStation(stationID);
	if (sta == NULL) return false;

	return area.isInternal(sta->getPosition());
}

bool StationFacilities::isStationInArea(stationID_t stationID, RoadElement &area) {
	const Station *sta = getStation(stationID);
	if (sta == NULL) return false;

	const Lane *stationLane = mapFac->convertPoint2Map((Point2D&) sta->getPosition());

	switch (area.getRoadElementType()) {
	case LANE: {
		Lane *curr = dynamic_cast<Lane*>(&area);
		if (curr == stationLane)
			return true;
		break;
	}
	case EDGE: {
		Edge *curr = dynamic_cast<Edge*>(&area);
		vector<roadElementID_t> vlanes = curr->getLaneIDs();
		for (unsigned j=0; j<vlanes.size(); j++) {
			Lane *currLane = (Lane*) mapFac->getLane(vlanes[j]);
			if (currLane == stationLane)
				return true;
		}
		break;
	}
	case JUNCTION: {
		Junction *curr = dynamic_cast<Junction*>(&area);
		vector<roadElementID_t> vlanes = curr->getInternalLaneIDs();
		for (unsigned j=0; j<vlanes.size(); j++) {
			Lane *currLane = (Lane*) mapFac->getLane(vlanes[j]);
			if (currLane == stationLane)
				return true;
		}
		break;
	}
	default: {
		cerr << "[facilities] ERROR: The area is not defined properly. Only lanes, edges and junctions can describe an area." << endl;
		abort();
	}
	}
	return false;
}

bool StationFacilities::isStationInArea(stationID_t stationID, vector<Area2D*> &area) {
	const Station *sta = getStation(stationID);
	if (sta == NULL) return false;

	for (unsigned int i = 0; i < area.size(); i++) {
		switch (area[i]->getArea2DType()) {
		case(ROADELEMENT): {
			RoadElement* element = dynamic_cast<RoadElement*>(area[i]);
			if (isStationInArea(stationID, *element))
				return true;
		}
		case(GEOMETRICSHAPE): {
			GeometricShape* element = dynamic_cast<GeometricShape*>(area[i]);
			if (isStationInArea(stationID, *element))
				return true;
		}
		default: {
			return false;
		}
		}
	}
	return false;
}

bool StationFacilities::enableRAT(stationID_t stationID, RATID RATtoBeEnabled) {
	Station *sta = (Station*) getStation(stationID);
	return sta->enableRAT(RATtoBeEnabled);
}

bool StationFacilities::disableRAT(stationID_t stationID, RATID RATtoBeDisabled) {
	Station *sta = (Station*) getStation(stationID);
	return sta->disableRAT(RATtoBeDisabled);
}

bool StationFacilities::enableRATAllStations(RATID RATtoBeEnabled) {
	bool flag = true;
	for (map<stationID_t, Station*>::iterator it = stations.begin(); it != stations.end(); it++)
		flag &= it->second->enableRAT(RATtoBeEnabled);
	return flag;
}

bool StationFacilities::disableRATAllStations(RATID RATtoBeDisabled) {
	bool flag = true;
	for (map<stationID_t, Station*>::iterator it = stations.begin(); it != stations.end(); it++)
		flag &= it->second->disableRAT(RATtoBeDisabled);
	return flag;
}

const map<RATID, float>& StationFacilities::getDefaultPenetrationRates() const {
	return defaultPenetrationRates;
}

const map<RATID, string>& StationFacilities::getDefaultCommunicationProfiles() const {
	return defaultCommunicationProfiles;
}

bool StationFacilities::isStationOfType(stationID_t stationID, icsstationtype_t type) {
	map<stationID_t, Station*>::iterator it = stations.find(stationID);
	if ((it != stations.end()) && (it->second->getType() == type))
		return true;
	return false;
}

void StationFacilities::updateMobilityHistory(stationID_t vehicleId, icstime_t time, Point2D pos) {

	map<icstime_t, vector<pair<stationID_t, Point2D> > >::iterator it;
	it = mobilityHistory.find(time);
	if (it != mobilityHistory.end()) {
		it->second.push_back(pair<stationID_t, Point2D>(vehicleId, pos));
#ifdef LOG_ON
std::cout<<"[StationFacilities][updateMobilityHistory] UPDATE Time "<<time<<", pos x "<<pos.x()<<", y "<<pos.y()<<std::endl;
#endif
	}
	else {
		vector<pair<stationID_t, Point2D> > posHistory;
		posHistory.push_back(pair<stationID_t, Point2D>(vehicleId, pos));
		mobilityHistory.insert(pair<icstime_t, vector<pair<stationID_t, Point2D> > >(time, posHistory));
#ifdef LOG_ON
		std::cout<<"[StationFacilities][updateMobilityHistory] INSERT Time "<<time<<", pos x "<<pos.x()<<", y "<<pos.y()<<std::endl;
#endif
	}

#ifdef _DEBUG_STATIONS
	map<stationID_t, Point2D > posMobility = getPositionFromMobilityHistory();
	std::map<stationID_t, Point2D >::iterator itPos = posMobility.find(vehicleId);
	if (itPos != posMobility.end()){
		std::cout<<"[StationFacilities][updateMobileStationDynamicInformation] node id "<<vehicleId<< ", x "<<itPos->second.x()<< ", y "<<itPos->second.y() <<std::endl;
	}
#endif

	return;
}

void StationFacilities::createMobilityHistoryFile() {

	cout << "[facilities] Write the mobility history file." << endl;

	ofstream outfile;
	outfile.open(mobilityHistoryFilename.c_str());
	if (outfile.bad()) {
		cerr << "[facilities] Impossible to write the mobility history file." << endl;
		return;
	}
	map<icstime_t, vector<pair<stationID_t, Point2D> > >::iterator it;
	for (it = mobilityHistory.begin(); it != mobilityHistory.end(); it++) {
		icstime_t currTimeStep = it->first;
		for (unsigned int i = 0; i < it->second.size(); i++) {
			// line-format: NODEID TIME (X, Y)
			outfile << it->second[i].first << "\t" << currTimeStep << "\t" <<
					"(" << it->second[i].second.x() << ", " <<  it->second[i].second.y() << ")" << endl;
		}
	}
	outfile.close();
}


Point2D StationFacilities::getNodePositionFromMobilityHistory( icstime_t time, stationID_t stationId)
{
	map<icstime_t, vector<pair<stationID_t, Point2D> > >::reverse_iterator rit;
	Point2D  pos(-101.0,-101.0);

	int i=0;
	if (!mobilityHistory.empty()){
		for (rit=mobilityHistory.rbegin(); rit!=mobilityHistory.rend(); ++rit){
			i=i+1;
			if(time == rit->first ) {
				for (int unsigned it=(rit->second.size()-1); it>0 ; --it) {
					if (stationId == rit->second[it].first){
#ifdef LOG_ON
						std::cout<<"[StationFacilities][getPositionFromMobilityHistory] timestep "<<rit->first <<", id "<< rit->second[it].first<<", pos x "<<rit->second[it].second.x()<<std::endl;
#endif
						return rit->second[it].second;
					}
				}

				if (stationId == rit->second[0].first){
#ifdef LOG_ON
					std::cout<<"[StationFacilities][getPositionFromMobilityHistory] item 0 timestep "<<rit->first <<", id "<< rit->second[0].first<<", pos x"<<rit->second[0].second.x()<<std::endl;
#endif
					return  rit->second[0].second;
				}
			}

		}
	}
	return pos;
}



}
