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

#include <cstdlib>
#include <set>
#include "server.h"
#include "log/log.h"
#include "log/console.h"
#include "exception/UtilExceptions.h"
#include "program-configuration.h"
#include "output-helper.h"
#include "model/behaviour-factory.h"
#include "uc/behaviour-uc-factory.h"
#include "uc/jsonReader.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace ucapp;

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int main(int argc, char **argv)
{
  int ret = 0;

  if (argc < 3)
  {
    Console::Error("Wrong number of command line arguments.");
    Console::Error("Usage: ucApp [-c] [<config-file>] [--remote-port<port>]");
    return -1;
  }

  char *configFile = NULL;
  char *scenario = NULL;
  bool use_ns3 = false;
  int port = -1;

  std::vector<std::string> argsStr(argv, argv + argc);

  for (unsigned int i = 1; i < argsStr.size(); i++)
  {
    std::size_t found = argsStr[i].find("-c");

    if (found != std::string::npos)
    {
      configFile = const_cast<char *>(argsStr[i + 1].c_str());
      i += 1;
      continue;
    }

    found = argsStr[i].find("--remote-port");
    if (found != std::string::npos)
    {
      port = atoi(argsStr[i + 1].c_str());
      i += 1;
      continue;
    }

    found = argsStr[i].find("-s");
    if (found != std::string::npos)
    {
      scenario = const_cast<char *>(argsStr[i + 1].c_str());
      i += 1;
      continue;
    }

    found = argsStr[i].find("--ns3");
    if (found != std::string::npos)
    {
      use_ns3 = true;
      continue;
    }

  }

  if (configFile == NULL)
  {
    std::cout << "error 0" << std::endl;
    Console::Error("Expected '-c' Usage: ucApp [-c <config-file>] [--remote-port <port>] [-s <json file> ] [--ns3]");
    return -2;
  }

  if (port == -1)
  {
    std::cout << "error 1" << std::endl;
    Console::Error("Expected '--remote-port' Usage: ucApp [-c] <config-file> [--remote-port <port>] ");
    return -2;
  }

  try
  {
    Console::SetAppName(argv[0]);

    // start-up
    Console::Log("Starting TransAID UC app");

    if (JsonReader::Open(scenario) == EXIT_FAILURE)
      throw ProcessError("Could not load scenario json file");

    if (ProgramConfiguration::LoadConfiguration(configFile, port) == EXIT_FAILURE)
      throw ProcessError("Could not load configuration file");

    Console::Log("Load Configuration done");

    // Initialize log file
    std::string log;
    if (ProgramConfiguration::GetLogFileName(LOG_FILE, log))
    {
      Log::StartLog(LOG_FILE, log);
      Log::WriteHeader(LOG_FILE, "Test App Log File");
    }

    if (ProgramConfiguration::GetLogFileName(NS_LOG_FILE, log))
    {
      Log::StartLog(NS_LOG_FILE, log);
      Log::WriteHeader(NS_LOG_FILE, "Test App NS-Log File");
    }

    if (ProgramConfiguration::GetLogFileName(DATA_FILE, log))
    {
      baseapp::application::OutputHelper::Start(log);
    }

    if (ProgramConfiguration::GetSocketPort() < 1024)
      throw ProcessError("Please use a socket port above 1024");

    // Start the server
    Console::Log("Starting the server.");
    baseapp::server::Server::RunServer(new ucapp::application::BehaviourUCFactory(use_ns3));

    ret = 0;
  }
  catch (std::runtime_error &e)
  {
    if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string(""))
    {
      Console::Error(e.what());
    }
    Console::Error("Quitting (on error).");
    ret = 1;
  }
  catch (...)
  {
    Console::Error("Quitting (on unknown error).");
    ret = 2;
  }

  delete baseapp::application::OutputHelper::Instance();
  Log::Close();

  JsonReader::Close();

  return ret;
}
