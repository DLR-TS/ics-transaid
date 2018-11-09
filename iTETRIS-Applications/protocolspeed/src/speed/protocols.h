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

#ifndef SRC_APPLICATION_MODEL_PROTOCOLS_H_
#define SRC_APPLICATION_MODEL_PROTOCOLS_H_

#include "data-manager.h"

using namespace baseapp;
using namespace baseapp::application;

namespace protocolspeedapp
{
	namespace application
	{

		/**
		 * Abstract class which represents a protocol
		 */
		class ExecuteBase
		{
			public:
				virtual ~ExecuteBase()
				{
				}
				virtual TraceMap & GetTracedCallbacks();
				/**
				 * @brief Process the information inside the DataMap to specify which data to return to iCS
				 * @param[out] data Data to return to iCS
				 * @param[in] dataMap Information known to the rsu
				 * @return true if data has to be sent to iCS. False otherwise
				 */
				virtual bool Execute(DirectionValueMap &data, const DataMap & dataMap) = 0;
			protected:
				/**
				 * @brief List of traces of the protocol
				 */
				TraceMap m_traceMap;
		};

		class CentralizedProtocol: public ExecuteBase
		{
			public:
				static double SpaceThreshold;
				static bool ReturnData;
				CentralizedProtocol();
				virtual ~CentralizedProtocol();
				virtual bool Execute(DirectionValueMap &, const DataMap &);
			private:
				/**
				 * @brief Calculates the data to send back for a particular direction
				 */
				void AggregateDataForDirection(const NodeDataMap&, ValueMap&);
				bool CompluteSpeed(const NodeInfo&, const NodeInfo&, double &, double &);
				static std::string TraceName;
				TracedCallback<std::vector<std::string>&> m_traceFlows;
		};

	} /* namespace application */
} /* namespace protocol */

#endif /* SRC_APPLICATION_MODEL_PROTOCOLS_H_ */
