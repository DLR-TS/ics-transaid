/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/****************************************************************************/
/// @file    iCS_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Julen Maneros
/// @date    08.09.2009
/// @version $Id: iCS_main.cpp 7536 2009-07-27 13:20:25Z dkrajzew $
///
// Main for the iTETRIS Control System
/****************************************************************************/
// iTETRIS; see http://www.ict-itetris.eu/
// Copyright 2008-2010 iTETRIS consortium
/****************************************************************************/

/****************************************************************************************
 * Modified and Adapted for SINETIC
 * Author: Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/

/****************************************************************************************
 * Edited for TransAID
 * Author: Alejandro Correa Vila (acorrea@umh.es)
 * UMH 2018
 * TransAID project
 *
 ***************************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef _WIN32
#include <windows.h> // needed for Sleep
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <limits>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <iostream>
#include <string>
#include <limits.h>
#include <ctime>
#include <utils/common/MsgHandler.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/iodevices/OutputDevice.h>
#include <time.h>
#include <algorithm>

#include "ics/ics.h"
#include "ics/utilities.h"
#include "utils/ics/log/ics-log.h"
#include "ics/itetris-simulation-config.h"
#include "ics/sync-manager.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace ics;

// ===========================================================================
// functions
// ===========================================================================
void fillOptions()
{
  OptionsCont &oc = OptionsCont::getOptions();
  oc.clearCopyrightNotices();
  oc.addCopyrightNotice("(c) iTETRIS consortium 2008-2010; http://www.ict-itetris.eu/");
  oc.addCopyrightNotice("(c) COLOMBO consortium 2012-2015; http://www.colombo-fp7.eu/");
  oc.addCopyrightNotice("(c) TransAID consortium 2018; http://www.trainsaid.eu/");
  oc.addCopyrightNotice("... and others (see source files).");
  oc.addCallExample("-c <CONFIGURATION>", "");

  // insert options sub-topics
  SystemFrame::addConfigurationOptions(oc);
  oc.addOptionSubTopic("Scenario");
  oc.addOptionSubTopic("TrafficSim");
  oc.addOptionSubTopic("CommunicationSim");
  oc.addOptionSubTopic("Applications");
  oc.addOptionSubTopic("Logs");
  oc.addOptionSubTopic("Output");
  SystemFrame::addReportOptions(oc);

  // insert options for scenario
  oc.doRegister("begin", 'b', new Option_Integer(0));
  oc.addDescription("begin", "Scenario", "Defines the begin time of the scenario simulation");

  oc.doRegister("end", 'e', new Option_Integer(-1));
  oc.addDescription("end", "Scenario", "Defines the end time of the scenario simulation");

  oc.doRegister("resolution", new Option_Integer(1000));
  oc.addDescription("resolution", "Scenario", "Time resolution of every simulation step (ms)");

  oc.doRegister("penetration-rate", 'r', new Option_Integer(-1));
  oc.addDescription("penetration-rate", "Scenario", "Defines the percentage of vehicles equipped with a RAT");

  oc.doRegister("vehicleSelector", new Option_String(""));
  oc.addDescription("vehicleSelector", "Scenario", "A list of substrings to determine whether vehicles are equipped with a RAT by their ID");

  oc.doRegister("facilities-config-file", 'f', new Option_FileName());
  oc.addDescription("facilities-config-file", "Scenario",
      "The entry point file for the facilities block configuration.");

  oc.doRegister("message-reception-window", 'm', new Option_Integer(0));
  oc.addDescription("message-reception-window", "Scenario",
      "Defines the time the iCS will keep schedule messages in the internal tables. That time window past, they are erased.");

  oc.doRegister("interactive", new Option_Bool(false));
  oc.addDescription("interactive", "Scenario", "Whether iCS shall be run in interactive mode");

  // insert options for traffic simulation
  oc.doRegister("traffic-executable", new Option_String());
  oc.addDescription("traffic-executable", "TrafficSim", "Defines the traffic simulation executable");

  oc.doRegister("traffic-file", new Option_FileName());
  oc.addDescription("traffic-file", "TrafficSim", "Defines the traffic simulation configuration");

  oc.doRegister("traffic-host", new Option_String("localhost"));
  oc.addDescription("traffic-host", "TrafficSim", "Defines the host the traffic simulation shall run on");

  oc.doRegister("traffic-port", new Option_Integer(-1));
  oc.addDescription("traffic-port", "TrafficSim", "Defines the port the traffic simulation shall use");

  // insert options for communication simulation
  oc.doRegister("communication-executable", new Option_String());
  oc.addDescription("communication-executable", "CommunicationSim", "Defines the communication simulation executable");

  oc.doRegister("communication-host", new Option_String("localhost"));
  oc.addDescription("communication-host", "CommunicationSim",
      "Defines the host the communication simulation shall run on");

  oc.doRegister("communication-port", new Option_Integer(-1));
  oc.addDescription("communication-port", "CommunicationSim",
      "Defines the port the communication simulation shall use");

  oc.doRegister("communication-general-params-file", new Option_String());
  oc.addDescription("communication-general-params-file", "CommunicationSim",
      "Defines the general ns-3 script configuration.");

  oc.doRegister("communication-config-technologies-file", new Option_String());
  oc.addDescription("communication-config-technologies-file", "CommunicationSim",
      "Defines the communication technologies configuration in ns-3");

  // insert options for applications
  oc.doRegister("app-config-file", 'a', new Option_FileName());
  oc.addSynonyme("app-config-file", "apps");
  oc.addDescription("app-config-file", "Applications", "Defines the Application configurations");

  // insert option for ics log file
  oc.doRegister("ics-log-path", new Option_FileName());
  oc.addDescription("ics-log-path", "Logs", "Defines the place where the iCS log file will be stored");

  // insert option for ics log file
  oc.doRegister("ics-log-time-size", new Option_String());
  oc.addDescription("ics-log-time-size", "Logs", "Defines the time step amount for each log file.");

  // insert option for log level
  oc.doRegister("ics-log-level", new Option_String());
  oc.addDescription("ics-log-level", "Logs", "Defines the output level of the log [ERROR, WARNING, INFO]");

  // insert option for log start and end (to slice a small part for tests)
  oc.doRegister("ics-log-begin", new Option_Integer(-1));
  oc.addDescription("ics-log-begin", "Logs", "Defines the time for the start of the ics-log in [ms].");

  oc.doRegister("ics-log-end", new Option_Integer(-1));
  oc.addDescription("ics-log-end", "Logs", "Defines the time for the end of the ics-log in [ms].");

  oc.doRegister("ics-log-omit-systime", new Option_Bool(false));
  oc.addDescription("ics-log-omit-systime", "Logs", "Whether system time shall be omitted in logging output");

  // insert option for ns3 log file
  oc.doRegister("ns3-log-path", new Option_FileName());
  oc.addDescription("ns3-log-path", "Logs", "Defines the place where the ns-3 log file will be stored");

  // add rand options
  RandHelper::insertRandOptions();
}

bool checkOptions()
{
  bool ret = true;
  OptionsCont &oc = OptionsCont::getOptions();

  // check last time step
  if (oc.getInt("end") < 0)
  {
    MsgHandler::getErrorInstance()->inform("Invalid ending simulation timestep.");
    ret = false;
  }

  // vehicle penetration rate
  if (oc.getInt("penetration-rate") < 0)
  {
    MsgHandler::getErrorInstance()->inform("Invalid overall vehicle penetration rate.");
    ret = false;
  }

  // check facilities config file
  if (!oc.isSet("facilities-config-file"))
  {
    MsgHandler::getErrorInstance()->inform("Missing path of the facilities config file.");
    ret = false;
  }

  // check facilities config file
  if (oc.getInt("message-reception-window") < 0)
  {
    MsgHandler::getErrorInstance()->inform("Invalid message reception time window.");
    ret = false;
  }

#ifdef SUMO_ON
  // sumo executable
  if (!oc.isSet("traffic-executable"))
  {
    MsgHandler::getErrorInstance()->inform("Missing definition of the traffic executable.");
    ret = false;
  }

  // traffic scenario defintion file
  if (!oc.isSet("traffic-file"))
  {
    MsgHandler::getErrorInstance()->inform("Missing traffic simulation configuration file definition.");
    ret = false;
  }

  // check traffic simulation settings
  if (oc.getInt("traffic-port") < 0)
  {
    MsgHandler::getErrorInstance()->inform("Invalid definition of the traffic simulation port to use.");
    ret = false;
  }
#endif

#ifdef NS3_ON
  // ns-3 executable
  if (!oc.isSet("communication-executable"))
  {
    MsgHandler::getErrorInstance()->inform("Missing definition of the communication executable.");
    ret = false;
  }

  // check communication settings
  if (oc.getInt("communication-port") < 0)
  {
    MsgHandler::getErrorInstance()->inform("Invalid definition of the communication simulation port to use.");
    ret = false;
  }

  // check communications general parameters file
  if (!oc.isSet("communication-general-params-file"))
  {
    MsgHandler::getErrorInstance()->inform(
        "Missing definition of the general parameters file of the communication simulator.");
    ret = false;
  }

  // check communications general parameters file
  if (!oc.isSet("communication-config-technologies-file"))
  {
    MsgHandler::getErrorInstance()->inform(
        "Missing definition of the technologies configuration file of the communication simulator.");
    ret = false;
  }



#endif

#ifdef APPLICATIONS_ON
  // check app config file
  if (!oc.isSet("apps"))
  {
    MsgHandler::getErrorInstance()->inform("Missing path of the Application config file.");
    ret = false;
  }
#endif

#ifdef LOG_ON
  // check ics log path
  if (!oc.isSet("ics-log-path"))
  {
    MsgHandler::getErrorInstance()->inform("Missing path of the iCS log file.");
    ret = false;
  }

  // check ns-3 log path
  if (!oc.isSet("ns3-log-path"))
  {
    MsgHandler::getErrorInstance()->inform("Missing path of the ns-3 log file.");
    ret = false;
  }
#endif

  return ret;
}

void *launchSystemExecutable(void *ptr)
{
  char *message;
  message = (char *) ptr;
  system(message);
  pthread_exit(NULL);
  return 0;
}

ICS*
launchIcsThreadless(int commPort, int trafficPort)
{
  OptionsCont &oc = OptionsCont::getOptions();
  ICS *ics = new ICS(commPort, trafficPort, oc.getString("traffic-host"),
      oc.getString("communication-host"), oc.getInt("begin"), oc.getInt("end"), oc.getInt("resolution"),
      oc.getInt("penetration-rate"), oc.getStringVector("vehicleSelector"), oc.getBool("interactive"));

  ics::ITetrisSimulationConfig::m_scheduleMessageCleanUp = oc.getInt("message-reception-window") * oc.getInt("resolution");

  if (ics->Setup(oc.getString("facilities-config-file"), oc.getString("apps")) == EXIT_SUCCESS)
  {
    ics->Run();
  }
  else
  {
    ics->Close();
    cout << "iCS --> Error occurred during iCS set-up" << endl;
    return 0;
  }
  return ics;
}

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int main(int argc, char **argv)
{
  //Simulation start time is set to 0
  utils::Conversion::m_startTime = time(NULL);

  OptionsCont &oc = OptionsCont::getOptions();
  // give some application descriptions
  oc.setApplicationDescription("The 'iTETRIS Control System'.");
#ifdef WIN32
  oc.setApplicationName("iCS.exe", "iTETRIS Control System Version " + (std::string) VERSION_STRING);
#else
  oc.setApplicationName("iCS", "iTETRIS Control System Version " + (std::string)VERSION_STRING);
#endif

  int ret = 0;
  ICS* ics = 0;
  bool logOn = false; // to check whether a log path for the iCS debuggin is defined

  try
  {
    // start-up
    XMLSubSys::init(); // xml-initialization
    //  options parsing
    fillOptions();
    OptionsIO::setArgs(argc, argv);
    OptionsIO::getOptions();
    if (oc.processMetaOptions(argc < 2))
    {
      SystemFrame::close();
      return 0;
    }
    //  message handler start-up
    MsgHandler::initOutputOptions();
    //  options verification
    if (!checkOptions())
      throw ProcessError();
    //  random numbers initialization
    RandHelper::initRandGlobal();

    cout << endl;
    cout << "WELCOME TO iTETRIS" << endl;
    cout << "==================" << endl;

    pthread_t sumoThread, ns3Thread;

    logOn = oc.isSet("ics-log-path");

    if (logOn)
    {
      // parse log start and end
      icstime_t logStart, logEnd ;
      logStart = oc.getInt("ics-log-begin");
      logEnd = oc.getInt("ics-log-end");
      bool omitSysTime = oc.getBool("ics-log-omit-systime");

      IcsLog::StartLog(oc.getString("ics-log-path"), oc.getString("ics-log-time-size"), logStart, logEnd, omitSysTime);
      string loglevel = oc.getString("ics-log-level");
      std::transform(loglevel.begin(), loglevel.end(), loglevel.begin(), ::toupper);
      if (loglevel == "INFO")
      {
        IcsLog::SetLogLevel(ics::kLogLevelInfo);
        IcsLog::Log("Log level is INFO");
      }
      else
      {
        if (loglevel == "WARNING")
        {
          IcsLog::SetLogLevel(ics::kLogLevelWarning);
          IcsLog::Log("Log level is WARNING");
        }
        else
        {
          if (loglevel == "ERROR")
          {
            IcsLog::SetLogLevel(ics::kLogLevelError);
            IcsLog::Log("Log level is ERROR");
          }
          else
          {
            IcsLog::Log("Log level is not correct");
            cout << "iCS --> [INFO] Log level in the config file is unknown" << endl;
          }
        }
      }
      IcsLog::Log("WELCOME TO iTETRIS");
#ifndef LOG_ON
      IcsLog::Log("LOG IS DISABLED");
#endif
    }

    bool interactive = oc.getBool("interactive");
    if (interactive)
    {
      cout << endl;
      cout << "STEP 1 - SETUP PHASE" << endl;
      cout << "====================" << endl;
      cout << "[ns-3] OFF" << endl;
      cout << "[APP]  OFF" << endl;
      cout << "[iCS]  STARTS SEPARATED THREADS FOR EACH SIMULATOR." << endl;
      cout << "[SUMO] OFF" << endl << endl;
      utils::Conversion::Wait("Press <Enter> to continue...");
      cout << endl;
    }
    else
    {
      cout << "STEP 1 - SETUP PHASE" << endl;
    }

#ifdef SUMO_ON
    //Launch SUMO
    int trafficPort = oc.getInt("traffic-port");
    if (trafficPort == 0) {
        trafficPort = tcpip::Socket::getFreeSocketPort();
    }
    std::string sumoCall = oc.getString("traffic-executable") + " " + oc.getString("traffic-file") + " --remote-port=" + to_string(trafficPort);

    // Debug: bypass for manual debug launch
//    sumoCall = "echo '" + sumoCall + "'";

    char* sumoChain = strdup(sumoCall.c_str());
    pthread_create(&sumoThread, NULL, launchSystemExecutable, (void *) sumoChain);
    cout << "iCS --> SUMO launched." << endl;
#ifdef LOG_ON
    IcsLog::LogLevel("SUMO launched.", ics::kLogLevelWarning);
#endif
    Sleep(1);
#endif

#ifdef NS3_ON
    int commPort = oc.getInt("communication-port");
    if (commPort == 0) {
        commPort = tcpip::Socket::getFreeSocketPort();
    }
    string exec_aux = oc.getString("communication-executable");
    if (exec_aux.find("lightcomm") == string::npos ){
        //Launch ns-3
        std::string ns3Call;
        //ns3Call = " gdb --args " + oc.getString("communication-executable") + " ";
        ns3Call = oc.getString("communication-executable") + " ";
        ns3Call += " --inciPort=" + to_string(commPort);
        ns3Call += " --fileGeneralParameters=" + oc.getString("communication-general-params-file");
        ns3Call += " --fileConfTechnologies=" + oc.getString("communication-config-technologies-file");
        if (!oc.getString("ns3-log-path").empty())
            ns3Call += " --logFile=" + oc.getString("ns3-log-path");
        char* ns3Chain = strdup(ns3Call.c_str());
        pthread_create(&ns3Thread, NULL, launchSystemExecutable, (void *) ns3Chain);
        cout << "iCS --> ns-3 launched." << endl;
        Sleep(1);
    }else{
        //Launch Lightcomm simulator
        std::string lightcommCall= oc.getString("communication-executable");
        char* executable_lightcomm =strdup(lightcommCall.c_str());
        pthread_create(&ns3Thread, NULL, launchSystemExecutable, (void *) executable_lightcomm);
        cout << "iCS --> lightcomm launched." << endl;
        Sleep(1);
    }
#endif

    //Launch iCS
    ics = launchIcsThreadless(commPort, trafficPort);
    if (ics == 0)
    {
      throw ProcessError();
    }
    cout << endl;
  } catch (std::runtime_error &e)
  {
    MsgHandler::getErrorInstance()->inform(e.what());
    MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
    ret = EXIT_FAILURE;
    if (ics != NULL)
    {
      ics->Close();
      delete ics;
    }
  } catch (std::exception &e)
  {
    MsgHandler::getErrorInstance()->inform(e.what());
    MsgHandler::getErrorInstance()->inform("Exception ", false);
    ret = EXIT_FAILURE;
    delete ics;
  }

  catch (...)
  {
    MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
    ret = EXIT_FAILURE;
    delete ics;
  }        //try+catch

  if (ics != 0)
    OutputDevice::closeAll();
  SystemFrame::close();
  if (ret != EXIT_FAILURE)
  {
    std::cout << "Success." << std::endl;
  }

  utils::Conversion::m_endTime = time(NULL);
  std::cout << "Elapsed time (in seconds): " << utils::Conversion::GetElapsedTime() << std::endl;

  if (logOn)
  {
    stringstream log;
    log << "Elapsed time (in seconds): " << utils::Conversion::GetElapsedTime();
    IcsLog::Log((log.str()).c_str());
    IcsLog::Close();
  }

//     delete ics;

  return ret;
}
/****************************************************************************/
