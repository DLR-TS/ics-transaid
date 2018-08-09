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

#include "server.h"
#include "log/log.h"
#include "log/console.h"
#include "exception/UtilExceptions.h"
#include "program-configuration.h"
#include "output-helper.h"
#include <cstdlib>

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include "config.h"
#endif

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace testapp;

// ===========================================================================
// functions
// ===========================================================================
/* -------------------------------------------------------------------------
 * options initialisation
 * ----------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */

uint32_t stringToUInt(std::string value)
{
  uint32_t ret;
  std::stringstream str;
  str << value;
  str >> ret;
  return ret;
}

double stringToDouble(std::string value)
{
  double ret;
  std::stringstream str;
  str << value;
  str >> ret;
  return ret;
}

void loadOptionFile()
{
  using namespace std;
  using namespace application;
  string file = "option.txt";
  ifstream inFile;
  inFile.open(file.c_str());
  if (!inFile.good())
  {
    Console::Warning(" No option file found. Must be named option.txt");
  }
  else
  {
    string key, value;
    while (inFile >> key >> value)
    {
      if (key == "RngRun")
      {
        ns3::RngSeedManager::SetRun(stringToUInt(value));
        Console::Log("RngRun set to ", value.c_str());
      }
      else if (key == "RngSeed")
      {
        ns3::RngSeedManager::SetSeed(stringToUInt(value));
        Console::Log("RngSeed set to ", value.c_str());
      }
      else if (key == "ProbFull")
      {
        Node::ProbabilityFull = stringToDouble(value);
        Console::Log("ProbFull set to ", value.c_str());
      }
      else if (key == "ProbMed")
      {
        Node::ProbabilityMedium = stringToDouble(value);
        Console::Log("ProbMed set to ", value.c_str());
      } else if (key == "test") {
          if(ProgramConfiguration::GetTestCase() != TEST_CASE_NONE) {
              Console::Warning(" Test case already set. Ignoring further values '"+value+"'");
          } else {
              bool testCaseExists = true;
              if (value=="simpleExecute"){
                  ProgramConfiguration::SetTestCase(TEST_CASE_EXECUTE);
              } else if (value=="setVType"){
                  ProgramConfiguration::SetTestCase(TEST_CASE_SETVTYPE);
              } else if (value=="acosta"){
                  ProgramConfiguration::SetTestCase(TEST_CASE_ACOSTA);
              } else if (value=="inductionLoop"){
                  ProgramConfiguration::SetTestCase(TEST_CASE_INDUCTIONLOOP);
              } else {
                  testCaseExists = false;
              }
              if (testCaseExists) {
                  Console::Log(" Test case set to ", value.c_str());
              } else {
                  Console::Warning(" No test case '"+value+"' exists. Ignoring option.");
              }
          }
      }
    }
  }
  inFile.close();
}

int main(int argc, char **argv)
{
  int ret = 0;

  if (argc != 2 && argc != 3)
  {
    Console::Error("Missing configuration file or too many arguments");
    Console::Error("Please call as appname configfile.xml or appname -c configfile.xml");
    return -1;
  }
  char * configFile;
  if (argc == 2)
    configFile = argv[1];
  else
  {
    std::string arg(argv[1]);
    if (arg != "-c")
    {
      Console::Error("Expected -c read " + arg);
      return -2;
    }
    configFile = argv[2];
  }

  try
  {
    // start-up
    Console::Log("Starting iTetris test app");
    if (ProgramConfiguration::LoadConfiguration(configFile) == EXIT_FAILURE)
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
      application::OutputHelper::Start(log);
    }

    if (ProgramConfiguration::GetSocketPort() < 1024)
      throw ProcessError("Please use a socket port above 1024");

    loadOptionFile();
    // Start the server
    server::Server::RunServer();

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
  delete application::OutputHelper::Instance();
  Log::Close();
  return ret;
}
