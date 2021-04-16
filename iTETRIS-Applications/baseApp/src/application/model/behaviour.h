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

#ifndef BEHAVIOUR_H_
#define BEHAVIOUR_H_

#include <memory>
#include "trace-manager.h"
#include "headers.h"
#include "fatal-error.h"
#include "structs.h"
#include "node.h"
#include "libsumo/TraCIDefs.h"
#include "libsumo/TraCIConstants.h"

namespace baseapp {
namespace server {
class Payload;
}

namespace application {

class iCSInterface;
class Node;
struct Command;
struct CommandInfo;

/// @brief Structure to hold the TraCI responses for all GET-commands
/// @brief maps: objID -> CMD -> Response-object (time x value)
/// @todo  Consider including the domain as top level. (Problem: it is not sent to the app currently)
typedef std::map<std::string, std::map<int, std::pair <int, std::shared_ptr<libsumo::TraCIResult> > > > TraCIResponseMap;

//executionId - <CommandInfo, TraCIResult>
typedef std::map<int, std::pair <std::shared_ptr<CommandInfo>, std::shared_ptr<libsumo::TraCIResult> > > TraCIResponseMapId;

// maps: objID -> parameterKey -> Response-object (time x value)
typedef std::map<std::string, std::map<std::string, std::pair<int, std::shared_ptr<libsumo::TraCIResult> > > > TraCIResponseMapParameterKey;

/**
* Abstract behaviour class
	 */
class Behaviour: public TraceManager {
public:
    static uint16_t DefaultResponseTimeSpacing;
public:
    Behaviour(iCSInterface* controller);
    virtual ~Behaviour();

    virtual bool IsActiveOnStart(void) const;
    bool IsRunning() const;
    /**
     * @brief Contains the actions to be executed when the behavior starts
     */
    virtual void Start();
    /**
     * @brief Contains the actions to be executed when the behavior stops
     */
    virtual void Stop();


    /** @brief OnAddSubscriptions is called at the begin of the app-interaction phase of the iCS simstep.
     */
    virtual void OnAddSubscriptions();

    /**
     * @brief If a message of the specified pid should be forwarded to the class
     * @param[in] pid the ProtocolId of the message
     * @return true if the class is interested messages of said pid. False otherwise
     */
    virtual bool IsSubscribedTo(ProtocolId pid) const = 0;
    /**
     * @brief Called by the ics-interface if a message is received by the node
     * @brief and its pid is relevant to the behavior
     * @param[in] payload The received message
     * @param[in] snr The snr of the reception from ns3
     */
    virtual void Receive(server::Payload* payload, double snr) = 0;

    /**
     * @brief Called by ics-interface to get data to send back to iCS.
     * @brief If the class does not returns data it has to return false.
     * @param[out] data Data to send back to iCS. The application has to fill the map
     * @return Whatever the application executed. If true data will be sent to iCS. If false data is discarded
     */
    virtual bool Execute(DirectionValueMap& data);

    /**
     * @brief Called by the ics-interface if a CAM message is received by the node
     * @param[in] the CAM received message
     */
    virtual void processCAMmessagesReceived(const int nodeID, const std::vector<CAMdata>& receivedCAMmessages);

    /// @brief Called whenever a GET-command result is returned from TraCI.
    ///        Writes the according information into TraCIResponses
    /// @note if this is overridden, the call to Behaviour::processTraCIResult should be
    ///       included in the derived method to store the response
    virtual void processTraCIResult(const int result, const Command& command, const int executionId = -1);
    virtual void processTraCIResult(const double result, const Command& command, const int executionId = -1);
    virtual void processTraCIResult(const std::string result, const Command& command, const int executionId = -1);
    virtual void processTraCIResult(const std::vector<std::string> result, const Command& command, const int executionId = -1);
    void processTraCIResult(std::shared_ptr<libsumo::TraCIColor> color, const Command& command, const int executionId = -1);
    void processTraCIResult(std::shared_ptr<libsumo::TraCILeaderDistance> leaderDist, const Command& command, const int executionId = -1);
    void processTraCIResult(std::shared_ptr<libsumo::TraCINextStopDataVector> stops, const Command& command, const int executionId = -1);
    virtual void processTraCIResult(std::shared_ptr<libsumo::TraCIPosition> position, const Command& command, const int executionId = -1);
    virtual void processTraCIResult(std::shared_ptr<baseapp::TraCIPair2Int> result, const Command& command, const int executionId = -1);
    virtual void processTraCIResult(std::shared_ptr<baseapp::TraCIVectorPair> result, const Command& command, const int executionId = -1);
    void processTraCIResult(std::shared_ptr<baseapp::TraCIParameterWithKey> paramWithKey, const Command& command, const int executionId = -1);


    virtual TypeBehaviour GetType() const {
        return Type();
    }

    static TypeBehaviour Type() {
        return TYPE_BEHAVIOUR;
    }

    /// @brief return time and result object for the last TraCI response received for the given object and variable
    /// returns noResponse if no entry exists in TraCIResponses
    static const std::pair<int, std::shared_ptr<libsumo::TraCIResult> >& GetLastTraCIResponse(std::string objID, int variableID, std::string parameterKey = INVALID_STRING);

    static const std::pair<std::shared_ptr<CommandInfo>, std::shared_ptr<libsumo::TraCIResult> >& getTraCIResponse(int executionId);

protected:
    virtual std::string Log() const;
    iCSInterface* GetController() const;

    /// @brief noResponse is returned by @getLastTraCIResponse if there is no entry for the requested
    /// object/command in TraCIResponses
    static std::pair<int, std::shared_ptr<libsumo::TraCIResult> > noResponse;

    static std::pair<std::shared_ptr<CommandInfo>, std::shared_ptr<libsumo::TraCIResult> > noResponseId;

    bool m_enabled;

private:
    /// @brief Stores TraCI response in TraCIResponses
    virtual void storeTraCIResult(const int time, const std::shared_ptr<libsumo::TraCIResult> result, const Command& command, const int executionId);

    virtual void storeTraCIResultId(const int executionId, const int time, const std::shared_ptr<libsumo::TraCIResult> result, const Command& command);

    virtual void storeTraCIResultParameterKey(const int time, const std::shared_ptr<libsumo::TraCIResult> result, const Command& command);

    iCSInterface* m_controller;
    bool m_running;

    // trace sources
    TracedCallback<bool> m_traceStartToggle;

    /// @brief Structure to hold the TraCI responses for all GET-commands
    /// @todo  Consider including the domain as top level. (Problem: it is not sent to the app currently)
    /// maps: objID -> CMD -> Response (time x value)
    static TraCIResponseMap TraCIResponses;
    static TraCIResponseMapId TraCIResponsesId;
    static TraCIResponseMapParameterKey TraCIResponsesParameterKey;
};

} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_H_ */
