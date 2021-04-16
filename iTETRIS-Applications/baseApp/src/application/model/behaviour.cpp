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

#include "behaviour.h"

#include "ics-interface.h"
#include "log/log.h"
#include "program-configuration.h"
#include "libsumo/TraCIDefs.h"
#include "current-time.h"

namespace baseapp {
namespace application {

uint16_t Behaviour::DefaultResponseTimeSpacing = 10;
TraCIResponseMap Behaviour::TraCIResponses;
TraCIResponseMapId Behaviour::TraCIResponsesId;
TraCIResponseMapParameterKey Behaviour::TraCIResponsesParameterKey;

std::pair<int, std::shared_ptr<libsumo::TraCIResult> > Behaviour::noResponse = std::make_pair(0.0, nullptr);

std::pair<std::shared_ptr<CommandInfo>, std::shared_ptr<libsumo::TraCIResult> > Behaviour::noResponseId = std::make_pair(nullptr, nullptr);

Behaviour::Behaviour(iCSInterface* controller) :
    m_running(false), m_enabled(true) {
    m_controller = controller;
    RegisterTrace("StartToggle", m_traceStartToggle);
}

Behaviour::~Behaviour() {
    //do not delete m_controller here
}

bool Behaviour::IsActiveOnStart() const {
    return true;
}

bool Behaviour::IsRunning() const {
    return m_running;
}

iCSInterface* Behaviour::GetController() const {
    return m_controller;
}

void Behaviour::Start() {
    NS_LOG_FUNCTION(Log());
    if (m_running) {
        NS_LOG_ERROR(Log() << "Was already on");
    }
    m_running = true;
    m_traceStartToggle(true);
}

void Behaviour::Stop() {
    NS_LOG_FUNCTION(Log());
    m_running = false;
    m_traceStartToggle(false);
}

void Behaviour::OnAddSubscriptions() {
//            NS_LOG_FUNCTION(Log());
}

std::string Behaviour::Log() const {
    std::ostringstream outstr;
    outstr << m_controller->NodeName() << ": " << ToString(this->GetType()) << ": ";
    return outstr.str();
}

bool Behaviour::Execute(DirectionValueMap& data) {
    return false;
}

void Behaviour::processCAMmessagesReceived(const int nodeID, const std::vector<CAMdata>& receivedCAMmessages)
{}

void Behaviour::processTraCIResult(const int result, const Command& command, const int executionId) {
    NS_LOG_INFO(m_controller->LogNode() << "iCSInferface::TraciCommandResult of " << command.objId << " for variable " << Log::toHex(command.variableId, 2) << " is " << result);
    if (command.type == GET_COMMAND) {
        std::shared_ptr<libsumo::TraCIResult> res = std::dynamic_pointer_cast<libsumo::TraCIResult>(std::make_shared<libsumo::TraCIInt>(result));
        const int time = CurrentTime::Now();
        storeTraCIResult(time, res, command, executionId);
    }
}

void Behaviour::processTraCIResult(const double result, const Command& command, const int executionId) {
    NS_LOG_INFO(m_controller->LogNode() << "iCSInferface::TraciCommandResult of " << command.objId << " for variable " << Log::toHex(command.variableId, 2) << " is " << result);
    if (command.type == GET_COMMAND) {
        std::shared_ptr<libsumo::TraCIResult> res = std::dynamic_pointer_cast<libsumo::TraCIResult>(std::make_shared<libsumo::TraCIDouble>(result));
        const int time = CurrentTime::Now();
        storeTraCIResult(time, res, command, executionId);
    }
}

void Behaviour::processTraCIResult(const std::string result, const Command& command, const int executionId) {
    NS_LOG_INFO(m_controller->LogNode() << "iCSInferface::TraciCommandResult of " << command.objId << " for variable " << Log::toHex(command.variableId, 2) << " is " << result);
    if (command.type == GET_COMMAND) {
        std::shared_ptr<libsumo::TraCIResult> res = std::dynamic_pointer_cast<libsumo::TraCIResult>(std::make_shared<libsumo::TraCIString>(result));
        const int time = CurrentTime::Now();
        storeTraCIResult(time, res, command, executionId);
    }
}

void Behaviour::processTraCIResult(std::shared_ptr<libsumo::TraCIColor> color, const Command& command, const int executionId) {
    NS_LOG_INFO(m_controller->LogNode() << "iCSInferface::TraciCommandResult of " << command.objId << " for variable " << Log::toHex(command.variableId, 2) << " is " << color->getString());
    if (command.type == GET_COMMAND) {
        std::shared_ptr<libsumo::TraCIResult> res = std::dynamic_pointer_cast<libsumo::TraCIResult>(color);
        const int time = CurrentTime::Now();
        storeTraCIResult(time, res, command, executionId);
    }
}

void Behaviour::processTraCIResult(std::shared_ptr<libsumo::TraCILeaderDistance> leaderDist, const Command& command, const int executionId) {
    NS_LOG_INFO(m_controller->LogNode() << "iCSInferface::TraciCommandResult of " << command.objId << " for variable " << Log::toHex(command.variableId, 2) << " is " << leaderDist->getString());
    if (command.type == GET_COMMAND) {
        std::shared_ptr<libsumo::TraCIResult> res = std::dynamic_pointer_cast<libsumo::TraCIResult>(leaderDist);
        const int time = CurrentTime::Now();
        storeTraCIResult(time, res, command, executionId);
    }
}

void Behaviour::processTraCIResult(std::shared_ptr<libsumo::TraCINextStopDataVector> stops, const Command& command, const int executionId) {
    NS_LOG_INFO(m_controller->LogNode() << "iCSInferface::TraciCommandResult of " << command.objId << " for variable " << Log::toHex(command.variableId, 2) << " is " << stops->getString());
    if (command.type == GET_COMMAND) {
        std::shared_ptr<libsumo::TraCIResult> res = std::dynamic_pointer_cast<libsumo::TraCIResult>(stops);
        const int time = CurrentTime::Now();
        storeTraCIResult(time, res, command, executionId);
    }
}

void Behaviour::processTraCIResult(std::shared_ptr<baseapp::TraCIParameterWithKey> paramWithKey, const Command& command, const int executionId) {
    NS_LOG_INFO(m_controller->LogNode() << "iCSInferface::TraciCommandResult of " << command.objId << " for variable " << Log::toHex(command.variableId, 2) << " is " << paramWithKey->getString());
    if (command.type == GET_COMMAND) {
        std::shared_ptr<libsumo::TraCIResult> res = std::dynamic_pointer_cast<libsumo::TraCIResult>(paramWithKey);
        const int time = CurrentTime::Now();
        storeTraCIResult(time, res, command, executionId);
    }
}

void Behaviour::processTraCIResult(const std::vector<std::string> result, const Command& command, const int executionId) {
    std::stringstream ss;
    ss << "[";
    for (std::vector<std::string>::const_iterator i = result.begin(); i != result.end(); ++i) {
        ss  << *i << ", ";
    }
    ss << "]";
    NS_LOG_INFO(m_controller->LogNode() << "iCSInferface::TraciCommandResult of " << command.objId << " is " << ss.str());
    if (command.type == GET_COMMAND) {
        // bare pointer base
        std::shared_ptr<libsumo::TraCIStringList> list = std::make_shared<libsumo::TraCIStringList>();
        list->value = result;
        std::shared_ptr<libsumo::TraCIResult> res = std::dynamic_pointer_cast<libsumo::TraCIResult>(list);
        const int time = CurrentTime::Now();
        storeTraCIResult(time, res, command, executionId);
    }
}

void Behaviour::processTraCIResult(std::shared_ptr<libsumo::TraCIPosition> position, const Command& command, const int executionId) {
    NS_LOG_INFO(m_controller->LogNode() << "iCSInferface::TraciCommandResult_ of " << command.objId << " is " << position->getString());

    if (command.type == GET_COMMAND) {
        std::shared_ptr<libsumo::TraCIResult> res = std::dynamic_pointer_cast<libsumo::TraCIResult>(position);
        const int time = CurrentTime::Now();
        storeTraCIResult(time, res, command, executionId);
    }
}

void Behaviour::processTraCIResult(std::shared_ptr<baseapp::TraCIPair2Int> result, const Command& command, const int executionId) {
    NS_LOG_INFO(m_controller->LogNode() << "iCSInferface::TraciCommandResult_ of " << command.objId << " is " << result->getString());

    if (command.type == GET_COMMAND) {
        std::shared_ptr<libsumo::TraCIResult> res = std::dynamic_pointer_cast<libsumo::TraCIResult>(result);
        const int time = CurrentTime::Now();
        storeTraCIResult(time, res, command, executionId);
    }
}

void Behaviour::processTraCIResult(std::shared_ptr<baseapp::TraCIVectorPair> result, const Command& command, const int executionId) {
    NS_LOG_INFO(m_controller->LogNode() << "iCSInferface::TraciCommandResult_ of " << command.objId << " is " << result->getString());

    if (command.type == GET_COMMAND) {
        std::shared_ptr<libsumo::TraCIResult> res = std::dynamic_pointer_cast<libsumo::TraCIResult>(result);
        const int time = CurrentTime::Now();
        storeTraCIResult(time, res, command, executionId);
    }
}

void Behaviour::storeTraCIResult(const int time, const std::shared_ptr<libsumo::TraCIResult> result, const Command& command, const int executionId) {
    if (TraCIResponses.find(command.objId) == end(TraCIResponses)) {
        TraCIResponses[command.objId] = std::map<int, std::pair <int, std::shared_ptr<libsumo::TraCIResult> > >();
    }
    TraCIResponses[command.objId][command.variableId] = std::make_pair(time, result);

    if (executionId != -1) {
        storeTraCIResultId(executionId, time, result, command);
    }
    if (command.variableId == libsumo::VAR_PARAMETER_WITH_KEY) {
        storeTraCIResultParameterKey(time, result, command);
    }
}

const std::pair<int, std::shared_ptr<libsumo::TraCIResult> >&
Behaviour::GetLastTraCIResponse(std::string objID, int variableID, std::string parameterKey) {
    if (parameterKey == INVALID_STRING) {
        auto objMapIt = TraCIResponses.find(objID);
        if (objMapIt != end(TraCIResponses)) {
            auto cmdMapIt = objMapIt->second.find(variableID);
            if (cmdMapIt != end(objMapIt->second)) {
                return cmdMapIt->second;
            }
        }
    } else {
        auto objMapIt = TraCIResponsesParameterKey.find(objID);
        if (objMapIt != end(TraCIResponsesParameterKey)) {
            auto keyMapIt = objMapIt->second.find(parameterKey);
            if (keyMapIt != end(objMapIt->second)) {
                return keyMapIt->second;
            }
        }
    }

    return noResponse;
}


void Behaviour::storeTraCIResultId(const int executionId, const int time, const std::shared_ptr<libsumo::TraCIResult> result, const Command& command) {
    if (TraCIResponsesId.find(executionId) == end(TraCIResponsesId)) {
        TraCIResponsesId[executionId] = std::pair <std::shared_ptr<CommandInfo>, std::shared_ptr<libsumo::TraCIResult> >();
    }
    std::shared_ptr<CommandInfo> info = std::make_shared<CommandInfo>();
    info->cmd = command;
    info->timeId = time;

    TraCIResponsesId[executionId] = std::make_pair(info, result);
}

void Behaviour::storeTraCIResultParameterKey(const int time, const std::shared_ptr<libsumo::TraCIResult> result, const Command& command) {
    if (TraCIResponsesParameterKey.find(command.objId) == end(TraCIResponsesParameterKey)) {
        TraCIResponsesParameterKey[command.objId] = std::map<std::string, std::pair <int, std::shared_ptr<libsumo::TraCIResult> > >();
    }

    std::string paramKey = std::dynamic_pointer_cast<baseapp::TraCIParameterWithKey>(result)->key;
    TraCIResponsesParameterKey[command.objId][paramKey] = std::make_pair(time, result);
}

const std::pair<std::shared_ptr<CommandInfo>, std::shared_ptr<libsumo::TraCIResult>>& Behaviour::getTraCIResponse(const int executionId) {
    auto objMapIt = TraCIResponsesId.find(executionId);

    //TODO erase first and then return copied value
    if (objMapIt != end(TraCIResponsesId)) {
        return objMapIt->second;
    }

    return noResponseId;
}


} /* namespace application */
} /* namespace baseapp */
