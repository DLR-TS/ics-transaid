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

#include <memory>
#include "ics-interface.h"
#include "utils/log/console.h"
#include "utils/xml/tinyxml2.h"
#include "fatal-error.h"
#include "behaviour-node.h"
#include "behaviour-rsu.h"
#include "node-sampler.h"
#include "output-helper.h"
#include "node.h"
#include "program-configuration.h"

using namespace std;

namespace baseapp {

using namespace tinyxml2;

std::unique_ptr<ProgramConfiguration> ProgramConfiguration::m_instance = nullptr;

int ProgramConfiguration::LoadConfiguration(const char* fileName, int port) {
    if (m_instance == nullptr) {
        m_instance = std::unique_ptr<ProgramConfiguration>(new ProgramConfiguration());
    }
    m_instance->SetSocketPort(port);
    XMLDocument* doc = new XMLDocument();
    XMLError result = doc->LoadFile(fileName);
    if (result != XML_NO_ERROR) {
        Console::Error("Failed to load xml file ", fileName);
        return EXIT_FAILURE;
    }
    XMLElement* xmlElem = doc->RootElement()->FirstChildElement("general");
    if (!xmlElem) {
        Console::Error("There isn't an element <general>");
        return EXIT_FAILURE;
    }
    if (m_instance->ParseGeneral(xmlElem) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    Console::Log("ParseGeneral done");
    xmlElem = doc->RootElement()->FirstChildElement("infrastructure");
    if (!xmlElem) {
        Console::Error("There isn't an element <infrastructure>");
        return EXIT_FAILURE;
    }
    if (m_instance->ParseInfrastructure(xmlElem) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    Console::Log("ParseInfrastructure done");
    xmlElem = doc->RootElement()->FirstChildElement("setup");
    if (xmlElem) {
        if (m_instance->ParseSetup(xmlElem) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    }
    Console::Log("ParseSetup done");
    xmlElem = doc->RootElement()->FirstChildElement("output");
    if (xmlElem) {
        if (m_instance->ParseOutput(xmlElem) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    }
    Console::Log("ParseOutput done");
    delete doc;
    return EXIT_SUCCESS;
}

ProgramConfiguration::ProgramConfiguration()
    : m_messageLifetime(10),
      m_sumoTotalDemandLevel(0),
      m_socket(-1),
      m_testCase(""),
      m_start(0) {}

ProgramConfiguration::~ProgramConfiguration() {}

void ProgramConfiguration::ParseLog(const XMLElement* element, const LogType type) {
    const char* clog = element->Attribute("value");
    if (clog != NULL) {
        m_logs.insert(make_pair(type, string(clog)));
    }
}

int ProgramConfiguration::ParseGeneral(XMLElement* general) {
    XMLElement* xmlElem = general->FirstChildElement("socket");
    if (!xmlElem && m_socket == -1) {
        Console::Error("Missing the socket tag <socket> inside <general>");
        return EXIT_FAILURE;
    } else {
        const int socket = xmlElem->IntAttribute("value");
        if (m_socket == -1) {
            m_socket = socket;
        } else if (m_socket > 0) {
            Console::Warning("Ignoring socket port specified in configuration. (Overridden by command line argument.)");
        }
    }
    NS_ASSERT(m_socket >= 0);
    NS_ASSERT(m_socket >= 1024);
    xmlElem = general->FirstChildElement("start");
    if (xmlElem) {
        m_start = xmlElem->IntAttribute("value") * 1000;
        NS_ASSERT(m_start >= 0);
    }

    xmlElem = general->FirstChildElement("log-file");
    if (xmlElem) {
        ParseLog(xmlElem, LOG_FILE);
    }
    xmlElem = general->FirstChildElement("data-file");
    if (xmlElem) {
        ParseLog(xmlElem, DATA_FILE);
    }
    xmlElem = general->FirstChildElement("ns-log-file");
    if (xmlElem) {
        ParseLog(xmlElem, NS_LOG_FILE);
    }

    xmlElem = general->FirstChildElement("message-lifetime");
    if (xmlElem) {
        unsigned tmp = xmlElem->UnsignedAttribute("value");
        if (tmp > 0) {
            m_messageLifetime = tmp;
        }
    }

    xmlElem = general->FirstChildElement("sumo-total-demand-level");
    if (xmlElem) {
        unsigned tmp = xmlElem->UnsignedAttribute("value");
        if (tmp > 0) {
            m_sumoTotalDemandLevel = tmp;
        }
    }

    xmlElem = general->FirstChildElement("random-run");
    if (xmlElem) {
        ns3::RngSeedManager::SetRun(xmlElem->IntAttribute("value"));
    }
    xmlElem = general->FirstChildElement("random-seed");
    if (xmlElem) {
        ns3::RngSeedManager::SetSeed(xmlElem->IntAttribute("value"));
    }
    xmlElem = general->FirstChildElement("test-case");
    if (xmlElem) {
        m_testCase = xmlElem->Attribute("value");
    }
    Console::Log("Test case set to " + m_testCase);

    return EXIT_SUCCESS;
}

Circle LoadCircle(const XMLElement* node) {
    Circle circle;
    circle.x = node->FloatAttribute("xpos");
    circle.y = node->FloatAttribute("ypos");
    circle.radius = node->FloatAttribute("radius");
    return circle;
}

int ProgramConfiguration::ParseInfrastructure(XMLElement* infrastructure) {
    XMLElement* xmlElem = infrastructure->FirstChildElement("rsu");
    if (!xmlElem) {
        Console::Error("There must be at least one <rsu> inside <infrastructure>");
        return EXIT_FAILURE;
    }
    while (xmlElem) {
        if (xmlElem->ToComment()) {
            xmlElem = xmlElem->NextSiblingElement();
            continue; // skip comment nodes
        }
        RsuData rsu;
        // RSU id
        rsu.id = xmlElem->IntAttribute("id");
        NS_ASSERT(rsu.id > 0);
        // RSU position
        XMLError result = xmlElem->QueryDoubleAttribute("xpos", &rsu.position.x);
        if (result != XML_NO_ERROR) {
            Console::Error("Error parsing attribute xpos of <rsu>");
            return EXIT_FAILURE;
        }
        result = xmlElem->QueryDoubleAttribute("ypos", &rsu.position.y);
        if (result != XML_NO_ERROR) {
            Console::Error("Error parsing attribute ypos of <rsu>");
            return EXIT_FAILURE;
        }
        XMLElement* circle = xmlElem->FirstChildElement("cam-area");
        if (circle) {
            rsu.cam_area = LoadCircle(circle);
        }
        circle = xmlElem->FirstChildElement("car-area");
        if (circle) {
            rsu.car_area = LoadCircle(circle);
        }
        // RSU directions
        XMLElement* rsudir = xmlElem->FirstChildElement("direction");
        while (rsudir) {
            double dir;
            bool bVal;
            int iVal;
            if (rsudir->ToComment()) {
                rsudir = rsudir->NextSiblingElement("direction");
                continue; // skip comment nodes
            }
            if (rsudir->QueryDoubleAttribute("value", &dir) != XML_NO_ERROR) {
                Console::Error("Error parsing direction inside <rsu>");
                return EXIT_FAILURE;
            }
            Direction dirStruct;
            dirStruct.direction = dir;
            if (rsudir->QueryBoolAttribute("approaching", &bVal) == XML_NO_ERROR) {
                dirStruct.approaching = bVal;
            }
            if (rsudir->QueryBoolAttribute("leaving", &bVal) == XML_NO_ERROR) {
                dirStruct.leaving = bVal;
            }
            if (rsudir->QueryIntAttribute("approaching-time", &iVal) == XML_NO_ERROR)
                if (iVal >= 0 && iVal <= 65535) {
                    dirStruct.approachingTime = iVal;
                }
            if (rsudir->QueryIntAttribute("leaving-time", &iVal) == XML_NO_ERROR)
                if (iVal >= 0 && iVal <= 65535) {
                    dirStruct.leavingTime = iVal;
                }
            rsu.directions.push_back(dirStruct);
            // RSU direction lanes
            XMLElement* lane = rsudir->FirstChildElement("lane");
            while (lane) {
                if (lane->ToComment()) {
                    lane = lane->NextSiblingElement("lane");
                    continue; // skip comment nodes
                }
                TLLane tllane;
                tllane.dir = dir;
                const char* cstr = lane->Attribute("friendly-name");
                if (cstr != NULL) {
                    tllane.friendlyName = std::string(cstr);
                }
                cstr = lane->Attribute("controlled-lane");
                if (cstr != NULL) {
                    tllane.controlledLane = std::string(cstr);
                }
                cstr = lane->Attribute("following-lane");
                if (cstr != NULL) {
                    tllane.followingLane = std::string(cstr);
                }

                rsu.lanes.push_back(tllane);

                lane = lane->NextSiblingElement("lane");
            }
            rsudir = rsudir->NextSiblingElement("direction");
        }

        m_rsus.insert(make_pair(rsu.id, rsu));
        xmlElem = xmlElem->NextSiblingElement("rsu");
    }
    return EXIT_SUCCESS;
}

bool ProgramConfiguration::IsRsu(const int id) {
    return m_instance->m_rsus.count(id) > 0;
}

const RsuData& ProgramConfiguration::GetRsuData(const int id) {
    std::map<int, RsuData>::const_iterator it = m_instance->m_rsus.find(id);
    return it->second;
}

bool ProgramConfiguration::GetLogFileName(LogType type, std::string& fileName) {
    std::map<LogType, std::string>::const_iterator it = m_instance->m_logs.find(type);
    if (it != m_instance->m_logs.end()) {
        fileName = it->second;
        return true;
    }
    return false;
}

int ProgramConfiguration::ParseSetup(XMLElement* setup) {
    using namespace application;
    int iVal;
    double dVal;
    bool bVal;
    XMLElement* xmlElem = setup->FirstChildElement("node");
    if (xmlElem) {
        if (xmlElem->QueryDoubleAttribute("probability-full", &dVal) == XML_NO_ERROR) {
            Node::ProbabilityFull = dVal;
        }
        if (xmlElem->QueryDoubleAttribute("probability-medium", &dVal) == XML_NO_ERROR) {
            Node::ProbabilityMedium = dVal;
        }
        if (xmlElem->QueryDoubleAttribute("propagation-radius-rsu", &dVal) == XML_NO_ERROR) {
            Node::PropagationRagiusRsu = dVal;
        }
        if (xmlElem->QueryDoubleAttribute("propagation-radius-full", &dVal) == XML_NO_ERROR) {
            Node::PropagationRagiusFull = dVal;
        }
        if (xmlElem->QueryDoubleAttribute("propagation-radius-medium", &dVal) == XML_NO_ERROR) {
            Node::PropagationRagiusMedium = dVal;
        }
    }
    xmlElem = setup->FirstChildElement("controller");
    if (xmlElem) {
        if (xmlElem->QueryDoubleAttribute("direction-tolerance", &dVal) == XML_NO_ERROR) {
            iCSInterface::DirectionTolerance = dVal;
        }
        if (xmlElem->QueryIntAttribute("average-speed-sample-small", &iVal) == XML_NO_ERROR)
            if (iVal >= 0 && iVal <= 65535) {
                iCSInterface::AverageSpeedSampleSmall = iVal;
            }
        if (xmlElem->QueryIntAttribute("average-speed-sample-high", &iVal) == XML_NO_ERROR)
            if (iVal >= 0 && iVal <= 65535) {
                iCSInterface::AverageSpeedSampleHigh = iVal;
            }
        if (xmlElem->QueryBoolAttribute("use-sink", &bVal) == XML_NO_ERROR) {
            iCSInterface::UseSink = bVal;
        }
    }
    xmlElem = setup->FirstChildElement("node-sampler");
    if (xmlElem) {
        ParseNodeSampler(xmlElem);
    }
    xmlElem = setup->FirstChildElement("behaviour-node");
    if (xmlElem) {
        if (xmlElem->QueryBoolAttribute("enabled", &bVal) == XML_NO_ERROR) {
            BehaviourNode::Enabled = bVal;
        }
        if (xmlElem->QueryIntAttribute("response-time-spacing", &iVal) == XML_NO_ERROR)
            if (iVal >= 0 && iVal <= 65535) {
                Behaviour::DefaultResponseTimeSpacing = iVal;
            }
        if (xmlElem->QueryDoubleAttribute("sink-threshold", &dVal) == XML_NO_ERROR) {
            BehaviourNode::SinkThreshold = dVal;
        }
    }
    xmlElem = setup->FirstChildElement("behaviour-rsu");
    if (xmlElem) {
        if (xmlElem->QueryBoolAttribute("enabled", &bVal) == XML_NO_ERROR) {
            BehaviourRsu::Enabled = bVal;
        }
        if (xmlElem->QueryIntAttribute("time-beacon", &iVal) == XML_NO_ERROR)
            if (iVal >= 0 && iVal <= 65535) {
                BehaviourRsu::TimeBeacon = iVal;
            }
        if (xmlElem->QueryIntAttribute("time-beacon-min", &iVal) == XML_NO_ERROR)
            if (iVal >= 0 && iVal <= 65535) {
                BehaviourRsu::TimeBeaconMin = iVal;
            }
        if (xmlElem->QueryIntAttribute("time-check", &iVal) == XML_NO_ERROR)
            if (iVal >= 0 && iVal <= 65535) {
                BehaviourRsu::TimeCheck = iVal;
            }
        if (xmlElem->QueryIntAttribute("timeout", &iVal) == XML_NO_ERROR)
            if (iVal >= 0 && iVal <= 65535) {
                BehaviourRsu::Timeout = iVal;
            }
    }
    return EXIT_SUCCESS;
}

int ProgramConfiguration::ParseOutput(tinyxml2::XMLElement* output) {
    using namespace application;
    int iVal;
    double dVal;
    bool bVal;
    if (output->QueryIntAttribute("sink-threshold-min", &iVal) == XML_NO_ERROR) {
        OutputHelper::SinkDistanceThresholdMin = iVal;
    }
    if (output->QueryIntAttribute("sink-threshold-max", &iVal) == XML_NO_ERROR) {
        OutputHelper::SinkDistanceThresholdMax = iVal;
    }
    if (output->QueryDoubleAttribute("sink-tolerance", &dVal) == XML_NO_ERROR) {
        OutputHelper::SinkOrientationTolerance = dVal;
    }
    if (output->QueryIntAttribute("sample-interval", &iVal) == XML_NO_ERROR) {
        OutputHelper::SampleInterval = iVal;
    }
    if (output->QueryBoolAttribute("sample-packets", &bVal) == XML_NO_ERROR) {
        OutputHelper::SamplePackets = bVal;
    }
    return EXIT_SUCCESS;
}

int ProgramConfiguration::ParseNodeSampler(tinyxml2::XMLElement* nodeSampler) {
    using namespace application;
    int iVal;
    double dVal;
    if (nodeSampler->QueryIntAttribute("quantity", &iVal) == XML_NO_ERROR)
        if (iVal >= 0 && iVal <= 65535) {
            NodeSampler::Quantity = iVal;
        }
    if (nodeSampler->QueryDoubleAttribute("movement-threshold", &dVal) == XML_NO_ERROR) {
        NodeSampler::MovementThreshold = dVal;
    }
    if (nodeSampler->QueryDoubleAttribute("steer-filter-threshold", &dVal) == XML_NO_ERROR) {
        NodeSampler::SteerFilterThreshold = dVal;
    }
    if (nodeSampler->QueryIntAttribute("steer-filter-count", &iVal) == XML_NO_ERROR)
        if (iVal >= 0 && iVal <= 255) {
            NodeSampler::SteerFilterCount = iVal;
        }
    if (nodeSampler->QueryDoubleAttribute("speed-bound", &dVal) == XML_NO_ERROR) {
        NodeSampler::SpeedBound = dVal;
    }
    if (nodeSampler->QueryDoubleAttribute("speed-variance", &dVal) == XML_NO_ERROR) {
        NodeSampler::SpeedVariance = dVal;
    }
    //Default attributes
    if (nodeSampler->QueryIntAttribute("default-resolution", &iVal) == XML_NO_ERROR)
        if (iVal >= 0 && iVal <= 65535) {
            NodeSampler::DefaultResolution = iVal;
        }
    if (nodeSampler->QueryDoubleAttribute("default-position-radius", &dVal) == XML_NO_ERROR) {
        NodeSampler::DefaultPositionRadius = dVal;
    }
    if (nodeSampler->QueryDoubleAttribute("default-position-variance", &dVal) == XML_NO_ERROR) {
        NodeSampler::DefaultPositionVariance = dVal;
    }
    if (nodeSampler->QueryDoubleAttribute("default-direction-variance", &dVal) == XML_NO_ERROR) {
        NodeSampler::DefaultDirectionVariance = dVal;
    }
    if (nodeSampler->QueryDoubleAttribute("default-speed-error", &dVal) == XML_NO_ERROR) {
        NodeSampler::DefaultSpeedError = dVal;
    }
    XMLElement* nodeClass = nodeSampler->FirstChildElement("node-class");
    while (nodeClass) {
        const char* type = nodeClass->Attribute("type");
        if (type == NULL) {
            Console::Warning("No type attribute in a node-class tag. It will be skipped.");
            continue;
        }
        NodeTypeSamplerAttributes classAttribute;
        if (!NodeSampler::GetNodeTypeFromString(type, classAttribute.nodeType)) {
            ostringstream oss;
            oss << "Type " << type << " is unknown. It will be skipped.";
            Console::Warning(oss.str());
            continue;
        }
        if (nodeClass->QueryIntAttribute("resolution", &iVal) == XML_NO_ERROR)
            if (iVal >= 0 && iVal <= 65535) {
                classAttribute.resolution = iVal;
            }
        if (nodeClass->QueryDoubleAttribute("position-radius", &dVal) == XML_NO_ERROR) {
            classAttribute.positionRadius = dVal;
        }
        if (nodeClass->QueryDoubleAttribute("position-variance", &dVal) == XML_NO_ERROR) {
            classAttribute.positionVariance = dVal;
        }
        if (nodeClass->QueryDoubleAttribute("direction-variance", &dVal) == XML_NO_ERROR) {
            classAttribute.directionVariance = dVal;
        }
        if (nodeClass->QueryDoubleAttribute("speed-error", &dVal) == XML_NO_ERROR) {
            classAttribute.speedError = dVal;
        }
        NodeSampler::AddNodeTypeSamplerAttributes(classAttribute);
        nodeClass = nodeClass->NextSiblingElement("node-class");
    }
    return EXIT_SUCCESS;
}

} /* namespace protocol */
