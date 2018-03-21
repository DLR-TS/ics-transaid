/****************************************************************************/
/// @file    ns3-client.h
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
#ifndef NS3_CLIENT_H
#define NS3_CLIENT_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <sstream>
#include <vector>

#include "wireless-communication-simulator-communicator.h"
#include "socket-ns3.h"
#include "storage-ns3.h"
#include "ns3-comm-constants.h"
#include "../../utils/ics/iCStypes.h"

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class SocketNs3;
class StorageNs3;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Ns3Client
 * @brief Manages the connection with ns-3.
 */
class Ns3Client: public WirelessComSimulatorCommunicator
{

public:

  /// @brief Constructor.
  Ns3Client();

  /// @brief Destructor.
  ~Ns3Client();

  /**
   * @brief Creates a socket and establishes the connection with ns-3.
   * @return True: If the connection establishes successfully.
   * @return False: If it is impossible to establish the connection.
   */
  bool Connect();

  /**
   * @brief Closes the connection with ns-3.
   * @return True: If the connection closes successfully.
   * @return EXIT_SUCCESS if connection was correctly closed, EXIT_FAILURE otherwise.
   */
  int Close();

  /**
   * @brief Sends a message to ns-3 in order to start its simulation.
   * @param[in] time Timestep in which the simulation of the ns-3 module should start.
   * @return True: If the messages exchange takes places successfully.
   * @return False: If the messages exchange doesn't take places successfully.
   */
  bool CommandSimulationStep(int time);

  /**
   * @brief Sends a message to ns-3 with the new values of the node's position.
   * @param[in] nodeID The ns-3 identifier of the node.
   * @param[in] x The new x coordinate of the node's position.
   * @param[in] y The new y coordinate of the node's position.
   * @return EXIT_SUCCESS if the nodes were correctly moved to their new position in ns-3, EXIT_FAILURE otherwise.
   */
  int CommandUpdateNodePosition(int nodeId, float x, float y);

  /**
   * @brief Sends a message to ns-3 with the new values of the node's position.
   * @param[in] nodeID The ns-3 identifier of the node.
   * @param[in] x The new x coordinate of the node's position.
   * @param[in] y The new y coordinate of the node's position.
   * @param[in] speed The vehicle's speed.
   * @param[in] heading The vehicle's heading.
   * @param[in] laneId The ID of the lane the vehicle is located in.
   * @return EXIT_SUCCESS if the nodes were correctly moved to their new position in ns-3, EXIT_FAILURE otherwise.
   */
  int CommandUpdateNodePosition2(int nodeId, float x, float y, float speed, float heading, std::string laneId);

  /**
   * @brief Sends the order to ns-3 to initialize a node.
   * @param[in] x The new x coordinate of the node's position.
   * @param[in] y The new y coordinate of the node's position.
   * @param[in] ratType The communication technology installed in the node.
   * @return The ID number assigned by ns-3 to the node. -1 in case something went wrong.
   */
  int CommandCreateNode(float x, float y, std::vector<std::string> techList);

  /**
   * @brief Sends the order to ns-3 to initialize a node.
   * @param[in] x The new x coordinate of the node's position.
   * @param[in] y The new y coordinate of the node's position.
   * @param[in] speed The vehicle's speed.
   * @param[in] heading The vehicle's heading.
   * @param[in] laneId The ID of the lane the vehicle is located in.
   * @param[in] ratType The communication technology installed in the node.
   * @return The ID number assigned by ns-3 to the node. -1 in case something went wrong.
   */
  int CommandCreateNode2(float x, float y, float speed, float heading, std::string laneId,
      std::vector<std::string> techList);

  /**
   * @brief Orders to ns-3 to start the Cam messages transmission.
   * @param[in] sendersId The identifiers of the nodes which will be the receivers of the Cam messages sent.
   * @param[in] payloadLength Length of the Cam messages payload.
   * @param[in] frequency Frequency of the Cam messages transmission.
   * @return True: If this operation takes places successfully.
   * @return False: If an error occurs.
   */
  bool CommandStartSendingCam(std::vector<std::string> sendersId, unsigned int payloadLength, float frequency);

  /**
   * @brief Orders to ns-3 to stop the Cam messages transmission.
   * @param[in] sendersId The identifiers of the nodes which were the receivers of the Cam messages sent.
   * @return True: If this operation takes places successfully.
   * @return False: If an error occurs.
   */
  bool CommandStopSendingCam(std::vector<std::string> sendersId);

  /**
   * @brief Asks for the received messages by a certain node.
   * @param[in] nodeID Node identifier.
   * @param[in] receivedMessages Received messages stored by the iCS.
   * @return True: If this operation finishes successfully.
   * @return False: If an error occurs.
   */
  bool CommandGetReceivedMessages(int nodeId, std::vector<ReceivedMessage>* receivedMessages, const int timeResolution);

  /**
	 * @brief Asks for the received messages from all nodes.
	 * @param[in] receivedMessages Received messages stored by the iCS.
	 * @return True: If this operation finishes successfully.
	 * @return False: If an error occurs.
	 */
	bool CommandGetAllReceivedMessages(std::map<int, std::vector<Message> >* receivedMessages, const int timeResolution);

  /**
   * @brief Orders ns-3 to activate a topobroadcast transmision using WAVE and the C2C stack.
   * @param[in] sendersId The identifiers of the nodes that will transmit the topobroadcast packet.
   * @param[in] serviceId ID of the service to be activated.
   * @param[in] the communication profile.
   * @param[in] technologyList List of technologies over which the service can operate.
   * @param[in] frequency Frequency of the topobroadcast message transmission.
   * @param[in] payloadLength Length of the topobroadcast message payload.
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3.
   * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers.
   * @param[in] numHops Number of hops or times that the topobroadcast packets can be retransmitted.
   * @return True: If this operation takes places successfully.
   * @return False: If an error occurs.
   */
  bool CommandStartTopoTxon(std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile,
      std::vector<std::string> technologyList, float frequency, unsigned int payloadLength, float msgRegenerationTime,
      unsigned int msgLifetime, unsigned int numHops, std::vector<unsigned char> *genericContainer);

  /**
   * @brief Order ns-3 to activates a transmision based on the destination node ID. This txon mode can be used to active a unicast or broadcast transmision in a vehicle or a RSU. It works with different radio access technologies (if a vehicle is the sender, e.g. WAVE, UMTS, etc.) and the C2C and IP stacks.
   * @param[in] sendersId The identifiers of the nodes that will transmit the ID-based packet.
   * @param[in] serviceId ID of the service to be activated.
   * @param[in] the communication profile.
   * @param[in] technologyList List of technologies over which the service can operate.
   * @param[in] frequency Frequency of the id-based message transmission.
   * @param[in] payloadLength Length of the id-based message payload.
   * @param[in] destination ID of the destination node.
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3.
   * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers.
   * @return True: If this operation takes places successfully.
   * @return False: If an error occurs.
   */
		bool CommandStartIdBasedTxon(std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile,
				std::vector<std::string> technologyList, float frequency, unsigned int payloadLength,
				unsigned int destination, float msgRegenerationTime, unsigned int msgLifetime, double time, int messageId,
				std::vector<unsigned char> *genericContainer);

  /**
   * @brief order ns-3 to activate the transmission of a TMC notification message in a geographical area. The selection of the CIU will be done by the MW node
   * @param[in] nodeId ID of the node that will transmit the packet
   * @param[in] serviceId ID of the service to be activated
   * @param[in] the communication profile.
   * @param[in] technologies List of technologies over which the service can operate
   * @param[in] destination Geographical area where the packet will be disseminated
   * @param[in] frequency Frequency of the message transmission
   * @param[in] packetSize Size of the message payload
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
   * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
   */
  bool CommandStartMWTxon(std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile,
      std::vector<std::string> technologyList, CircularGeoAddress destination, float frequency, uint32_t packetSize,
      double msgRegenerationTime, uint8_t msgLifetime, std::vector<unsigned char> *genericContainer);

  /**
   * @brief Orders ns-3 to activates an IP transmision based on the destination node ID. This txon mode can be used to active a unicast, broadcast or multicast transmision in a IP-based base station or CIU (e.g. UMTS base stations, WiMAX base station, etc.).
   * @param[in] sendersId The identifiers of the CIU nodes that will transmit the IP packet.
   * @param[in] serviceId ID of the service to be activated.
   * @param[in] frequency Frequency of the IP message transmission.
   * @param[in] payloadLength Length of the IP message payload.
   * @param[in] destination ID of the destination node.
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3.
   * @return True: If this operation takes places successfully.
   * @return False: If an error occurs.
   */
  bool CommandStartIpCiuTxon(std::vector<std::string> sendersId, std::string serviceId, float frequency,
      unsigned int payloadLength, unsigned int destination, float msgRegenerationTime,
      std::vector<unsigned char> *genericContainer);

  /**
   * @brief Orders ns-3 to deactivate the txon of a service running in a vehicle or a RSU.
   * @param[in] sendersId The identifiers of the nodes running the service to be deactivated.
   * @param[in] serviceId ID of the service to be deactivated.
   * @return True: If this operation takes places successfully.
   * @return False: If an error occurs.
   */
  bool CommandStopServiceTxon(std::vector<std::string> sendersId, std::string serviceId);

  /**
   * @brief Orders ns-3 to deactivate the txon of a service running in an IP-based base station or CIU.
   * @param[in] sendersId The identifiers of the CIU nodes running the service to be deactivated.
   * @param[in] serviceId ID of the service to be deactivated.
   * @return True: If this operation takes places successfully.
   * @return False: If an error occurs.
   */
  bool CommandStopIpCiuServiceTxon(std::vector<std::string> sendersId, std::string serviceId);

  /**
   * @brief Orders ns-3 to deactivate the txon of a service running in the TMC
   * @param[in] sendersId The identifiers of the TMC the service to be deactivated.
   * @param[in] serviceId ID of the service to be deactivated.
   * @return True: If this operation takes places successfully.
   * @return False: If an error occurs.
   */
  bool CommandStopMWServiceTxon(std::vector<std::string> sendersId, std::string serviceId);

  /**
   * @brief Activates a geobroadcast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
   * @param[in] sendersId The identifiers of the nodes that will transmit the geobroadcast packet.
   * @param[in] serviceId ID of the service to be activated.
   * @param[in] commProfile Communication Profile
   * @param[in] technList List of available preferred technologies
   * @param[in] destination Destination area defined as a circle.
   * @param[in] frequency Frequency of the geobraodcast message transmission.
   * @param[in] payloadLength Length of the geobroadcast message payload.
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3.
   * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers.
   * @return True: If this operation takes places successfully.
   * @return False: If an error occurs.
   */
		bool StartGeobroadcastTxon(std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile,
				std::vector<std::string> technList, CircularGeoAddress destination, float frequency,
				unsigned int payloadLength, float msgRegenerationTime, unsigned int msgLifetime, double time, int messageId,
				std::vector<unsigned char> *genericContainer);

  /**
   * @brief Activates a geoanycast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
   * @param[in] sendersId The identifiers of the nodes that will transmit the geoanycast packet.
   * @param[in] serviceId ID of the service to be activated.
   * @param[in] commProfile Communication Profile.
   * @param[in] technList List of available preferred technologies.
   * @param[in] destination Destination area defined as a circle.
   * @param[in] frequency Frequency of the geoanycast message transmission.
   * @param[in] payloadLength Length of the geoanycast message payload.
   * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3.
   * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers.
   * @return True: If this operation takes places successfully.
   * @return False: If an error occurs.
   */
  bool StartGeoanycastTxon(std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile,
      std::vector<std::string> technList, CircularGeoAddress destination, float frequency, unsigned int payloadLength,
      float msgRegenerationTime, unsigned int msgLifetime, std::vector<unsigned char> *genericContainer);

  /**
   * @brief Activate the nodes in ns3 and all their communication modules, e.g. PHY layer
   * @param[in] nodesId IDs of the nodes to be activated
   */
  virtual bool CommandActivateNode(const std::vector<int> & nodesId);

  /**
   * @brief Deactivate the nodes in ns3 and all their communication modules, e.g. PHY layer
   * @param[in] nodesId IDs of the nodes to be deactivated
   */
  virtual bool CommandDeactivateNode(const std::vector<int> &  nodesId);

  /// @brief Sends a command close message.
  int CommandClose();

  /// @brief Port used for the connection with ns-3.
  int m_port;

  /// @brief Host in which ns-3 is running.
  std::string m_host;

private:
  /**
   * @brief Validates the received command responses.
   * @param[in,out] &inMsg Message received.
   * @param[in] command Command received.
   * @return True: If the command messages match.
   * @return False: If the command messages don't match.
   */
  bool ReportResultState(StorageNs3& inMsg, int command);

  /// @brief Socket for the connection.
  SocketNs3* m_socket;

  /// @brief String to identify Cam messages.
  static std::string CAM_TYPE;

  /// @brief String to identify DENM messages.
  static std::string DENM_TYPE;

  /// @todo To be commented.
  int CalculateStringListByteSize(std::vector<std::string> list);

  ics_types::messageType_t ParseMessageType(std::string & messageTypeString);

};

}
#endif
