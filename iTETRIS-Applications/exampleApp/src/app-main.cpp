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
int main(int argc, char** argv) {
    int ret = 0;

    if (argc < 3) {
        Console::Error("Wrong number of command line arguments.");
        Console::Error("Usage: ucApp [-c] [<config-file>] [--remote-port<port>]");
        return -1;
    }

    char* configFile = NULL;
    char* scenario = NULL;
    bool use_ns3 = false;
    int port = -1;

    std::vector<std::string> argsStr(argv, argv + argc);

    for (unsigned int i = 1; i < argsStr.size(); i++) {
        std::size_t found = argsStr[i].find("-c");

        if (found != std::string::npos) {
            configFile = const_cast<char*>(argsStr[i + 1].c_str());
            i += 1;
            continue;
        }

        found = argsStr[i].find("--remote-port");
        if (found != std::string::npos) {
            port = atoi(argsStr[i + 1].c_str());
            i += 1;
            continue;
        }

        found = argsStr[i].find("-s");
        if (found != std::string::npos) {
            scenario = const_cast<char*>(argsStr[i + 1].c_str());
            i += 1;
            continue;
        }

        found = argsStr[i].find("--ns3");
        if (found != std::string::npos) {
            use_ns3 = true;
            continue;
        }

    }

    if (configFile == NULL) {
        std::cout << "error 0" << std::endl;
        Console::Error("Expected '-c' Usage: ucApp [-c <config-file>] [--remote-port <port>] [-s <json file> ] [--ns3]");
        return -2;
    }

    if (port == -1) {
        std::cout << "error 1" << std::endl;
        Console::Error("Expected '--remote-port' Usage: ucApp [-c] <config-file> [--remote-port <port>] ");
        return -2;
    }

    try {
        Console::SetAppName(argv[0]);

        // start-up
        Console::Log("Starting TransAID UC app");

        if (JsonReader::Open(scenario) == EXIT_FAILURE) {
            throw ProcessError("Could not load scenario json file");
        }

        if (ProgramConfiguration::LoadConfiguration(configFile, port) == EXIT_FAILURE) {
            throw ProcessError("Could not load configuration file");
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
        Console::Log("Starting the server.");
        baseapp::server::Server::RunServer(new ucapp::application::BehaviourUCFactory(use_ns3));

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

    JsonReader::Close();

    return ret;
}
