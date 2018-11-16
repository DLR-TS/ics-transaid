/****************************************************************************/
/// @file    ics-log.cpp
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

//#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#else
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#endif
#include <sstream>
#include <cstdlib>
#include <iomanip>
//#include <sys/time.h>

#include "ics-log.h"
#include "../../../ics/sync-manager.h"
#include <utils/common/TplConvert.h>
//#include "../../common/TplConvert.h"

using namespace std;

namespace ics
{

IcsLog* IcsLog::instance_ = 0;
long IcsLog::lineCounter_ = 0;
ics_types::icstime_t IcsLog::gLogStart=-1;
ics_types::icstime_t IcsLog::gLogEnd=-1;
bool IcsLog::gOmitSystemTime=false;
ics::LogLevel IcsLog::logLevel_;
long IcsLog::timeStepThreshold_;
long IcsLog::nextTimeStepThreshold_;
int IcsLog::currentNumberLogFiles_ = 0;
string name, ext;

IcsLog::IcsLog(string path, string timeThreshold, ics_types::icstime_t logStart, ics_types::icstime_t logEnd, bool omitSysTime)
{
  path_ = path;
  string::size_type pointPos = path.rfind(".");
  if (pointPos != string::npos)
  {
    ext = path.substr(pointPos);
    name = path.substr(0, pointPos);
  }
  else
  {
    name = path;
    ext = "";
  }
  myfile_.open(path.c_str());
  logLevel_ = kLogLevelInfo;
  IcsLog::SetLogTimeThreshold(timeThreshold.c_str());
  //Using ms instead of seconds for the timestep
  timeStepThreshold_ *= 1000;
  nextTimeStepThreshold_ = timeStepThreshold_ - 1;
  gLogStart = logStart;
  gLogEnd = logEnd;
  gOmitSystemTime = omitSysTime;
}

int IcsLog::StartLog(string path, string timeThreshold, ics_types::icstime_t logStart, ics_types::icstime_t logEnd, bool omitSysTime)
{
  if (instance_ == 0)
  {
    instance_ = new IcsLog(path, timeThreshold, logStart, logEnd, omitSysTime);
    return EXIT_SUCCESS;
  }

  return -1;
}

void IcsLog::SetLogLevel(ics::LogLevel logLevel)
{
  logLevel_ = logLevel;
}

void IcsLog::SetLogTimeThreshold(char* logThreshold)
{
  try
  {
    timeStepThreshold_ = TplConvert::_2long(logThreshold);
  } catch (EmptyData e)
  {
    cout << "[INFO] Log time files maximum timestep set to 200." << endl;
    timeStepThreshold_ = 200;
  }
}

void IcsLog::SetLogTimeThreshold(string logThreshold)
{
  try
  {
    timeStepThreshold_ = TplConvert::_2long(logThreshold.c_str());
  } catch (EmptyData e)
  {
    cout << "[INFO] Log time files maximum timestep set to 200." << endl;
    timeStepThreshold_ = 200;
  }
}

void IcsLog::Close()
{
  instance_->myfile_.close();
  delete instance_;
}

string getTime()
{
  struct timeval tim;
#ifdef WIN32
    static const unsigned __int64 epoch = ((unsigned __int64) 116444736000000000ULL);
    FILETIME    file_time;
    SYSTEMTIME  system_time;
    ULARGE_INTEGER ularge;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    ularge.LowPart = file_time.dwLowDateTime;
    ularge.HighPart = file_time.dwHighDateTime;

    tim.tv_sec = (long) ((ularge.QuadPart - epoch) / 10000000L);
    tim.tv_usec = (long) (system_time.wMilliseconds * 1000);
#else
  gettimeofday(&tim, NULL);
#endif
  char buffer[21];
  strftime(buffer, 21, "%Y-%m-%dT%H:%M:%S.", localtime((time_t*) &tim.tv_sec));
  string mytime(buffer);
  long milli = tim.tv_usec / 1000;
  ostringstream oss;
  if (milli < 10)
    oss << "00";
  else if (milli < 100)
    oss << "0";
  oss << milli;
  return mytime += oss.str();
}

string getTimeStep()
{
  ostringstream oss;
  oss << (SyncManager::m_simStep / 1000) << ",";
  int milli = SyncManager::m_simStep % 1000;
  if (milli < 10)
    oss << "00";
  else if (milli < 100)
    oss << "0";
  oss << milli;
  return oss.str();
}

bool IcsLog::Log(const char* message)
{
  // check the time step and create a new file if necessary
  if (timeStepThreshold_ != 0 && SyncManager::m_simStep == nextTimeStepThreshold_)
  {
    nextTimeStepThreshold_ += timeStepThreshold_;
    instance_->StartNewFile();
  }

  if (!instance_->myfile_.good())
  {
    return false;
  }
  /*time_t rawtime;
   struct tm * timeinfo;
   time(&rawtime);
   timeinfo = localtime(&rawtime);

   if (timeinfo == NULL)
   {
   cout << "[WARNING] Impossible to get time from system." << endl;
   return false;
   }

   // Removes \0 character
   stringstream auxi;
   auxi.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
   auxi << asctime(timeinfo);
   string mytime;
   try
   {
   getline(auxi, mytime);
   } catch (stringstream::failure e)
   {
   cout << "[WARNING] Impossible to format system time." << endl;
   return false;
   }*/

  if (isActive()) {
      if (!gOmitSystemTime) {
          instance_->myfile_ << "[" << getTime() << "] ";
      }
      instance_->myfile_ << "[" << getTimeStep() << "] " << message << endl;
      lineCounter_++;
  }

  return true;
}

bool
IcsLog::isActive() {
    return SyncManager::m_simStep > gLogStart && (gLogEnd == -1 || SyncManager::m_simStep < gLogEnd);
}

bool IcsLog::LogLevel(const char* message, ics::LogLevel messageLogLevel)
{
  // check the time step and create a new file if necessary
  if (timeStepThreshold_ != 0 && SyncManager::m_simStep >= nextTimeStepThreshold_)
  {
    nextTimeStepThreshold_ += timeStepThreshold_;
    instance_->StartNewFile();
  }

  // decides if the message will be written or not
  bool write = false;
  string levelName;

  // If level set to INFO write ALL messages
  if (logLevel_ == kLogLevelInfo)
  {
    write = true;
    levelName = "[INFO] ";
  }

  // If level set to WARNING write except if message leve is is INFO
  if (logLevel_ == kLogLevelWarning && messageLogLevel != kLogLevelInfo)
  {
    write = true;
    levelName = "[WARNING] ";
  }

  // If level set to ERROR and message IS ERROR write
  if ((logLevel_ == kLogLevelError) && (messageLogLevel == kLogLevelError))
  {
    write = true;
    levelName = "[ERROR] ";
  }

  if (!write)
    return true; // Writing is canceled

  if (!instance_->myfile_.good())
  {
    return false;
  }
  /*time_t rawtime;
   struct tm * timeinfo;
   time(&rawtime);
   timeinfo = localtime(&rawtime);

   if (timeinfo == NULL)
   {
   cout << "[WARNING] Impossible to get time from system." << endl;
   return false;
   }

   // Removes \0 character
   stringstream auxi;
   auxi.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
   auxi << asctime(timeinfo);
   string mytime;
   try
   {
   getline(auxi, mytime);
   } catch (stringstream::failure e)
   {
   cout << "[WARNING] Impossible to format system time." << endl;
   return false;
   }*/

#ifdef LOG_ON
  if (isActive()) {
      if (!gOmitSystemTime) {
          instance_->myfile_ << "[" << getTime() << "] ";
      }
      instance_->myfile_ << "[" << getTimeStep() << "] " << levelName << message << endl;
  }
#endif

  lineCounter_++;

  return true;
}

string IcsLog::GetPath()
{
  return instance_->path_;
}

int IcsLog::StartNewFile()
{
  instance_->myfile_.close();
  currentNumberLogFiles_++;
  string counter = utils::Conversion::int2String(currentNumberLogFiles_);
  string auxiPath = name + "-" + counter + ext;
  instance_->myfile_.open(auxiPath.c_str());
  return EXIT_SUCCESS;
}

string
IcsLog::toHex(const int i, streamsize numDigits) {
    // taken from http://stackoverflow.com/questions/5100718/int-to-hex-string-in-c
    stringstream stream;
    stream << "0x" << setfill('0') << setw(numDigits == 0 ? sizeof(int) * 2 : numDigits) << hex << i;
    return stream.str();
}

}
