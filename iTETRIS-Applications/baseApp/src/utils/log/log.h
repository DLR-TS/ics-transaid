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
#ifndef LOG_H
#define LOG_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

enum LogLevel {
    LOG_NONE = 0x00000000, // no logging

    LOG_ERROR = 0x00000001, // serious error messages only
    LOG_LEVEL_ERROR = 0x00000001,

    LOG_WARN = 0x00000002, // warning messages
    LOG_LEVEL_WARN = 0x00000003,

    LOG_DEBUG = 0x00000004, // rare ad-hoc debug messages
    LOG_LEVEL_DEBUG = 0x00000007,

    LOG_INFO = 0x00000008, // informational messages (e.g., banners)
    LOG_LEVEL_INFO = 0x0000000f,

    LOG_FUNCTION = 0x00000010, // function tracing
    LOG_LEVEL_FUNCTION = 0x0000001f,

    LOG_LOGIC = 0x00000020, // control flow tracing within functions
    LOG_LEVEL_LOGIC = 0x0000003f,
    kLogLevelError,
    kLogLevelWarning,
    kLogLevelInfo
};

#define NS_LOG(level, msg)                \
  do                                      \
  {                                       \
    std::ostringstream oss;               \
    oss.precision(12);										\
    oss << msg;                           \
    Log::Write(1, oss, level);            \
  }                                       \
  while (false)

#define NS_LOG_ERROR(msg)   \
  NS_LOG (LOG_ERROR, msg)

#define NS_LOG_WARN(msg)    \
  NS_LOG (LOG_WARN, msg)

#define NS_LOG_DEBUG(msg)   \
  NS_LOG (LOG_DEBUG, msg)

#define NS_LOG_INFO(msg)    \
  NS_LOG (LOG_INFO, msg)

#define NS_LOG_LOGIC(msg)   \
  NS_LOG (LOG_LOGIC, msg)

#if __GNUG__
#define NS_LOG_FUNCTION(msg)   \
  NS_LOG (LOG_FUNCTION, __PRETTY_FUNCTION__ << ":~" << msg)
#else
#define NS_LOG_FUNCTION(msg)   \
  NS_LOG (LOG_FUNCTION, __FUNCTION__ << ":~" << msg)
#endif
//#define NS_LOG_FUNCTION_MSG(msg)   \
//  NS_LOG (LOG_FUNCTION, __FUNCTION__ << msg)

class Log {
public:
    ~Log();
    static int StartLog(int type, std::string file);
    static void SetLogLevel(LogLevel logLevel);
    static void SetLogActive(bool logActive) {
        m_logActive = logActive;
    }
    static void Close();
    static bool Write(const int index, const char* message, const LogLevel messageLogLevel);
    static bool Write(const char* message, const LogLevel messageLogLevel);
    static bool Write(const std::ostringstream& message, const LogLevel messageLogLevel) {
        return Write(message.str().c_str(), messageLogLevel);
    }
    static bool Write(const int index, const std::ostringstream& message, const LogLevel messageLogLevel) {
        return Write(index, message.str().c_str(), messageLogLevel);
    }
    static bool WriteLog(const char* message) {
        return Write(message, kLogLevelInfo);
    }
    static bool WriteLog(const std::ostringstream& message) {
        return WriteLog(message.str().c_str());
    }
    static bool WriteHeader(const int index, const char* message);

    static std::string toHex(const int i, std::streamsize numDigits = 0);

private:
    Log();

    static std::string GetTime();
    std::ofstream* getLog(const int index) const;
    bool hasType(const int index) const;
    std::string getLevelLabel(const enum LogLevel level) const;
    std::map<int, std::ofstream*> m_files;
    static bool m_logActive;
    static Log* m_instance;
};

#endif
