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
using namespace protocol;

// ===========================================================================
// functions
// ===========================================================================
/* -------------------------------------------------------------------------
 * options initialisation
 * ----------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */

uint32_t stringToUInt(std::string value) {
    uint32_t ret;
    std::stringstream str;
    str << value;
    str >> ret;
    return ret;
}

double stringToDouble(std::string value) {
    double ret;
    std::stringstream str;
    str << value;
    str >> ret;
    return ret;
}

void loadOptionFile() {
    using namespace std;
    using namespace application;
    string file = "option.txt";
    ifstream inFile;
    inFile.open(file.c_str());
    if (!inFile.good()) {
        Console::Warning("No option file found. Must be named option.txt");
    } else {
        string key, value;
        while (inFile >> key >> value) {
            if (key == "RngRun") {
                ns3::RngSeedManager::SetRun(stringToUInt(value));
                Console::Log("RngRun set to ", value.c_str());
            } else if (key == "RngSeed") {
                ns3::RngSeedManager::SetSeed(stringToUInt(value));
                Console::Log("RngSeed set to ", value.c_str());
            } else if (key == "ProbFull") {
                Node::ProbabilityFull = stringToDouble(value);
                Console::Log("ProbFull set to ", value.c_str());
            } else if (key == "ProbMed") {
                Node::ProbabilityMedium = stringToDouble(value);
                Console::Log("ProbMed set to ", value.c_str());
            }
        }
    }
    inFile.close();
}

int main(int argc, char** argv) {
    int ret = 0;

    if (argc != 2 && argc != 3) {
        Console::Error("Missing configuration file or too many arguments");
        Console::Error("Please call as appname configfile.xml or appname -c configfile.xml");
        return -1;
    }
    char* configFile;
    if (argc == 2) {
        configFile = argv[1];
    } else {
        std::string arg(argv[1]);
        if (arg != "-c") {
            Console::Error("Expected -c read " + arg);
            return -2;
        }
        configFile = argv[2];
    }

    try {
        // start-up
        Console::Log("Starting Protocol speed");
        if (ProgramConfiguration::LoadConfiguration(configFile) == EXIT_FAILURE) {
            throw ProcessError("Could not load configuration file");
        }

        Console::Log("Load Configuration done");
        // Initialize log file
        std::string log;
        if (ProgramConfiguration::GetLogFileName(LOG_FILE, log)) {
            Log::StartLog(LOG_FILE, log);
            Log::WriteHeader(LOG_FILE, "Protocol speed Log File");
        }
        if (ProgramConfiguration::GetLogFileName(NS_LOG_FILE, log)) {
            Log::StartLog(NS_LOG_FILE, log);
            Log::WriteHeader(NS_LOG_FILE, "Protocol speed NS-Log File");
        }
        if (ProgramConfiguration::GetLogFileName(DATA_FILE, log)) {
            application::OutputHelper::Start(log);
        }

        if (ProgramConfiguration::GetSocketPort() < 1024) {
            throw ProcessError("Please use a socket port above 1024");
        }

        loadOptionFile();
        // Start the server
        server::Server::RunServer();

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
    delete application::OutputHelper::Instance();
    Log::Close();
    return ret;
}
