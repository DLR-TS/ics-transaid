/****************************************************************************/
/// @file    LDMLogic.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 17, 2010
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

#ifndef LDMLOGIC_H_
#define LDMLOGIC_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "../../../utils/ics/iCStypes.h"
#include "../../../utils/ics/geometric/Shapes.h"
using namespace ics_types;

#include "FacilityMessages.h"
#include "../mapFacilities/MapFacilities.h"
#include "../stationFacilities/StationFacilities.h"

#include <map>
#include <vector>
using namespace std;

namespace ics_facilities
{

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LDMLogic
 * @brief Contains the data structure for handling the received messages and the LDM storage rules. Moreover, it can provide the useful methods to filter the reception table.
 */
class LDMLogic
{
public:

  /**
   * @brief Costructor.
   * param[in] mapFac Pointer to the mapFacilities.
   * param[in] staFac Pointer to the stationFacilities.
   */
  LDMLogic(MapFacilities* mapFac, StationFacilities* staFac);

  /**
   * @brief Destructor.
   */
  virtual ~LDMLogic();

  //**** Relevance Rules configuration method ****
  bool configureRelevanceRules(string relevFilename);

  //**** Payload Creation ****
  actionID_t createCAMpayload(stationID_t stationID);
  actionID_t createDENMpayload(stationID_t stationID, icstime_t expiryTime, icstime_t frequency,
      denmReliability_t reliability, bool negation, denmSituationCause_t causeCode, denmSituationSeverity_t severity,
      latitude_t latitude, longitude_t longitude, vector<Area2D*> relevanceArea);

  actionID_t createApplicationMessagePayload(stationID_t stationID, messageType_t comType,
      TApplicationMessageDestination destination, unsigned char preferredTechnologies, unsigned short messageLength,
      unsigned char communicationProfile, int applicationId, unsigned char applicationMessageType,
      int messageSequenceNumber, string &extra);

  //**** Message Storage ****
  bool storeMessage(actionID_t actionID, vector<stationID_t> receivers);

  //**** Message related get methods ****
  ReceivedMessage* getReceivedMessage(actionID_t actionID) const;
  FacilityMessagePayload* getReceivedMessagePayload(actionID_t actionID) const;
  map<actionID_t, ReceivedMessage*>* getLDM(stationID_t stationID);
  map<actionID_t, ReceivedMessage*>* getLastMessages(icstime_t startTime);
  map<actionID_t, ReceivedMessage*>* getAllSpecificMessages(messageType_t type);
  map<actionID_t, ReceivedMessage*>* getMessagesFromSender(stationID_t stationID);

  vector<CAMPayloadGeneral*>* getGeneratedCAMsFromStationInTable(stationID_t stationID);
  CAMPayloadGeneral* getLastGeneratedCAM(stationID_t stationID);
  vector<CAMPayloadGeneral*>* getLastCAMsReceivedByStation(stationID_t stationID);

  map<stationID_t, Point2D>* getClassifiedStationsPositionsAroundStations(stationID_t receiverID,
      icsstationtype_t type);
  map<stationID_t, Point2D>* getClassifiedStationsPositionsAroundStationsInTemporalWindow(stationID_t receiverID,
      icsstationtype_t type, icstime_t start, icstime_t stop);

  //**** Relevant Check settings (get and set methods) ****
  const vector<Area2D*>& getRelevantArea();
  map<icsstationtype_t, bool>* getRelevantStationTypes() const;
  map<messageType_t, bool>* getRelevantMessageTypes() const;
  direction_t getRelevantDirection() const;
  direction_t getRelevantDirectionAccuracy() const;
  icstime_t getDefaultMessageLifeInterval() const;
  void setRelevantStationTypes(map<icsstationtype_t, bool> &relevStaTypes);
  void setRelevantDirection(direction_t relevDirection, direction_t relevDirectionAccuracy);
  void setDefaultMessageLifeInterval(icstime_t defMesLifeInterval);
  void setRelevantArea(vector<Area2D*>& areas);
  void addRelevantArea(Area2D *relevArea);
  void deleteRelevantArea(Area2D *relevArea);
  void clearRelevantArea();

  //**** Miscellaneous ****
  void updateClock(icstime_t newSimTime);
  bool hasStationEverSentMessages(stationID_t stationID);
  seqNo_t getLastSeqNoForStation(stationID_t stationID);
  icstime_t getReceivedMessageDeletionTimeFromTable(actionID_t actionID);

private:

  //***********************
  //**** Record Tables ****
  //***********************

  /// @brief iTETRIS FACILITIES MESSAGE TABLE: Structure that stores the messages and their receivers
  map<actionID_t, ReceivedMessage*> iFMT;

  /// @brief iTETRIS FACILITIES PAYLOAD TABLE: Structure that stores the created payloads (as ReceivedMessage but with an empty receivers list) to be sent
  map<actionID_t, ReceivedMessage*> iFPT;

  /// @brief Structure that keeps track of the message sequence number of each station
  map<stationID_t, seqNo_t> stationsSeqNoRecord;

  //***********************
  //**** iFMT Cleanup structures ****
  //***********************

  /// @brief Dictionary that for each actionID reports the deletion time.
  map<actionID_t, icstime_t> eventList;

  /// @brief Dictionary that for each time step reports the vector of messages (actionID) to be deleted.
  map<icstime_t, vector<actionID_t> > deletionList;

  //***********************
  //**** Relevance variables ****
  //***********************

  /// @brief Vector of areas considered for the storage of the received messages. These areas can be used to decrease the number of messages stored in the iFMT.
  vector<Area2D*> relevantArea;

  /// @brief This variable allows to store in the iFMT the reception of a message only for specified types of stations.
  map<icsstationtype_t, bool> relevantStationTypes;

  /// @brief This variable allows to store in the iFMT only messages of a certain type. The others will not be saved.
  map<messageType_t, bool> relevantMessages;

  /// @brief This variable allows to store in the iFMT only the messages received by mobile nodes that are moving in a certain direction.
  direction_t relevantDirection;

  /// @brief Accuracy of the 'relevantDirection' variable
  direction_t relevantDirectionAccuracy;

  /// @brief True if the 'relevantDirection' is defined. False otherwise.
  bool isRelevantDirectionDefined;

  //***********************
  //**** Pointers to the map and the stations ****
  //***********************

  /// @brief Pointer to the mapFacilities.
  MapFacilities* mapFac;

  /// @brief Pointer to the stationsFacilities.
  StationFacilities* staFac;

  //***********************
  //**** Time related variables ****
  //***********************

  /// @brief Variable that stores the value of the current time step.
  icstime_t simTime;

  /// @brief Variable that defines which is the default life interval of the messages inside the iFMT before being deleted.
  icstime_t defaultMessageLifeInterval;

  //***********************
  //**** Table Cleanup Methods ****
  //***********************

  /**
   * @brief Schedule the removal of a message from the iFMT.
   * @param[in] actionID Action ID of the message.
   * @param[in] deletionTime Time step for the deletion.
   */
  void scheduleMessageRemoval(actionID_t actionID, icstime_t deletionTime);

  /**
   * @brief Delete all the message that were scheduled to be removed from the iFMT.
   * @param[in] deletionTime Time step for the deletion.
   */
  int launchIFMTCleanupManager(icstime_t deletionTime);

  /**
   * @brief Delete all the message that were scheduled to be removed from the iFMT.
   * @param[in] deletionTime Time step for the deletion.
   */
  int iFPTCleanup(icstime_t deletionTime);

  /**
   * @brief Delete a message from the iFMT.
   * @param[in] actionID Action ID of the message.
   */
  bool deleteMessage(actionID_t actionID);

  //***********************
  //**** Table Maintenance Methods ****
  //***********************

  /**
   * @brief Create the action ID starting from the station ID and the message sequence number.
   * @param[in] station ID of the station sending the message.
   * @param[in] seqNo Message sequence number.
   * @return .
   */
  actionID_t createActionID(stationID_t stationId, seqNo_t seqNo);

  /**
   * @brief Read the sender ID from the action ID of a message.
   * @param[in] actionID Message action ID.
   * @return ID of the station.
   */
  stationID_t getStationIDfromActionID(actionID_t actionID);

  /**
   * @brief Read the message sequence number from the action ID of a message.
   * @param[in] actionID Message action ID.
   * @return Message sequence number.
   */
  seqNo_t getSequenceNumberfromActionID(actionID_t actionID);

  /**
   * @brief Generate a new sequence number for a given station. Every station has its own counter.
   * @param[in] stationID ID of the station.
   * @return Generated sequence number.
   */
  seqNo_t getNewSeqNo(stationID_t stationID);

  /**
   * @brief Store the payload of the message in the iFPT.
   * @param[in] actionID Action ID that identifies the message.
   * @param[in] payload Pointer to the message payload.
   * @param[in] message Type of message (e.g., CAM, DENM, ...).
   * @return True if the storage was completed, False otherwise.
   */
  bool storePayload(actionID_t actionID, FacilityMessagePayload* payload, messageType_t message);

  //***********************
  //**** Relevance Check Rules Methods ****
  //***********************

  /**
   * @brief Given a list of receivers, it returns the list of those that are inside the relevance area.
   * @param[in] receivers Vector of receivers (their IDs).
   * @return Pointer to the vector containing the relevant receivers.
   */
  vector<stationID_t>* getReceiversInRelevantArea(vector<stationID_t>& receivers);

  /**
   * @brief Given a list of receivers, it returns the list of those that are of the types defined in the 'relevantStationTypes' variable.
   * @param[in] receivers Vector of receivers (their IDs).
   * @return Pointer to the vector containing the relevant receivers.
   */
  vector<stationID_t>* getReceiversRelevantStations(vector<stationID_t>& receivers);

  /**
   * @brief Given a list of receivers, it returns the list of those that are either fixed stations or moving toward a relevant direction.
   * @param[in] receivers Vector of receivers (their IDs).
   * @return Pointer to the vector containing the relevant receivers.
   */
  vector<stationID_t>* getReceiversOnRelevantDirection(vector<stationID_t>& receivers);

  /**
   * @brief Given a list of receivers, it returns the list of those that are inside the geocast area of the received message.
   * @param[in] receivers Vector of receivers (their IDs).
   * @param[in] msg Geocast message.
   * @return Pointer to the vector containing the relevant receivers.
   */
  vector<stationID_t>* getReceiversInGeocastArea(vector<stationID_t> receivers, const FacilityMessagePayload* msg);

  /**
   * @brief Given a message, it returns if the message is relevant (according to the relevance rules), or not.
   * @param[in] message Reference to the message to be checked.
   * @return True if the message is relevant, False otherwise.
   */
  bool isMessageRelevant(const ReceivedMessage& message);

  /**
   * @brief Returns all the messages still stored in the iFMT that have been received by a given station.
   * @param[in] receiverID Station ID.
   * @return Vector of pointers to the received messages.
   */
  vector<ReceivedMessage*> getReceivedMessagesFromStation(stationID_t receiverID);

  /**
   * @brief Returns all the messages of a certain type still stored in the iFMT that have been received by a given station.
   * @param[in] receiverID Station ID.
   * @param[in] type Message type (CAM, DENM, etc...).
   * @return Vector of pointers to the received messages.
   */
  vector<ReceivedMessage*> getReceivedTypeMessagesFromStation(stationID_t receiverID, messageType_t type);

  /**
   * @brief Returns the reception time of the message by a certain receiver.
   * @param[in] receiverID Station ID.
   * @param[in] message Pointer to the received message.
   * @return Time step of reception.
   */
  icstime_t getMessageReceptionTimeByReceiver(stationID_t receiverID, ReceivedMessage* message);

};

}

#endif /* LDMLOGIC_H_ */
