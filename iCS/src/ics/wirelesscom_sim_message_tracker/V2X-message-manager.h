/****************************************************************************/
/// @file    V2X-message-manager.h
/// @author  Julen Maneros
/// @date
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

/**************************************************************************************
 * Edited by Jérôme Härri <haerri@eurecom.fr>
 * EURECOM 2015
 *
 * Added a generic tap container to allow sending and returning
 * generic Tx/Rx specific data from iCS an ns-3
 *
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
 *************************************************************************************/

#ifndef V2X_MESSAGE_MANAGER_H
#define V2X_MESSAGE_MANAGER_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "../../utils/ics/iCStypes.h"
#include <vector>

using namespace ics_types;

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class V2xMessage;
class V2xMessageInformation;
class V2xCamArea;
class V2xGeobroadcastArea;

// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @struct ScheduledCamMessageData
 * @brief Struct to store the information related to the CAM scheduled messages.
 * @todo To be commented.
*/
struct ScheduledCamMessageData {
    stationID_t senderNs3ID;
    stationID_t senderIcsID;
    messageType_t messageType;
    icstime_t timeStep;
    seqNo_t sequenceNumber;
    actionID_t actionID;
    bool received;
};

/**
 * @struct ScheduledUnicastMessageData
 * @brief Struct to store the information related to the UNICAST scheduled messages.
 * @todo To be commented.
*/
struct ScheduledUnicastMessageData {
    stationID_t senderNs3ID;
    stationID_t senderIcsID;
    stationID_t receiverNs3ID;
    stationID_t receiverIcsID;
    messageType_t messageType;
    icstime_t timeStep;
    seqNo_t sequenceNumber;
    actionID_t actionID;
    int appMessageId;
    // enhanced fields, to bring ns-3 specific RX data to iCS and the application
    std::vector<unsigned char>* packetTagContainer;
    int messageId;
};

/**
 * @struct ScheduledAPPMessageData
 * @brief Struct to store the information related to the Application message scheduled messages.
 * @todo To be commented.
*/
struct ScheduledAPPMessageData {
    stationID_t senderNs3ID;
    stationID_t senderIcsID;
    stationID_t receiverNs3ID;
    stationID_t receiverIcsID;
    messageType_t messageType;
    icstime_t timeStep;
    seqNo_t sequenceNumber;
    actionID_t actionID;
    int appMessageId;
    // enhanced fields, to bring ns-3 specific RX data to iCS and the application
    std::vector<unsigned char>* packetTagContainer;
};

/**
 * @struct ScheduledGeobroadcastMessageData
 * @brief Struct to store the information related to the GEOBROADCAST scheduled messages.
 * @todo To be commented.
*/
struct ScheduledGeobroadcastMessageData {
    stationID_t senderNs3ID;
    stationID_t senderIcsID;
    stationID_t receiverIcsID;
    messageType_t messageType;
    icstime_t timeStep;
    seqNo_t sequenceNumber;
    int appMessageId;
    actionID_t actionID;
    std::string subscriptionType;
    bool received;
    // enhanced fields, to bring ns-3 specific RX data to iCS and the application
    std::vector<unsigned char>* packetTagContainer;
    int messageId;
};

/**
 * @struct IdentifiersStorageStruct
 * @brief Struct to store the identifiers related to a certain message.
 * @todo To be commented.
*/
struct IdentifiersStorageStruct {
    actionID_t actionID;
    stationID_t senderID;
    stationID_t receiverID;
    bool stored;
};

/**
 * @struct ScheduledTopobroadcastMessageData
 * @brief Struct to store the information related to the TOPOBROADCAST scheduled messages.
 * @todo To be commented.
*/
struct ScheduledTopobroadcastMessageData {
    stationID_t senderNs3ID;
    stationID_t senderIcsID;
    messageType_t messageType;
    icstime_t timeStep;
    seqNo_t sequenceNumber;
    actionID_t actionID;
    int appMessageId;
    // enhanced fields, to bring ns-3 specific RX data to iCS and the application
    std::vector<unsigned char>* packetTagContainer;
};

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class V2xMessageManager
 * @brief Keeps track of the messages and the information attached
 * scheduled in the Wireless Communications Simulator.
 */
class V2xMessageManager
{
public:
    /// @brief Constructor.
    V2xMessageManager();

    /// @brief Destructor.
    ~V2xMessageManager();

    /**
     * @brief Creates a CAM area
     * @param[in] subscriptionId Identifier of the subscription that requested CAM area.
     * @param[in] frequency Frequency of the transmissions in the area.
     * @param[in] payloadLenght Lenght of the CAM messages payload.
     * @return True: If the operation takes place successfully
     * @return False: If an error occurs
     */
    bool CreateV2xCamArea(int subscriptionId, float frequency, unsigned int payloadLenght);

    /**
     * @brief Creates a Geobroadcast area
     * @param[in] subscriptionId Identifier of the subscription that requested Geobroadcast area.
     * @param[in] frequency Frequency of the transmissions in the area.
     * @param[in] payloadLenght Lenght of the Geobroadcast messages payload.
     * @return True: If the operation takes place successfully
     * @return False: If an error occurs
     */
    bool CreateV2xGeobroadcastArea(int subscriptionId, float frequency, unsigned int payloadLenght);

    /**
     * @brief Inserts a new row in the table that stores the scheduled CAM messages
     * @param[in] &table Table that contains the scheduled  CAM messages
     * @param[in] data New information to store in the table
     */
    void InsertCamRow(std::vector<ScheduledCamMessageData> &table, ScheduledCamMessageData data);

    /**
     * @brief Inserts a new row in the table that stores the scheduled UNICAST messages
     * @param[in] &table Table that contains the scheduled UNICAST messages
     * @param[in] data New information to store in the table
     */
    void InsertUnicastRow(std::vector<ScheduledUnicastMessageData> &table, ScheduledUnicastMessageData data);

    /**
     * @brief Inserts a new row in the table that stores the scheduled GEOBROADCAST messages
     * @param[in] &table Table that contains the scheduled  GEOBROADCAST messages
     * @param[in] data New information to store in the table
     */
    void InsertGeobroadcastRow(std::vector<ScheduledGeobroadcastMessageData> &table, ScheduledGeobroadcastMessageData data);

    /**
     * @brief Inserts a new row in the table that stores the scheduled TOPOBROADCAST messages
     * @param[in] &table Table that contains the scheduled  TOPOBROADCAST messages
     * @param[in] data New information to store in the table
     */
    void InsertTopobroadcastRow(std::vector<ScheduledTopobroadcastMessageData> &table, ScheduledTopobroadcastMessageData data);

    /**
      * @brief Compares two structures that contain information about two scheduled CAM messages
      * @param[in] rowReceived Information obtained from ns-3
      * @param[in] rowTable One of the rows that the table contains
      * @return True: If there is a match
      * @return False: If there isn't any coincidence
      */
    bool CompareCamRows(ScheduledCamMessageData rowReceived, ScheduledCamMessageData rowTable);

    /**
      * @brief Compares two structures that contain information about two scheduled UNICAST messages
      * @param[in] rowReceived Information obtained from ns-3
      * @param[in] rowTable One of the rows that the table contains
      * @return True: If there is a match
      * @return False: If there isn't any coincidence
      */
    bool CompareUnicastRows(ScheduledUnicastMessageData rowReceived, ScheduledUnicastMessageData rowTable, int resolution);

    /**
      * @brief Compares two structures that contain information about two scheduled GEOBROADCAST messages
      * @param[in] rowReceived Information obtained from ns-3
      * @param[in] rowTable One of the rows that the table contains
      * @return True: If there is a match
      * @return False: If there isn't any coincidence
      */
    bool CompareGeobroadcastRows(ScheduledGeobroadcastMessageData rowReceived, ScheduledGeobroadcastMessageData rowTable, int resolution);

    /**
      * @brief Compares two structures that contain information about two scheduled TOPOBROADCAST messages
      * @param[in] rowReceived Information obtained from ns-3
      * @param[in] rowTable One of the rows that the table contains
      * @return True: If there is a match
      * @return False: If there isn't any coincidence
      */
    bool CompareTopobroadcastRows(ScheduledTopobroadcastMessageData rowReceived, ScheduledTopobroadcastMessageData rowTable);

    /**
    * @brief Inserts information to the table that stores the different idenfitiers associated to a message
    * @param[in] &table Table that contains the identifiers related to a certain message
    * @param[in] actionID Action identifier of the message
    * @param[in] senderID Identifier of the node that sent the message
    * @param[in] receiverID Idenfitier of the node that received the message
    */
    void UpdateIdentifiersTable(std::vector<IdentifiersStorageStruct> &table, actionID_t actionID, stationID_t senderID, stationID_t receiverID);

    /**
    * @brief Groups the receivers of a certain message
    * @param[in] &table Table that contains the identifiers related to a certain message
    * @param[in] actionID Action identifier of the message
    * @param[in] senderID Identifier of the node that sent the message
    * @param[in] receiverID Idenfitier of the node that received the message
    * @return Group of receivers related to a certain message
    */
    std::vector<stationID_t> GroupReceivers(std::vector<IdentifiersStorageStruct> &table, actionID_t actionID, stationID_t senderID);

    /// @brief Collection of existing CAM areas.
    std::vector<V2xCamArea*>* m_v2xCamAreaCollection;

    /// @brief Collection of Geobroadcasting areas.
    std::vector<V2xGeobroadcastArea*>* m_v2xGeobroadcastAreaCollection;
};

}

#endif
