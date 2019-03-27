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
#include "test/behaviour-test-factory.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace testapp;

// ===========================================================================
// functions
// ===========================================================================
/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int main(int argc, char **argv)
{
  int ret = 0;
  Console::SetAppName("testApp");

  if (argc != 2 && argc != 3 && argc != 5)
  {
      Console::Error("Wrong number of command line arguments.");
      Console::Error("Usage: testApp [-c] <config-file> [--remote-port <port>]");
      return -1;
  }
  char * configFile;
  int port = -1;
  if (argc == 2) {
      configFile = argv[1];
  } else if (argc >= 3) {
      std::string arg(argv[1]);
      if (arg == "-c") {
          configFile = argv[2];
      } else {
          Console::Error("Expected '-c' read " + arg + " Usage: testApp [-c] <config-file> [--remote-port <port>]");
          return -2;
      }
      if (argc == 5) {
          arg = std::string(argv[3]);
          if (arg == "--remote-port") {
              port = atoi(argv[4]);
          } else {
              Console::Error("Expected '--remote-port' read " + arg + " Usage: testApp [-c] <config-file> [--remote-port <port>]");
              return -2;
          }
      }
  }

  try
  {
    std::set<std::string> testCases({"", "acosta", "simpleExecute", "setVType", "inductionLoop",
                                     "commSimple", "commSimple2", "CAMsimple", "drivingDistance",
                                     "getSUMOStepLength", "testMobility", "testTrajectory", "testToC",
                                     "testOpenGap", "testV2XmsgSet", "TMCBehaviour"});
    // start-up
    Console::Log("Starting iTetris test app");
    if (ProgramConfiguration::LoadConfiguration(configFile, port) == EXIT_FAILURE)
      throw ProcessError("Could not load configuration file");
    if (testCases.count(ProgramConfiguration::GetTestCase()) == 0) {
      throw ProcessError("Unknown test case '" + ProgramConfiguration::GetTestCase() + "'");
    }

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
    baseapp::server::Server::RunServer(new testapp::application::BehaviourTestFactory());

    ret = 0;

  } catch (std::runtime_error &e)
  {
    if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string(""))
    {
      Console::Error(e.what());
    }
    Console::Error("Quitting (on error).");
    ret = 1;
  } catch (...)
  {
    Console::Error("Quitting (on unknown error).");
    ret = 2;
  }
  delete baseapp::application::OutputHelper::Instance();
  Log::Close();
  return ret;
}
