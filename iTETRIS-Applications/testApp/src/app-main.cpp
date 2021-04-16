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
int main(int argc, char** argv) {
    int ret = 0;
    Console::SetAppName("testApp");

    if (argc != 2 && argc != 3 && argc != 5) {
        Console::Error("Wrong number of command line arguments.");
        Console::Error("Usage: testApp [-c] <config-file> [--remote-port <port>]");
        return -1;
    }
    char* configFile;
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

    try {
        std::set<std::string> testCases({"",
                                         "acosta",
                                         "simpleExecute",
                                         "setVType",
                                         "inductionLoop",
                                         "commSimple",
                                         "commSimple2",
                                         "commRSU2Vehicle",
                                         "CAMsimple",
                                         "drivingDistance",
                                         "getLeader",
                                         "GetTraciParameterWithKey",
                                         "getSUMOStepLength",
                                         "testMobility",
                                         "testTrajectory",
                                         "testToC",
                                         "testOpenGap",
                                         "testV2XmsgSet",
                                         "TMCBehaviour",
                                         "TMCBehaviour_multiRSU",
                                         "testMessageScheduler"});

        // start-up
        Console::Log("Starting iTetris test app");
        if (ProgramConfiguration::LoadConfiguration(configFile, port) == EXIT_FAILURE) {
            throw ProcessError("Could not load configuration file");
        }
        if (testCases.count(ProgramConfiguration::GetTestCase()) == 0) {
            throw ProcessError("Unknown test case '" + ProgramConfiguration::GetTestCase() + "'");
        }

        Console::Log("Load Configuration done");
        // Initialize log file
        std::string log;
        if (ProgramConfiguration::GetLogFileName(LOG_FILE, log)) {
            Log::StartLog(LOG_FILE, log);
            Log::WriteHeader(LOG_FILE, "Test App Log File");
        }
        if (ProgramConfiguration::GetLogFileName(NS_LOG_FILE, log)) {
            Log::StartLog(NS_LOG_FILE, log);
            Log::WriteHeader(NS_LOG_FILE, "Test App NS-Log File");
        }
        if (ProgramConfiguration::GetLogFileName(DATA_FILE, log)) {
            baseapp::application::OutputHelper::Start(log);
        }

        if (ProgramConfiguration::GetSocketPort() < 1024) {
            throw ProcessError("Please use a socket port above 1024");
        }

        // Start the server
        baseapp::server::Server::RunServer(new testapp::application::BehaviourTestFactory());

        ret = 0;

    } catch (std::runtime_error& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            Console::Error(e.what());
        }
        Console::Error("Quitting (on error).");
        ret = 1;
    } catch (...) {
        Console::Error("Quitting (on unknown error).");
        ret = 2;
    }
    delete baseapp::application::OutputHelper::Instance();
    Log::Close();
    return ret;
}
