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

#include "scheduler.h"

using namespace std;

namespace baseapp {
namespace application {
//  Event
event_id Event::CurrentId = 0;

Event::Event(double time, EventCallBack* callBack) :
    m_id(++CurrentId), m_time(time), m_callBack(callBack) {
}

Event::~Event() {
    delete m_callBack;
}

//  EventOrdering
bool EventOrdering::operator()(const Event* left, const Event* right) {
    return left->m_time < right->m_time;
}

//  Scheduler
multiset<Event*, EventOrdering> Scheduler::m_list;
event_id Scheduler::m_currentInvoke = 0;
double Scheduler::m_currentTime = 0;

void Scheduler::Cancel(event_id& id) {
    if (id == 0) {
        return;
    }
    for (multiset<Event*>::iterator it = m_list.begin(); it != m_list.end(); ++it) {
        if ((*it)->m_id == id) {
            //Can't cancel the current invocation here. Will be removed at the end of the invocation by the Notify
            if (id != m_currentInvoke) {
                delete *it;
                m_list.erase(it);
            }
            //Set it to zero so subsequent calls will avoid entering the loop
            id = 0;
            return;
        }
    }
}

int Scheduler::Notify(int currentTime) {
    NS_LOG_INFO("[Scheduler] Notify time: " << currentTime << ". Current size: " << m_list.size());
    int number = 0;
    for (multiset<Event*>::iterator it = m_list.begin(); it != m_list.end();) {
        Event* e = *it;
        if (currentTime >= e->m_time) {
            //I need to save the id of the current invocation so I don't remove it in Cancel
            m_currentInvoke = e->m_id;
            m_currentTime = e->m_time;
            e->m_callBack->invoke();
            ++number;
            delete e;
            m_list.erase(it++);
        } else {
            //The elements are ordered by time so I can exit the loop the first time the condition is not satisfied
            break;
        }
    }
    //Zero is invalid as Id
    m_currentInvoke = 0;
    m_currentTime = currentTime;
    NS_LOG_INFO("[Scheduler] " << number << " have been executed.");
    if (m_list.size() > 0) {
        Event* begin = *(m_list.begin());
        NS_LOG_INFO(
            "[" << currentTime << "][Scheduler] next at " << begin->m_time << " in " << (begin->m_time - currentTime));
    }
    return number;
}

bool Scheduler::IsRunning(event_id id) {
    if (id == 0) {
        return false;
    }
    for (multiset<Event*>::iterator it = m_list.begin(); it != m_list.end(); ++it) {
        if ((*it)->m_id == id) {
            return true;
        }
    }
    return false;
}

double Scheduler::GetCurrentTime() {
    return m_currentTime;
}

} /* namespace application */
} /* namespace protocol */
