/****************************************************************************/
/// @file    ics-log.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef ICS_LOG_H
#define ICS_LOG_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

namespace ics {

// ===========================================================================
// enum definitions
// ===========================================================================
/**
 * @enum LogLevel
 * @brief The logging levels
 */
enum LogLevel {
    
    ///@brief Maximum log level error, for critical simulation stopping situations.
    kLogLevelError = 2,

    ///@brief Medium log level error, for situations that could produce odd behaviours in the simulation. The simulation may continue.
    kLogLevelWarning = 1,

    ///@brief Lower log level error, just information.
    kLogLevelInfo = 0,
};

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class IcsLog
 * @brief To control the iCS debugging log file.
 */
class IcsLog {
public:

    /**
    * @brief Initializes the log file.
    * @param [in] path The path in which the messages will be written.
    * @return EXIT_SUCCESS if the file was correctly created, -1 otherwise.
    */
    static int StartLog(std::string path, std::string timeThreshold);

    /**
    * @brief Establishes the log level with the input of the user.
    * @param [in] logLevel The log level defined by the user in the iTETRIS config file.
    */
    static void SetLogLevel(ics::LogLevel logLevel);

    /**
    * @brief
    * @param [in]
    */
    static void SetLogTimeThreshold(char* logThreshold);

    /**
    * @brief
    * @param [in]
    */
    static void SetLogTimeThreshold(std::string logThreshold);

    /// @brief Closes the log file.
    static void Close();

    /**
    * @brief
    * @param [in]
    */
    static bool Log(const char* message);

    /**
    * @brief
    * @param [in]
    */
    static bool LogLevel(const char* message, ics::LogLevel messageLogLevel);

    /**
    * @brief
    * @param [in]
    */
    static std::string GetPath();

private:

    /**
    * @brief
    * @param [in]
    */
    IcsLog(std::string path, std::string timeThreshold);
    
    /// @brief File for the iCS loggin.
    std::ofstream myfile_;

    /// @brief Path of the file
    std::string path_;

    /// @brief Singleton instance
    static IcsLog* instance_;

    static long lineCounter_;

    /// @brief User defined number of timesteps for each log file.
    static long timeStepThreshold_;

    /// @brief Next timestep in which the log file has to be changed.
    static long nextTimeStepThreshold_;

    /// @brief The amount of created log files.
    static int currentNumberLogFiles_;

    /// @brief Defines the log level of the output.
    static ics::LogLevel logLevel_;

    /// @brief Closes and opens a new log file.
    int StartNewFile();
};

}

#endif
