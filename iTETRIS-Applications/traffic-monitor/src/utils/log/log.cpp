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
#include <time.h>
#include <cstdlib>
#include <sys/time.h>
#include "log.h"

using namespace std;

Log* Log::m_instance = NULL;
bool Log::m_logActive = false;

int Log::StartLog(int type, string path) {
    if (m_instance == NULL) {
        m_instance = new Log();
    }
    ofstream* fs = new ofstream(path.c_str());
    m_instance->m_files.insert(make_pair(type, fs));
    m_logActive = true;
    return EXIT_SUCCESS;
}

Log::Log() {
}

Log::~Log() {
    for (std::map<int, std::ofstream*>::const_iterator it = m_files.begin(); it != m_files.end(); ++it) {
        it->second->close();
        delete it->second;
    }
    m_files.clear();
    m_instance = NULL;
}

void Log::Close() {
    delete m_instance;
}

std::ofstream* Log::getLog(const int index) const {
    return m_files.find(index)->second;
}

bool Log::hasType(const int index) const {
    return m_files.find(index) != m_files.end();
}

string Log::getLevelLabel(const enum LogLevel level) const {
    switch (level) {
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

string Log::GetTime() {
    struct timeval tim;
    gettimeofday(&tim, NULL);
    char buffer[21];
    strftime(buffer, 21, "%Y-%m-%dT%H:%M:%S.", localtime((time_t*) &tim.tv_sec));
    string mytime(buffer);
    long milli = tim.tv_usec / 1000;
    ostringstream oss;
    if (milli < 10) {
        oss << "00";
    } else if (milli < 100) {
        oss << "0";
    }
    oss << milli;
    return mytime += oss.str();
}

bool Log::Write(const char* message, const LogLevel messageLogLevel) {
    Write(0, message, messageLogLevel);
}

bool Log::Write(const int index, const char* message, const LogLevel messageLogLevel) {
    if (!m_logActive) {
        return false;
    }
    if (!m_instance->hasType(index)) {
        return false;
    }

    ofstream* fs = m_instance->getLog(index);

    if (!fs->good()) {
        return false;
    }
    (*fs) << "[" << GetTime() << "] " << m_instance->getLevelLabel(messageLogLevel) << message << endl;
    //fs->flush();
    return true;
}

bool Log::WriteHeader(const int index, const char* message) {
    if (!m_logActive) {
        return false;
    }
    if (!m_instance->hasType(index)) {
        return false;
    }
    ofstream* fs = m_instance->getLog(index);
    if (!fs->good()) {
        return false;
    }
    (*fs) << message << endl;
    (*fs) << "===============================" << endl;
    return true;
}
