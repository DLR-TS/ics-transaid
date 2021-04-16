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
#include "program-configuration.h"
#include <cstdlib>
#include <stdint.h>

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include "config.h"
#endif

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace lightcomm;

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

string charToString(char value) {
    string ret;
    std::stringstream str;
    str << value;
    str >> ret;
    return ret;
}

int main(int argc, char** argv) {
    int ret = 0;

    if (argc != 2 && argc != 3 && argc != 5) {
        //  Console::Error("Wrong number of command line arguments.");
        //  Console::Error("Usage: lightcomm [-c] <config-file> [--remote-port <port>]");
        std::cout << "Lightcomm: Wrong number of command line arguments" << endl;
        return -1;
    }
    char* configFile = nullptr;
    int port = -1;
    if (argc == 2) {
        configFile = argv[1];
    } else if (argc >= 3) {
        std::string arg(argv[1]);
        if (arg == "-c") {
            configFile = argv[2];
        } else if (arg == "--remote-port") {
            port = atoi(argv[2]);
        } else {
            //  Console::Error("Expected '-c' read " + arg + " Usage: lightcomm [-c] <config-file> [--remote-port <port>]");
            std::cout << "Expected '-c' or '--remote-port' but read " + arg + " Usage: lightcomm [[-c] <config-file> | --remote-port <port>]" << endl;
            return -2;
        }
        if (argc == 5) {
            arg = std::string(argv[3]);
            if (arg == "--remote-port") {
                port = atoi(argv[4]);
            } else {
                //Console::Error("Expected '--remote-port' read " + arg + " Usage: lightcomm [-c] <config-file> [--remote-port <port>]");
                std::cout << "Expected '--remote-port' read " + arg + " Usage: lightcomm [-c] <config-file> [--remote-port <port>]" << endl;
                return -2;
            }
        }
    }



    try {
        // start-up
        ProgramConfiguration::LoadConfiguration(configFile, port);

        // Start the server
        server::Server::RunServer();

        ret = 0;

    } catch (std::runtime_error& e) {
        ret = 1;
    }
    return ret;
}
