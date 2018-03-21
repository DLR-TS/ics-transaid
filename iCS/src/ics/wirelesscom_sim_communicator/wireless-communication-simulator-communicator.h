/****************************************************************************/
/// @file    wireless-communication-simulator-communicator.h
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
#ifndef WIRELES_COMMUNICATION_SIMULATOR_COMMUNICATOR_H
#define WIRELES_COMMUNICATION_SIMULATOR_COMMUNICATOR_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <string>
#include "../../utils/ics/iCStypes.h"
#include "ns3-comm-constants.h"

namespace ics
{

// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @struct ReceivedMessage
 * @brief Contains information about the received messages
 */
struct ReceivedMessage
{
  ics_types::stationID_t senderId;
  ics_types::messageType_t messageType;
  ics_types::icstime_t timeStep;
  ics_types::seqNo_t sequenceNumber;
  std::vector<unsigned char>* packetTagContainer;
  int messageId;
  /* ics_types::rssi_t rssi;
   ics_types::snr_t snr;*/
};

struct Message
{
		Message()
		{
//			Can delete it even if I've never assigned it
			packetTagContainer = NULL;
		}
		ics_types::stationID_t senderNs3Id;
		ics_types::stationID_t senderIcsId;
		ics_types::stationID_t receiverNs3Id;
		ics_types::stationID_t receiverIcsId;
		ics_types::messageType_t messageType;
		ics_types::icstime_t timeStep;
		ics_types::seqNo_t sequenceNumber;
		std::vector<unsigned char>* packetTagContainer;
		int messageId;
		ics_types::actionID_t actionId;
		bool received;
		std::string subscriptionType;
		int appMessageId;
}typedef Message;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class WirelessComSimulatorCommunicator
 * @brief Contains virtual functions to manage the connection with ns-3
 */
class WirelessComSimulatorCommunicator
{
public:

  /**
   * @brief Creates a socket and establishes the connection with ns-3
   * @return True: If the connection establishes successfully
   * @return False: If it is impossible to establish the connection
   */
  virtual bool Connect() = 0;

  /**
   * @brief Closes the connection with ns-3
   * @return True: If the connection closes successfully
   * @return False: If an error occurs trying to close the connection
   */
  virtual int Close() = 0;

  /**
   * @brief Sends a message to ns-3 in order to start its simulation
   * @param[in] time Timestep in which the simulation of the ns-3 module should start
   * @return True: If the messages exchange takes places successfully
   * @return False: If the messages exchange doesn't take places successfully
   */
  virtual bool CommandSimulationStep(int time) = 0;

  /**
   * @brief Sends a message to ns-3 with the new values of the node's position
   * @param[in] nodeID The ns-3 identifier of the node
   * @param[in] x The new x coordinate of the node's position
   * @param[in] y The new y coordinate of the node's position
   * @return EXIT_SUCCESS if the nodes were correctly moved to their new position, EXIT_FAILURE otherwise
   */
  virtual int CommandUpdateNodePosition(int nodeId, float x, float y) = 0;

  // New 16-09-10
  /**
   * @brief Sends a message to ns-3 with the new values of the node's position
   * @param[in] nodeID The ns-3 identifier of the node
   * @param[in] x The new x coordinate of the node's position
   * @param[in] y The new y coordinate of the node's position
   * @param[in] speed The vehicle's speed
   * @param[in] heading The vehicle's heading
   * @param[in] laneId The ID of the lane the vehicle is located in
   * @return EXIT_SUCCESS if the nodes were correctly moved to their new position in ns-3, EXIT_FAILURE otherwise
   */
  virtual int CommandUpdateNodePosition2(int nodeId, float x, float y, float speed, float heading,
      std::string laneId) = 0;

  /**
   * @brief Sends the order to ns-3 to initializa a node.
   * @param[in] x The new x coordinate of the node's position
   * @param[in] y The new y coordinate of the node's position
   * @param[in] ratType The communication technology installed in the node.
   * @return The ID number assigned by ns-3 to the node. -1 in case something went wrong.
   */
  virtual int CommandCreateNode(float x, float y, std::vector<std::string> techList) = 0;

  // New 16-09-10
  /**
   * @brief Sends the order to ns-3 to initialize a node.
   * @param[in] x The new x coordinate of the node's position
   * @param[in] y The new y coordinate of the node's position
   * @param[in] speed The vehicle's speed
   * @param[in] heading The vehicle's heading
   * @param[in] laneId The ID of the lane the vehicle is located in
   * @param[in] ratType The communication technology installed in the node.
   * @return The ID number assigned by ns-3 to the node. -1 in case something went wrong.
   */
  virtual int CommandCreateNode2(float x, float y, float speed, float heading, std::string laneId,
      std::vector<std::string> techList) = 0;

  /**
   * @brief Orders to ns-3 to start the Cam messages transmission
   * @param[in] sendersId The identifiers of the nodes which will be the receivers of the Cam messages sent
   * @param[in] payloadLength Length of the Cam messages payload
   * @param[in] frequency Frequency of the Cam messages transmission
   * @return True: If this operation takes places successfully
   * @return False: If an error occurs
   */
  virtual bool CommandStartSendingCam(std::vector<std::string> sendersId, unsigned int payloadLength,
      float frequency) = 0;

  /**
   * @brief Orders to ns-3 to stop the Cam messages transmission
   * @param[in] sendersId The identifiers of the nodes which were the receivers of the Cam messages sent
   * @return True: If this operation takes places successfully
   * @return False: If an error occurs
   */
  virtual bool CommandStopSendingCam(std::vector<std::string> sendersId) = 0;

  /**
   * @brief Asks for the received messages by a certain node and checks if everythig is correct
   * @param[in] nodeID Node identifier
   * @param[in] receivedMessages Received messages stored by the iCS
   * @return True: If this operation finishes successfully
   * @return False: If an error occurs
   */
  virtual bool CommandGetReceivedMessages(int nodeId, std::vector<ReceivedMessage>* receivedMessages,
      const int timeResolution) = 0;

  /**
	 * @brief Asks for the received messages from all nodes.
	 * @param[in] receivedMessages Received messages stored by the iCS.
	 * @return True: If this operation finishes successfully.
	 * @return False: If an error occurs.
	 */
  virtual bool CommandGetAllReceivedMessages(std::map<int, std::vector<Message> >*, const int timeResolution) = 0;

  /**
   * @brief Orders ns-3 to activate a topobroadcast transmission using WAVE and the C2C stack
   * @param[in] sendersId The identifiers of the nodes that will transmit the topobroadcast packet
   * @param[in] serviceId ID of the service to be activated
   * @param[in] commProfile Communication Profile.
   * @param[in] technologyList List of technologies over which the service can operate
   * @param[in] frequency Frequency of the topobroadcast message transmission
   * @param[in] payloadLength Length of the topobroadcast message payload
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
   * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
   * @param[in] numHops Number of hops or times that the topobroadcast packets can be retransmitted
   * @param[in] genericContainer A generic container to pass any type of information between the application module and the network simulator
   * @return True: If this operation takes places successfully
   * @return False: If an error occurs
   */
  virtual bool CommandStartTopoTxon(std::vector<std::string> sendersId, std::string serviceId,
      unsigned char commProfile, std::vector<std::string> technologyList, float frequency, unsigned int payloadLength,
      float msgRegenerationTime, unsigned int msgLifetime, unsigned int numHops,
      std::vector<unsigned char> *genericContainer) = 0;

  /**
   * @brief Order ns-3 to activates a transmission based on the destination node ID. This txon mode can be used to active a unicast or broadcast transmission in a vehicle or a RSU. It works with different radio access technologies (if a vehicle is the sender, e.g. WAVE, UMTS, etc.) and the C2C and IP stacks
   * @param[in] sendersId The identifiers of the nodes that will transmit the ID-based packet
   * @param[in] serviceId ID of the service to be activated
   * @param[in] commProfile Communication Profile.
   * @param[in] technologyList List of technologies over which the service can operate
   * @param[in] frequency Frequency of the id-based message transmission
   * @param[in] payloadLength Length of the id-based message payload
   * @param[in] destination ID of the destination node
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
   * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
   * @param[in] genericContainer A generic container to pass any type of information between the application module and the network simulator
   * @return True: If this operation takes places successfully
   * @return False: If an error occurs
   */
	virtual bool CommandStartIdBasedTxon(std::vector<std::string> sendersId, std::string serviceId,
			unsigned char commProfile, std::vector<std::string> technologyList, float frequency,
			unsigned int payloadLength, unsigned int destination, float msgRegenerationTime, unsigned int msgLifetime,
			double time, int messageId, std::vector<unsigned char> *genericContainer) = 0;

  /**
   * @brief order ns-3 to activate the transmission of a TMC notification message in a geographical area. The selection of the CIU will be done by the MW node
   * @param[in] nodeId ID of the node that will transmit the packet
   * @param[in] serviceId ID of the service to be activated
   * @param[in] commProfile Communication Profile.
   * @param[in] technologies List of technologies over which the service can operate
   * @param[in] destination Geographical area where the packet will be disseminated
   * @param[in] frequency Frequency of the message transmission
   * @param[in] packetSize Size of the message payload
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
   * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
   * @param[in] genericContainer A generic container to pass any type of information between the application module and the network simulator
   */
  virtual bool CommandStartMWTxon(std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile,
      std::vector<std::string> technologyList, CircularGeoAddress destination, float frequency, uint32_t packetSize,
      double msgRegenerationTime, uint8_t msgLifetime, std::vector<unsigned char> *genericContainer) = 0;

  /**
   * @brief Orders ns-3 to activates an IP transmission based on the destination node ID. This txon mode can be used to active a unicast, broadcast or multicast transmission in a IP-based base station or CIU (e.g. UMTS base stations, WiMAX base station, etc.)
   * @param[in] sendersId The identifiers of the CIU nodes that will transmit the IP packet
   * @param[in] serviceId ID of the service to be activated
   * @param[in] frequency Frequency of the IP message transmission
   * @param[in] payloadLength Length of the IP message payload
   * @param[in] destination ID of the destination node
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
   * @param[in] genericContainer A generic container to pass any type of information between the application module and the network simulator
   * @return True: If this operation takes places successfully
   * @return False: If an error occurs
   */
  virtual bool CommandStartIpCiuTxon(std::vector<std::string> sendersId, std::string serviceId, float frequency,
      unsigned int payloadLength, unsigned int destination, float msgRegenerationTime,
      std::vector<unsigned char> *genericContainer) = 0;

  /**
   * @brief Orders ns-3 to deactivate the txon of a service running in a vehicle or a RSU.
   * @param[in] sendersId The identifiers of the nodes running the service to be deactivated
   * @param[in] serviceId ID of the service to be deactivated
   * @return True: If this operation takes places successfully
   * @return False: If an error occurs
   */
  virtual bool CommandStopServiceTxon(std::vector<std::string> sendersId, std::string serviceId) = 0;

  /**
   * @brief Orders ns-3 to deactivate the txon of a service running in an IP-based base station or CIU.
   * @param[in] sendersId The identifiers of the CIU nodes running the service to be deactivated
   * @param[in] serviceId ID of the service to be deactivated
   * @return True: If this operation takes places successfully
   * @return False: If an error occurs
   */
  virtual bool CommandStopIpCiuServiceTxon(std::vector<std::string> sendersId, std::string serviceId) = 0;

  /**
   * @brief Orders ns-3 to deactivate the txon of a service running in the TMC
   * @param[in] sendersId The identifiers of the TMC the service to be deactivated.
   * @param[in] serviceId ID of the service to be deactivated.
   * @return True: If this operation takes places successfully.
   * @return False: If an error occurs.
   */
  virtual bool CommandStopMWServiceTxon(std::vector<std::string> sendersId, std::string serviceId) = 0;

  /**
   * @brief Activates a geobroadcast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
   * @param[in] sendersId The identifiers of the nodes that will transmit the geobroadcast packet
   * @param[in] serviceId ID of the service to be activated
   * @param[in] destination Destination area defined as a circle
   * @param[in] commProfile Communication Profile.
   * @param[in] technList List of available preferred technologies.
   * @param[in] frequency Frequency of the geobraodcast message transmission
   * @param[in] payloadLength Length of the geobroadcast message payload
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
   * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
   * @param[in] genericContainer A generic container to pass any type of information between the application module and the network simulator
   * @return True: If this operation takes places successfully
   * @return False: If an error occurs
   */
	virtual bool StartGeobroadcastTxon(std::vector<std::string> sendersId, std::string serviceId,
			unsigned char commProfile, std::vector<std::string> technList, CircularGeoAddress destination,
			float frequency, unsigned int payloadLength, float msgRegenerationTime, unsigned int msgLifetime, double time,
			int messageId, std::vector<unsigned char> *genericContainer) = 0;

  /**
   * @brief Activates a geoanycast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters
   * @param[in] sendersId The identifiers of the nodes that will transmit the geoanycast packet
   * @param[in] serviceId ID of the service to be activated
   * @param[in] destination Destination area defined as a circle
   * @param[in] commProfile Communication Profile.
   * @param[in] technList List of available preferred technologies.
   * @param[in] frequency Frequency of the geoanycast message transmission
   * @param[in] payloadLength Length of the geoanycast message payload
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
   * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
   * @param[in] genericContainer A generic container to pass any type of information between the application module and the network simulator
   * @return True: If this operation takes places successfully
   * @return False: If an error occurs
   */
  virtual bool StartGeoanycastTxon(std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile,
      std::vector<std::string> technList, CircularGeoAddress destination, float frequency, unsigned int payloadLength,
      float msgRegenerationTime, unsigned int msgLifetime, std::vector<unsigned char> *genericContainer) = 0;

  /**
   * @brief Activate the nodes in the wireless network simulator and all their communication modules, e.g. PHY layer
   * @param[in] nodesId IDs of the nodes to be activated
   */
  virtual bool CommandActivateNode(const std::vector<int> & nodesId) = 0;

  /**
   * @brief Deactivate the nodes in the wireless network simulator and all their communication modules, e.g. PHY layer
   * @param[in] nodesId IDs of the nodes to be deactivated
   */
  virtual bool CommandDeactivateNode(const std::vector<int> &  nodesId) = 0;

  /**
   * @brief Sends a command close message
   */
  virtual int CommandClose() = 0;

};

}

#endif
