/****************************************************************************************
 * Copyright (c) 2020 Centre for Research and Technology-Hellas (CERTH)
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
 * CERTH and its contributors''.
 * 4. Neither the name of the Centre nor the names of its contributors may be used to
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
 * Author Vasilios Karagounis
  ***************************************************************************************/

#include "jsonReader.h"
#include "log/console.h"

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <limits.h>
#include <unistd.h>

using namespace std;
using namespace baseapp;

JsonReader * JsonReader::m_instance(NULL);
json JsonReader::object;
//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
JsonReader::JsonReader()
{
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
JsonReader::~JsonReader()
{

}
//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
int JsonReader::Open(const std::string & _name)
{

	if (m_instance == NULL)
		m_instance = new JsonReader();

	string pathJson = string("settings/") + string(_name);
	
	Console::Log(std::string("Path json file : ") + pathJson.c_str());

	ifstream file_json(pathJson.c_str(), ifstream::in);

	try
	{
		if (file_json.is_open())
		{
			object = json::parse(file_json);
		}
		else
		{
			Console::Error("Can not open json file. Check filename...");
			return EXIT_FAILURE;
		}
	}
	catch (json::exception &e)
	{
		Console::Error(std::string("Json error : ") + e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void JsonReader::Close()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

