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
#ifndef LOG_H
#define LOG_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

enum LogLevel
{
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

class Log
{
public:
  ~Log();
  static int StartLog(int type, std::string file);
  static void SetLogLevel(LogLevel logLevel);
  static void SetLogActive(bool logActive)
  {
    m_logActive = logActive;
  }
  static void Close();
  static bool Write(const int index, const char* message, const LogLevel messageLogLevel);
  static bool Write(const char* message, const LogLevel messageLogLevel);
  static bool Write(const std::ostringstream & message, const LogLevel messageLogLevel)
  {
    return Write(message.str().c_str(), messageLogLevel);
  }
  static bool Write(const int index, const std::ostringstream & message, const LogLevel messageLogLevel)
  {
    return Write(index, message.str().c_str(), messageLogLevel);
  }
  static bool WriteLog(const char* message)
  {
    return Write(message, kLogLevelInfo);
  }
  static bool WriteLog(const std::ostringstream & message)
  {
    return WriteLog(message.str().c_str());
  }
  static bool WriteHeader(const int index, const char* message);

  static std::string toHex(const int i, std::streamsize numDigits = 0);

private:
  Log();

  static std::string GetTime();
  std::ofstream * getLog(const int index) const;
  bool hasType(const int index) const;
  std::string getLevelLabel(const enum LogLevel level) const;
  std::map<int, std::ofstream*> m_files;
  static bool m_logActive;
  static Log * m_instance;
};

#endif
