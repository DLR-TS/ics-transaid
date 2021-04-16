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
 ***************************************************************************************/

#ifndef SERVER_H_
#define SERVER_H_

#include "foreign/tcpip/socket.h"
#include "foreign/tcpip/storage.h"
#include "node-handler.h"

namespace baseapp {
namespace server {

class Server {
public:
    static void RunServer(application::BehaviourFactory* factory);
    static NodeHandler* GetNodeHandler();
    static int CurrentTimeStep();
    static int getSUMOStepLength();
    /// Returns the SUMO id for the given ics id
    /// If the icsID is not known, an empty string is returned
    static const std::string& getSumoID(int icsID);

    /// @brief Looks up icsID for given sumo ID, returns false, if the sumoID is not known
    /// @param[in] sumoID sumo id of node for which the ics id should be retrieved
    /// @param[out] icsID here the corresponding ics ID will be written, if the node exists
    static bool getICSID(std::string sumoID, int& icsID);

private:
    Server(application::BehaviourFactory* factory);
    virtual ~Server();

    int dispatchCommand();
    /// @brief Called by the iCS once at the begin of each simulations step
    bool step();
    void updateTimeStep(int current);
    void checkNodeToRemove();
    void writeStatusCmd(int commandId, int status, const std::string& description);

    /// @brief Reads SUMO step length info from input storage.
    bool storeSUMOStepLength();

    bool createMobileNode();
    bool removeMobileNode();
    bool askForSubscription();
    bool endSubscription();
    //bool carsInZone();
    bool mobilityInformation();
    bool applicationMessageReceive();
    bool applicationConfirmSubscription(int commandId);
    bool applicationExecute();
    bool trafficLightInformation();
    bool sumoTraciCommand();
    bool getReceivedCAMinfo();

private:
    static Server* m_instance;
    static bool m_closeConnection;
    tcpip::Socket* m_socket;
    tcpip::Storage m_inputStorage;
    tcpip::Storage m_outputStorage;
    int m_currentTimeStep;
    NodeHandler* m_nodeHandler;
    std::map<int, int> m_lastSeenNodes;
    static const int MAX_NODE_TIMESTEP = 5;
    static int SUMO_STEPLENGTH;
};

} /* namespace server */
} /* namespace protocol */

#endif /* SERVER_H_ */
