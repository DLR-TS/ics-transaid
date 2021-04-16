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
#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <set>
#include <vector>
#include "current-time.h"
#include "log/log.h"

namespace lightcomm {
typedef unsigned event_id;

class EventCallBack {
public:
    virtual ~EventCallBack() {
    }
    virtual void invoke() = 0;
};

template<class Class, class Parameter = void>
class EventCallBackImpl : public EventCallBack {
    typedef void (Class::*Method)(Parameter);
public:
    EventCallBackImpl(Class* instance, Method method, Parameter arg) {
        m_instance = instance;
        m_method = method;
        m_arg = arg;
    }
    void invoke() {
        (m_instance->*m_method)(m_arg);
    }
protected:
    Class* m_instance;
    Method m_method;
    Parameter m_arg;
};

template<class Class>
class EventCallBackImpl<Class> : public EventCallBack {
    typedef void (Class::*Method)();
public:
    EventCallBackImpl(Class* instance, Method method) {
        m_instance = instance;
        m_method = method;
    }
    void invoke() {
        (m_instance->*m_method)();
    }
private:
    Class* m_instance;
    Method m_method;
};

class Event {
public:
    Event(double time, EventCallBack* callBack);
    virtual ~Event();

    event_id m_id;
    double m_time;
    EventCallBack* m_callBack;
private:
    static event_id CurrentId;
};

struct EventOrdering {
    bool operator()(const Event* left, const Event* right);
};

class Scheduler {
public:
    static void Cancel(event_id& id);
    static int Notify(int currentParameterime);
    static bool IsRunning(event_id id);
    static double GetCurrentTime();
    template<typename Method, class Class>
    static event_id Schedule(double time, Method function, Class* instance) {
        time += CurrentTime::Now() > 0 ? CurrentTime::Now() : 0;

        EventCallBack* cb = new EventCallBackImpl<Class>(instance, function);
        Event* e = new Event(time, cb);
        m_list.insert(e);
        return e->m_id;
    }
    template<typename Method, class Class, typename Parameter>
    static event_id Schedule(double time, Method function, Class* instance, Parameter arg) {
        time += CurrentTime::Now() > 0 ? CurrentTime::Now() : 0;

        EventCallBack* cb = new EventCallBackImpl<Class, Parameter>(instance, function, arg);
        Event* e = new Event(time, cb);
        m_list.insert(e);
        return e->m_id;
    }
private:
    static std::multiset<Event*, EventOrdering> m_list;
    static event_id m_currentInvoke;
    static double m_currentTime;

    Scheduler();
    ~Scheduler();
    static void updateAfterNotify();
};

} /* namespace lightcomm */

#endif /* SCHEDULER_H_ */
