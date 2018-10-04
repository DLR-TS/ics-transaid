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

#include "behaviour.h"

#include "ics-interface.h"
#include "log/log.h"
#include "program-configuration.h"

namespace testapp
{
	namespace application
	{

		Behaviour::Behaviour(iCSInterface* controller) :
				m_running(false)
		{
			m_controller = controller;
			RegisterTrace("StartToggle", m_traceStartToggle);
		}

		Behaviour::~Behaviour()
		{
			//do not delete m_controller here
		}

		bool Behaviour::IsActiveOnStart() const
		{
			return true;
		}

		bool Behaviour::IsRunning() const
		{
			return m_running;
		}

		iCSInterface* Behaviour::GetController() const
		{
			return m_controller;
		}

		void Behaviour::Start()
		{
			NS_LOG_FUNCTION(Log());
			if (m_running)
				NS_LOG_ERROR(Log()<<"Was already on");
			m_running = true;
			m_traceStartToggle(true);
		}

		void Behaviour::Stop()
		{
			NS_LOG_FUNCTION(Log());
			m_running = false;
			m_traceStartToggle(false);
		}

		std::string Behaviour::Log() const
		{
			std::ostringstream outstr;
			outstr << m_controller->NodeName() << ": " << ToString(this->GetType()) << ": ";
			return outstr.str();
		}

        void Behaviour::processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages)
        {

        }

        void Behaviour::processTraCIResult(const int result, const Command& command) {
            NS_LOG_INFO(m_controller->LogNode() <<"iCSInferface::TraciCommandResult of " << command.objId << " for variable " << Log::toHex(command.variableId, 2) << " is " << result);
        }

        void Behaviour::processTraCIResult(const double result, const Command& command) {
            NS_LOG_INFO(m_controller->LogNode() <<"iCSInferface::TraciCommandResult of " << command.objId << " for variable " << Log::toHex(command.variableId, 2) << " is " << result);
        }

        void Behaviour::processTraCIResult(const std::string result, const Command& command) {
            NS_LOG_INFO(m_controller->LogNode() <<"iCSInferface::TraciCommandResult of " << command.objId << " for variable " << Log::toHex(command.variableId, 2) << " is " << result);
        }

        void Behaviour::processTraCIResult(const std::vector<std::string> result, const Command& command) {
            std::stringstream ss;
            ss << "[";
            for (std::vector<std::string>::const_iterator i = result.begin(); i != result.end(); ++i) {
                ss  << *i << ", ";
            }
            ss << "]";
            NS_LOG_INFO(m_controller->LogNode() <<"iCSInferface::TraciCommandResult of " << command.objId << " is " << ss.str());
        }

	} /* namespace application */
} /* namespace protocol */
