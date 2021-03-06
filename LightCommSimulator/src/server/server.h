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
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 *
 * Author Alejandro Correa Vila <acorrea@umh.es> Universidad Miguel Hernandez
 ***************************************************************************************/

#ifndef SERVER_H_
#define SERVER_H_

#include "tcpip/server-socket.h"
#include "tcpip/storage.h"
#include "scheduler.h"



namespace lightcomm {
namespace server {

class Server {
public:
    static void RunServer();
    static int CurrentTimeStep();


private:
    Server();
    virtual ~Server();

    int dispatchCommand();

    void writeStatusCmd(int commandId, int status, std::string description);

    /**
     * @brief In the Lightcomms there is are no task for the running sim step so this function only updates the timestep
     */
    bool RunSimStep(int time);

    /**
     * @brief Create a new node (vehicle or CIU) specifying its initial position
     */
    bool CreateNode();

    /**
     * @brief Create a new node (vehicle or CIU) specifying its initial position, speed, heading and laneId
     */
    bool CreateNode2();

    /**
     * @brief Update node's position
     */
    bool UpdateNodePosition();

    /**
     * @brief Update node's position, speed, heading and laneId
     */
    bool UpdateNodePosition2();


    /**
     * @brief Activate a node and all its communication modules, e.g. PHY layer
     */
    bool ActivateNode(void);

    /**
     * @brief Deactivate a node and all its communication modules, e.g. PHY layer
     */
    bool DeactivateNode(void);

    /**
     * @brief Start sending CAM in the nodes indicated in a list of nodes
     */
    bool StartSendingCam();

    /**
     * @brief Stop sending CAM in the nodes indicated in a list of nodes
     */
    bool StopSendingCam();

    /**
     * @brief Retrieve the messages that have been received by the simulated nodes
     */
    bool GetReceivedMessages();

    /**
         * @brief Retrieve all the messages that have been received by the all the simulated nodes
    */
    bool GetAllReceivedMessages();


    bool Close();
    int CalculateStringListByteSize(std::vector<std::string> list);

    bool StartTopoTxon(void);

    /**
     * @brief Activate a transmision based on the destination node ID. This txon mode can be used to active a unicast or broadcast transmision in a vehicle or a RSU. It works with different radio access technologies (if a vehicle is the sender, e.g. WAVE, UMTS, etc.) and the C2C and IP stacks.
     */
    bool StartIdBasedTxon(void);

    /**
     * @brief Activate a transmision from the TMC based on a geographic area. This txon mode can be used to activate any kind of transmission supported by the Technology Selector according the ServiceID rules.
    */
    bool StartMWTxon(void);

    /**
     * @brief Activate a transmision based on the destination node ID. This txon mode can be used to active a unicast, broadcast or multicast transmision in a IP-based base station or CIU (e.g. UMTS base stations, WiMAX base station, etc.)
     */
    bool StartIpCiuTxon(void);

    /**
     * @brief Activate a geobroadcast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
     */
    bool StartGeobroadcastTxon(void);

    /**
     * @brief Activate a geoanycast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
     */
    bool StartGeoanycastTxon(void);

    /**
     * @brief Deactivate the txon of a service running in a vehicle or a RSU.
     */
    bool StopServiceTxon(void);

    /**
     * @brief Deactivate the txon of a service running in IP-based base station or CIU.
     */
    bool StopIpCiuServiceTxon(void);

    /**
     * @brief Deactivate the MWtxon of a MW service running on a TMC.
     */
    bool StopMWServiceTxon(void);




private:
    static Server* m_instance;
    static bool m_closeConnection;


    tcpip::ServerSocket* m_socket;
    tcpip::Storage m_inputStorage;
    tcpip::Storage m_outputStorage;
    int m_currentTimeStep;
    event_id m_eventBroadcast;

    // replicate nodeID of NS-2 in the lightcomm simulator
    int nodeId;

    struct Message {
        Message() {
            //			Can delete it even if I've never assigned it
            packetTagContainer = NULL;
        }
        int senderId;
        int messageId;
        std::string messageType;
        int timeStep;
        int sequenceNumber;
        //std::vector<unsigned char>* packetTagContainer;
        tcpip::Storage* packetTagContainer;
        float frequency;

    } typedef Message;


    struct NodeData {
        float posX;
        float posY;
        std::string type;
    };

    // Table that stores all the received messages
    std::map<int, std::vector<Message> > m_GeneralReceivedMessageMap;

    // Table that stores the nodeId and the position of the node
    std::map<int, NodeData> m_NodeMap;

    // Table that stores the event_id of the CAM scheduled
    std::map<int, event_id> m_CAMeventIDMap;

    void ScheduleMessageTx(Message msg);
    std::vector<int> GetReceivers(int nodeId);
};

} /* namespace server */
} /* namespace lightcomm */

#endif /* SERVER_H_ */
