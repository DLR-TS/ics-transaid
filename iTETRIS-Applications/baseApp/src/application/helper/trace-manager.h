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
#ifndef TRACE_MANAGER_H_
#define TRACE_MANAGER_H_

#include <map>
#include <string>
#include "traced-callback.h"

namespace baseapp
{
	namespace application
	{
		typedef std::map<std::string, TracedCallbackBase*> TraceMap;

		/**
		 * @brief Utility class to be extended to allow push model interaction.
		 * @brief A subclass can bind a name to a trace source at with a client can connect
		 * @brief Mainly ported and adapted from ns3
		 */
		class TraceManager
		{
			public:
				TraceManager();
				virtual ~TraceManager()
				{
				}

				/**
				 * @brief Register to a trace a callback method
				 * @brief It does not check the signature of the method.
				 * @brief Could cause a runtime error if the incorrect method is registered
				 * @param[in] key The trace name
				 * @param[in] callback The method to register
				 * @return True if success. False otherwise.
				 */
				virtual bool TraceConnect(const std::string &key, const CallbackBase &callback);
				/**
				 * @brief Disconnect a callback method from a trace if found
				 * @param[in] key The trace name
				 * @param[in] callback The method to disconnect
				 * @return True if success. False otherwise.
				 */
				virtual bool TraceDisconnect(const std::string &key, const CallbackBase &callback);

			protected:
				virtual void RegisterTrace(const std::string &key, TracedCallbackBase &trace);
				virtual void RemoveTrace(const std::string &key);

			private:
				TraceMap m_traceMap;
		};

	} /* namespace application */
} /* namespace protocol */

#endif /* TRACE_MANAGER_H_ */
