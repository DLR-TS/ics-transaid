/****************************************************************************************
 * Copyright (c) 2015 The Regents of the University of Bologna.
 * This code has been developed in the context of the
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software must display
 * the following acknowledgement: ''This product includes software developed by the
 * University of Bologna and its contributors''.
 * 4. Neither the name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************************/
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/

#include "log/console.h"
#include "ics-interface.h"
#include "behaviour-test-node.h"
#include "program-configuration.h"
#include "node.h"
#include "../../app-commands-subscriptions-constants.h"
#include "current-time.h"
#include "libsumo/TraCIDefs.h"
#include <cmath>
#include <memory>
#include "message-scheduler-helper.h"

using namespace baseapp;
using namespace baseapp::application;

namespace testapp
{
	namespace application
	{
		///BehaviourTestNode implementation
		BehaviourTestNode::BehaviourTestNode(iCSInterface* controller) :
				BehaviourNode(controller)
		{
            m_waitForRSUAcknowledgement = true;
            m_vehicleStopScheduled = false;
            m_firstBroadcast = true;
            m_eventAbortWaitingForRSU = 0;
            m_eventBroadcast = 0;
            m_broadcastInterval = 1000;
            m_setCAMareaSubscription=false;
            m_subReceiveMessage = false;
            m_eventAbortBroadcast = 0;
            m_broadcastActive = true;
            m_eventBroadcastCAM = 0;
            m_broadcastCheckInterval = 100;
            m_eventBroadcastMCM = 0;
            m_eventBroadcastCPM = 0;
		}

        BehaviourTestNode::~BehaviourTestNode() {
            Scheduler::Cancel(m_eventAbortWaitingForRSU);
            Scheduler::Cancel(m_eventBroadcast);
            Scheduler::Cancel(m_eventAbortBroadcast);
            Scheduler::Cancel(m_eventBroadcastCAM);
            Scheduler::Cancel(m_eventBroadcastCPM);
            Scheduler::Cancel(m_eventBroadcastMCM);
			if (ProgramConfiguration::GetTestCase() == "testMessageScheduler"){
				delete m_MessageScheduler;
			}
        }

		void BehaviourTestNode::Start()
		{
			if (!m_enabled)
				return;
			BehaviourNode::Start();

            //Example use of a traci command subscription
            if (ProgramConfiguration::GetTestCase()=="simpleExecute") {
                // do nothing
            } else if (ProgramConfiguration::GetTestCase()=="setVType") {
                GetController()->AddTraciSubscription(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::VAR_TYPE);
                tcpip::Storage type;
                type.writeString("t2");
                GetController()->AddTraciSubscription(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::VAR_TYPE, libsumo::TYPE_STRING, &type);
            } else if (ProgramConfiguration::GetTestCase() == "commSimple") {
                // Time to abort waiting for a response after insertion.
                // Will be ineffective if communication runs as intended (test case commSimple2).
                // Used for testing purposes before random offsets were assigned to messages. (test case commSimple)
                // (=> abort at 12000, as the test vehicle is inserted at t=2000)
                if (!m_subReceiveMessage) {
                    GetController()->startReceivingUnicast();
                    GetController()->startReceivingGeobroadcast(MSGCAT_TESTAPP);
                    m_subReceiveMessage = true;
                }
                const int endWaitingTime = 10000;
                m_eventAbortWaitingForRSU = Scheduler::Schedule(endWaitingTime, &BehaviourTestNode::abortWaitingForRSUResponse, this);
            } else if (ProgramConfiguration::GetTestCase() == "commSimple2") {
                // After t=8000, vehicle starts broadcasting until its broadcast is acknowledged or aborted at t = 12000
                const int endWaitingTime = 10000;
                const int insertionAccordingToRoutresFile = 2000;
                if (!m_subReceiveMessage) {
                    GetController()->startReceivingUnicast();
                    GetController()->startReceivingGeobroadcast(MSGCAT_TESTAPP);
                    m_subReceiveMessage = true;
                }
                m_eventAbortWaitingForRSU = Scheduler::Schedule(endWaitingTime, &BehaviourTestNode::abortWaitingForRSUResponse, this);
                NS_LOG_INFO(Log() << "Vehicle scheduled abort waiting for RSU acknowledgement at " << endWaitingTime + insertionAccordingToRoutresFile);
            } else if (ProgramConfiguration::GetTestCase() == "CAMsimple"){
                if (!m_setCAMareaSubscription)
                {
                    GetController()->StartSendingCAMs();
                    GetController()->startReceivingCAMs();
                    m_setCAMareaSubscription = true;
                }
            } else if (ProgramConfiguration::GetTestCase() == "acosta" || ProgramConfiguration::GetTestCase() == "") {
                if (!m_subReceiveMessage)
                {
                    //Subscribe to geobraodcast and unicast
                    GetController()->requestMobilityInfo();
                    GetController()->startReceivingUnicast();
                    GetController()->startReceivingGeobroadcast(PROTOCOL_MESSAGE);
                    m_subReceiveMessage = true;
                }
            } else if (ProgramConfiguration::GetTestCase() == "getSUMOStepLength") {
                NS_LOG_INFO(Log() << "SUMO step length is " << GetController()->getSUMOStepLength() << " milliseconds");
            } else if (ProgramConfiguration::GetTestCase() == "testV2XmsgSet"){

                GetController()->startReceivingGeobroadcast(MSGCAT_TESTAPP);
                m_subReceiveMessage = true;

                m_eventBroadcast = Scheduler::Schedule(m_broadcastCheckInterval, &BehaviourTestNode::VehicleBroadcastTestV2XmsgSet, this);

			} else if (ProgramConfiguration::GetTestCase() == "testMessageScheduler"){

				GetController()->startReceivingGeobroadcast(MSGCAT_TRANSAID);
				m_subReceiveMessage = true;

				std::cout << "Starting test Message Scheduler at Vehicle"  << std::endl;


				m_MessageScheduler = new MessageScheduler( GetController());


			}
		}


        void BehaviourTestNode::OnAddSubscriptions() {
            if (ProgramConfiguration::GetTestCase() == "drivingDistance") {
                if (CurrentTime::Now() == 5000) {
                    GetController()->commandTraciGetDrivingDistance("CE", 50);
                }
            }
        }

		bool BehaviourTestNode::IsSubscribedTo(ProtocolId pid) const
		{
			if (ProgramConfiguration::GetTestCase() == "testMessageScheduler"){
				return pid == PID_TRANSAID;
			} else {
				return pid == PID_UNKNOWN;
			}
		}

		void BehaviourTestNode::Receive(server::Payload *payload, double snr)
		{
            NS_LOG_FUNCTION(Log());
            if (!m_enabled)
                return;
            CommHeader* commHeader;
            GetController()->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);

            if (ProgramConfiguration::GetTestCase() == "testV2XmsgSet" || ProgramConfiguration::GetTestCase() == "testMessageScheduler"){

            	TransaidHeader* transaidHeader;
				GetController()->GetHeader(payload, server::PAYLOAD_END, transaidHeader);
				Header * receivedHeader = payload->getHeader(baseapp::server::PAYLOAD_END);
				TransaidHeader* receivedTestHeader = dynamic_cast<TransaidHeader*>(receivedHeader);

				NS_LOG_INFO(Log() << "Received a test message from node " << commHeader->getSourceId() << " with message type: " << transaidHeader->getMessageType() );

				if (commHeader->getMessageType() == TRANSAID_CAM)
				{
				    const TransaidHeader::CamInfo * camInfo = transaidHeader->getCamInfo();

					std::cout << "Received CAM at node " << GetController()->GetId() << "  sender " << camInfo->senderID << " time " << camInfo->generationTime <<  " position " << camInfo->position <<
							 " speed " << camInfo->speed  <<  " acceleration " << camInfo->acceleration << " Message size " << receivedTestHeader->getMessageRealSize()  << std::endl;
					return;
				}

				if (commHeader->getMessageType() == TRANSAID_DENM)
				{
				    const TransaidHeader::DenmInfo * denmInfo = transaidHeader->getDenmInfo();

					std::cout << "Received DENM at node " << GetController()->GetId() << "  sender " << denmInfo->senderID << " type " << denmInfo->denmType
					    << " time " << denmInfo->generationTime << " Message size " << receivedTestHeader->getMessageRealSize() << std::endl;

					return;
				}

				if (commHeader->getMessageType() == TRANSAID_CPM)
				{
				    const TransaidHeader::CpmInfo * cpmInfo = transaidHeader->getCpmInfo();

					std::cout << "Received CPM at node " << GetController()->GetId() << "  sender " << cpmInfo->senderID << " time " << cpmInfo->generationTime << " Number OF Received Obstacles in the CPM " << cpmInfo->numObstacles << " CPM Message Size " << cpmInfo->CPM_message_size << std::endl;
					/*
					for(int i=0; i < cpmInfo->numObstacles ; ++i )
					{
						std::cout << "Received Object ID : " << cpmInfo->CPM_detected_objectID[i] <<std::endl;
					}
					 */
					return;
				}

				if (commHeader->getMessageType() == TRANSAID_MCM_VEHICLE)
				{
				    const TransaidHeader::McmVehicleInfo * mcmInfo = transaidHeader->getMcmVehicleInfo();

					std::cout << "Received MCM from a vehicle at node " << GetController()->GetId() << "  sender " << mcmInfo->senderID
					                << " time " << mcmInfo->generationTime  << " Message size " << receivedTestHeader->getMessageRealSize() << std::endl;

					return;
				}

				if (commHeader->getMessageType() == TRANSAID_MCM_RSU)
				{
				    const TransaidHeader::McmRsuInfo * mcmInfo = transaidHeader->getMcmRsuInfo();

					std::cout << "Received MCM from a RSU at node " << GetController()->GetId() << "  sender " << mcmInfo->senderID << " time " << mcmInfo->generationTime  << std::endl;

					std::shared_ptr<TransaidHeader::AdviceInfo> adviceInfo = mcmInfo->adviceInfo;;
					std::shared_ptr<TransaidHeader::ToCAdvice> tocAdvice = std::dynamic_pointer_cast<TransaidHeader::ToCAdvice>(adviceInfo->advice);

					std::cout << "Received MCM from a RSU with a ToC advice at time " << tocAdvice->tocTime << " and start position " << tocAdvice->tocStartPosition <<
							" and end position " << tocAdvice->tocEndPosition << " Message size " << receivedTestHeader->getMessageRealSize() << std::endl;


					return;
				}

				if (commHeader->getMessageType() == TRANSAID_MAP)
				{
					const TransaidHeader::MapInfo * mapInfo = transaidHeader->getMapInfo();

					std::cout << "Received MAP at node " << GetController()->GetId() << "  sender " << mapInfo->senderID << " time " << mapInfo->generationTime
                            << " Message size " << receivedTestHeader->getMessageRealSize() << std::endl;

					return;
				}

				if (commHeader->getMessageType() == TRANSAID_IVI)
				{
					const TransaidHeader::IviInfo * iviInfo = transaidHeader->getIviInfo();

					std::cout << "Received IVI at node " << GetController()->GetId() << "  sender " << iviInfo->senderID << " time " << iviInfo->generationTime
                            << " Message size " << receivedTestHeader->getMessageRealSize() << std::endl;

					return;
				}

            } else{

				if (commHeader->getMessageType() != MT_RSU_TEST)
				{
					NS_LOG_WARN(Log()<< "Received an unknown message "<< commHeader->getMessageType());
					return;
				}
				NodeInfo rsu;
				rsu.nodeId = commHeader->getSourceId();
				rsu.position = commHeader->getSourcePosition();


				TestHeader* testHeader;
				GetController()->GetHeader(payload, server::PAYLOAD_END, testHeader);
				Header * receivedHeader = payload->getHeader(baseapp::server::PAYLOAD_END);
				TestHeader* receivedTestHeader = dynamic_cast<TestHeader*>(receivedHeader);

				NS_LOG_INFO(Log() << "Received a test message with content: " << testHeader->getMessage());

				if (ProgramConfiguration::GetTestCase() == "commSimple") {
					if (m_waitForRSUAcknowledgement && receivedTestHeader->getMessage() == "RSU Vehicle acknowledgement") {
						Scheduler::Cancel(m_eventAbortWaitingForRSU);
						abortWaitingForRSUResponse();
						NS_LOG_DEBUG(Log() << "On reception of RSU Vehicle acknowledgement.");
					}
				} else if (ProgramConfiguration::GetTestCase() == "commSimple2") {
					// Random offset for responseTime
					double responseTime = m_rnd.GetValue(m_responseTimeSpacing, testHeader->getMaxResponseTime() - m_responseTimeSpacing);
					TestHeader::ResponseInfo response;
					response.targetID = commHeader->getSourceId();
					if (m_waitForRSUAcknowledgement && receivedTestHeader->getMessage() == "RSU regular broadcast message") {
	//                    NS_LOG_DEBUG(Log() << "Vehicle " << GetController()->GetNode()->getId() << "  and responds.");
						NS_LOG_DEBUG(Log() << "Vehicle " << GetController()->GetNode()->getId() << "  and responds.");
						response.message = "Vehicle response to RSU broadcast";
						Scheduler::Cancel(m_eventResponse);
						m_eventResponse = Scheduler::Schedule(responseTime, &BehaviourTestNode::EventSendResponse, this, response);
					} else if (m_waitForRSUAcknowledgement && receivedTestHeader->getMessage() == "RSU Vehicle acknowledgement") {
						Scheduler::Cancel(m_eventAbortWaitingForRSU);
						abortWaitingForRSUResponse();
						NS_LOG_DEBUG(Log() << "On reception of RSU Vehicle acknowledgement.");
						// Send response
						response.message = "Vehicle response to RSU Vehicle acknowledgement";
						Scheduler::Cancel(m_eventResponse);
						m_eventResponse = Scheduler::Schedule(responseTime, &BehaviourTestNode::EventSendResponse, this, response);
						NS_LOG_INFO(Log() << "Scheduled a test response in " << responseTime);
					} else if (receivedTestHeader->getMessage() == "RSU Stop advice" && !m_vehicleStopScheduled) {
						NS_LOG_DEBUG(Log() << "On reception of RSU Stop advice.");
						std::string stopEdge = receivedTestHeader->getStopEdge();
						double stopPosition = receivedTestHeader->getStopPosition();
						GetController()->AddTraciStop(stopEdge, stopPosition, 0, 3.);
						m_vehicleStopScheduled = true;
						NS_LOG_INFO(Log() << "Added a stop on edge " << stopEdge << " at position" << stopPosition);
					}
				}
            }
		}

		bool BehaviourTestNode::Execute(DirectionValueMap &data)
		{

            if (ProgramConfiguration::GetTestCase() == "drivingDistance") {
                if (CurrentTime::Now() == 5000) {
                    std::string sumoID = GetController()->GetNode()->getSumoId();
                    auto distResponse = std::dynamic_pointer_cast<libsumo::TraCIDouble>(GetLastTraCIResponse(sumoID, libsumo::DISTANCE_REQUEST).second);
                    if (distResponse != nullptr) {
                        double dist = distResponse->value;
                        NS_LOG_INFO(Log() << "Driving distance for veh " << sumoID << " at time " << CurrentTime::Now() << " is " << dist);
                    } else {
                        NS_LOG_INFO(Log() << "No driving distance result for veh " << sumoID);
                    }
                }
            } else if (ProgramConfiguration::GetTestCase() == "setVType") {
                if (CurrentTime::Now() == 10000) {
                    // check vType at time 10.
                    GetController()->AddTraciSubscription(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::VAR_TYPE);
                }
            } else if (ProgramConfiguration::GetTestCase() == "inductionLoop") {
                // Vehicle does nothing
						} else if (ProgramConfiguration::GetTestCase() == "testTrajectory") {
							  //QUESTION: This seems to work only on specifi values. Why ?
							  if (CurrentTime::Now() % 1000 == 0.0) {
								  	// retrieve speed every 10[sec].
									  GetController()->AddTraciSubscription(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::VAR_SPEED);
										// retrieve lane id every 10[sec].
									  GetController()->AddTraciSubscription(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::VAR_LANE_ID);
										// retrieve 1D position every lane at 10[sec].
									  GetController()->AddTraciSubscription(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::VAR_LANEPOSITION);
							  }
						} else if (ProgramConfiguration::GetTestCase() == "testToC") {
							  // TODO: instead of time, trigger ToC via lane ID and position
						  	if (CurrentTime::Now() == 10000 ) {
										// Requesting ToC at 10[sec].
										GetController()->requestToC("veh0",4.0);
								}
						} else if (ProgramConfiguration::GetTestCase() == "testMobility") {
								if (CurrentTime::Now() == 12000 ) {
										// Requesting Mobility Info at 12[sec].
										GetController()->requestMobilityInfo();
								}
            } else if (ProgramConfiguration::GetTestCase() == "commSimple") {
                // After t=8000, vehicle starts broadcasting until its broadcast is acknowledged or aborted at t = 12000
                if (CurrentTime::Now() > 8000 && m_waitForRSUAcknowledgement){
                    TestHeader * header = new TestHeader(PID_UNKNOWN, MT_TEST_RESPONSE, "Vehicle regular broadcast");
                    GetController()->SendTo(5000, header, PID_UNKNOWN, MSGCAT_TESTAPP);
                }
            } else if (ProgramConfiguration::GetTestCase() == "testOpenGap") {
                if (CurrentTime::Now() == 8000 && GetController()->GetNode()->getSumoId() == "veh0") {
                    GetController()->commandTraciOpenGap(5, 50, 100, 0.1, 1);
                }
            }
			return false;
		}


        void BehaviourTestNode::EventSendResponse(TestHeader::ResponseInfo response)
        {
            NS_LOG_FUNCTION(Log());

            // React to perception of vehicle.
            TestHeader * responseHeader = new TestHeader(PID_UNKNOWN, MT_TEST_RESPONSE, response);
            GetController()->SendTo(response.targetID, responseHeader , PID_UNKNOWN, MSGCAT_TESTAPP);

            NS_LOG_DEBUG(Log() << "Sent test response to RSU " << response.targetID);
        }


        void BehaviourTestNode::abortWaitingForRSUResponse()
        {
            NS_LOG_FUNCTION(Log());
            m_waitForRSUAcknowledgement = false;
            NS_LOG_DEBUG(Log() << "Aborted waiting for RSU response");
        }


        void BehaviourTestNode::VehicleBroadcastTestV2XmsgSet()
        {
            if (m_firstBroadcast) {
                NS_LOG_FUNCTION(Log());
                m_firstBroadcast = false;
                NS_LOG_DEBUG(Log() << "Starting vehicle broadcast");
            }

            if (!m_broadcastActive)
                return;

            //std::cout << "Check the transmission of messages at node  " << GetController()->GetId() << " at time " << CurrentTime::Now() <<  std::endl;

			// Check CAM Tx

            double distanceBTcams = CalculateDistance(m_lastCAMsent.position, GetController()->GetNode()->getPosition());
            double speedBTcams = fabs(m_lastCAMsent.speed - GetController()->GetNode()->getSpeed() );
            double headingBTcams = fabs(m_lastCAMsent.heading - GetController()->GetNode()->getDirection() );

            std::cout << "Check the transmission of messages at node  " << GetController()->GetId()  << " at time diff " << (CurrentTime::Now() - m_lastCAMsent.generationTime) <<
            		" with position difference " << distanceBTcams << " with speed difference  " << speedBTcams <<  " with heading difference " <<  headingBTcams << std::endl;

			if ( (CurrentTime::Now() - m_lastCAMsent.generationTime)>1000 || (distanceBTcams > 4) || (speedBTcams > 0.5) ) //TODO add heading condition once it is clear that direction is the heading
			{

				double nextTime = m_rnd.GetValue(0, 50); // introduce random value to avoid collisions
				m_eventBroadcastCAM = Scheduler::Schedule(nextTime, &BehaviourTestNode::SendCAM, this);
			}

			if ( (CurrentTime::Now() - m_lastCPMsent.generationTime)>1000  )
			{

				double nextTime = m_rnd.GetValue(0, 50); // introduce random value to avoid collisions
				m_eventBroadcastCPM = Scheduler::Schedule(nextTime, &BehaviourTestNode::SendCPM, this);
			}

			if ( (CurrentTime::Now() - m_lastMCMsent.generationTime)>1000 )
			{

				double nextTime = m_rnd.GetValue(0, 50); // introduce random value to avoid collisions
				m_eventBroadcastMCM = Scheduler::Schedule(nextTime, &BehaviourTestNode::SendMCM, this);
			}



			// Schedule next broadcast for check transmission of messages
			m_eventBroadcast = Scheduler::Schedule(m_broadcastCheckInterval, &BehaviourTestNode::VehicleBroadcastTestV2XmsgSet, this);

        }

        void BehaviourTestNode::SendCAM()
        {

            const Node * veh = GetController()->GetNode();
            // TODO: Check if direction and heading equivalent!!
            TransaidHeader::CamInfo * message = new TransaidHeader::CamInfo(GetController()->GetId(), CurrentTime::Now(), veh->getLaneIndex(),
                    veh->getPosition(), veh->getSpeed(), veh->getAcceleration(), veh->getDirection());

			m_lastCAMsent = *message;

			TransaidHeader * header = new TransaidHeader(PID_UNKNOWN, TRANSAID_CAM, message,100);
			GetController()->Send(NT_ALL, header, PID_UNKNOWN, MSGCAT_TESTAPP);

            std::cout << "Send CAM at node " << GetController()->GetId() <<  std::endl;
        }

        void BehaviourTestNode::SendCPM()
        {

            TransaidHeader::CpmInfo * message = new TransaidHeader::CpmInfo();
			message->generationTime = CurrentTime::Now();
			message->senderID = GetController()->GetId();
			message->numObstacles = 1;

			m_lastCPMsent = *message;

			TransaidHeader * header = new TransaidHeader(PID_UNKNOWN, TRANSAID_CPM, message,100);
			GetController()->Send(NT_ALL, header, PID_UNKNOWN, MSGCAT_TESTAPP);

            //std::cout << "Send CPM at node " << GetController()->GetId() <<  std::endl;
        }


        void BehaviourTestNode::SendMCM()
        {

            TransaidHeader::McmVehicleInfo * message = new TransaidHeader::McmVehicleInfo();
			message->generationTime = CurrentTime::Now();
			message->senderID = GetController()->GetId();


			m_lastMCMsent = *message;

			TransaidHeader * header = new TransaidHeader(PID_UNKNOWN,   TRANSAID_MCM_VEHICLE, message,100);
			GetController()->Send(NT_ALL, header, PID_UNKNOWN, MSGCAT_TESTAPP);

            //std::cout << "Send MCM at node " << GetController()->GetId() <<  std::endl;
        }


        void BehaviourTestNode::abortBroadcast()
        {
            NS_LOG_FUNCTION(Log());
            m_broadcastActive = false;
            Scheduler::Cancel(m_eventBroadcast);
            NS_LOG_DEBUG(Log() << "Vehicle aborted broadcasting");
        }


        void BehaviourTestNode::processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages)
        {
            NS_LOG_FUNCTION(Log());
            if (ProgramConfiguration::GetTestCase() == "CAMsimple") {
                NS_LOG_DEBUG(Log() << "Node " << nodeID <<   " received CAM messages");
                for (std::vector<CAMdata>::const_iterator it = receivedCAMmessages.begin(); it != receivedCAMmessages.end(); ++it)
                {
                    NS_LOG_INFO(Log() << "CAM message received from node " << it->senderID << " at time " << it->generationTime);
                }
            }
        }


	} /* namespace application */
} /* namespace protocol */
