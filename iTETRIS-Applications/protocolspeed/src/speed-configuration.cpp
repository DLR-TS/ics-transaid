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

#include <memory>
#include "utils/xml/tinyxml2.h"
#include "speed/data-manager.h"
#include "speed/protocols.h"
#include "speed-configuration.h"

namespace testapp
{

	int SpeedConfiguration::LoadConfiguration(const char * fileName, int port)
	{
		if (m_instance == nullptr) {
			m_instance = std::unique_ptr<SpeedConfiguration>(new SpeedConfiguration());
		}
		return ProgramConfiguration::LoadConfiguration(fileName, port);
	}

	int SpeedConfiguration::ParseSetup(tinyxml2::XMLElement* setup)
	{
		ProgramConfiguration::ParseSetup(setup);
		int iVal;
		double dVal;
		bool bVal;
		tinyxml2::XMLElement* xmlElem = setup->FirstChildElement("data-manager");
		if (xmlElem)
		{
			if (xmlElem->QueryBoolAttribute("enabled", &bVal) == tinyxml2::XML_NO_ERROR)
				application::DataManager::Enabled = bVal;
			if (xmlElem->QueryIntAttribute("execute-time", &iVal) == tinyxml2::XML_NO_ERROR)
				if (iVal >= 0 && iVal <= 65535)
					application::DataManager::ExecuteTime = iVal;
			if (xmlElem->QueryBoolAttribute("enable-centralized-protocol", &bVal) == tinyxml2::XML_NO_ERROR)
				application::DataManager::EnableCentralizedProtocol = bVal;
		}
		xmlElem = setup->FirstChildElement("centralized-protocol");
		if (xmlElem)
		{
			if (xmlElem->QueryDoubleAttribute("space-threshold", &dVal) == tinyxml2::XML_NO_ERROR)
				application::CentralizedProtocol::SpaceThreshold = dVal;
			if (xmlElem->QueryBoolAttribute("return-data", &bVal) == tinyxml2::XML_NO_ERROR)
				application::CentralizedProtocol::ReturnData = bVal;
		}
		return EXIT_SUCCESS;
	}
} /* namespace protocol */
