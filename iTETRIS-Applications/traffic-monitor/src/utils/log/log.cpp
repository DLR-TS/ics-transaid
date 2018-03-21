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
#include <time.h>
#include <cstdlib>
#include <sys/time.h>
#include "log.h"

using namespace std;

Log * Log::m_instance = NULL;
bool Log::m_logActive = false;

int Log::StartLog(int type, string path)
{
	if (m_instance == NULL)
		m_instance = new Log();
	ofstream* fs = new ofstream(path.c_str());
	m_instance->m_files.insert(make_pair(type, fs));
	m_logActive = true;
	return EXIT_SUCCESS;
}

Log::Log()
{
}

Log::~Log()
{
	for (std::map<int, std::ofstream*>::const_iterator it = m_files.begin(); it != m_files.end(); ++it)
	{
		it->second->close();
		delete it->second;
	}
	m_files.clear();
	m_instance = NULL;
}

void Log::Close()
{
	delete m_instance;
}

std::ofstream * Log::getLog(const int index) const
{
	return m_files.find(index)->second;
}

bool Log::hasType(const int index) const
{
	return m_files.find(index) != m_files.end();
}

string Log::getLevelLabel(const enum LogLevel level) const
{
	switch (level)
	{
	case LOG_ERROR:
	case kLogLevelError:
		return "[ERROR] ";
	case LOG_WARN:
	case kLogLevelWarning:
		// whitespace left at the end for aligment
		return "[WARN ] ";
	case LOG_DEBUG:
		return "[DEBUG] ";
	case LOG_INFO:
	case kLogLevelInfo:
		// whitespace left at the end for aligment
		return "[INFO ] ";
	case LOG_FUNCTION:
		return "[FUNCT] ";
	case LOG_LOGIC:
		return "[LOGIC] ";
	default:
		return "unknown";
	}
}

string Log::GetTime()
{
	struct timeval tim;
	gettimeofday(&tim, NULL);
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

bool Log::Write(const char* message, const LogLevel messageLogLevel)
{
	Write(0, message, messageLogLevel);
}

bool Log::Write(const int index, const char* message, const LogLevel messageLogLevel)
{
	if (!m_logActive)
		return false;
	if (!m_instance->hasType(index))
		return false;

	ofstream* fs = m_instance->getLog(index);

	if (!fs->good())
	{
		return false;
	}
	(*fs) << "[" << GetTime() << "] " << m_instance->getLevelLabel(messageLogLevel) << message << endl;
	//fs->flush();
	return true;
}

bool Log::WriteHeader(const int index, const char* message)
{
	if (!m_logActive)
		return false;
	if (!m_instance->hasType(index))
		return false;
	ofstream* fs = m_instance->getLog(index);
	if (!fs->good())
	{
		return false;
	}
	(*fs) << message << endl;
	(*fs) << "===============================" << endl;
	return true;
}
