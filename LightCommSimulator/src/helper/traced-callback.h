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
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006,2007 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
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
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#ifndef TRACED_CALLBACK_H
#define TRACED_CALLBACK_H

#include <vector>
#include "callback.h"
#include <iostream>
#include "log/log.h"

namespace protocol {
namespace application {

class TracedCallbackBase {
public:
    virtual ~TracedCallbackBase() {
    }
    virtual void Connect(const CallbackBase& callback) {
    }
    virtual void Disconnect(const CallbackBase& callback) {
    }
    virtual void SetName(const std::string& name) {
    }
};

/**
 * \brief forward calls to a chain of Callback
 * \ingroup tracing
 *
 * An ns3::TracedCallback has almost exactly the same API as a normal ns3::Callback but
 * instead of forwarding calls to a single function (as an ns3::Callback normally does),
 * it forwards calls to a chain of ns3::Callback. TracedCallback::Connect adds a ns3::Callback
 * at the end of the chain of callbacks. TracedCallback::Disconnect removes a ns3::Callback from
 * the chain of callbacks.
 */
template<typename T1 = empty, typename T2 = empty, typename T3 = empty, typename T4 = empty>
class TracedCallback: public TracedCallbackBase {
public:
    TracedCallback();
    virtual ~TracedCallback();
    /**
     * \param callback callback to add to chain of callbacks
     *
     * Append the input callback to the end of the internal list
     * of ns3::Callback.
     */
    void Connect(const CallbackBase& callback);
    /**
     * \param callback callback to remove from the chain of callbacks.
     *
     * Remove the input callback from the internal list
     * of ns3::Callback. This method is really the symmetric
     * of the TracedCallback::ConnectWithoutContext method.
     */
    void Disconnect(const CallbackBase& callback);
    void operator()(void) const;
    void operator()(T1 a1) const;
    void operator()(T1 a1, T2 a2) const;
    void operator()(T1 a1, T2 a2, T3 a3) const;
    void operator()(T1 a1, T2 a2, T3 a3, T4 a4) const;

    void SetName(const std::string& name);

private:
    typedef std::vector<Callback<void, T1, T2, T3, T4>*> CallbackList;
    CallbackList m_callbackList;
    std::string m_name;
};

} /* namespace application */

// implementation below.

namespace application {

template<typename T1, typename T2, typename T3, typename T4>
TracedCallback<T1, T2, T3, T4>::~TracedCallback() {
    for (typename CallbackList::const_iterator it = m_callbackList.begin(); it != m_callbackList.end(); ++it) {
        (*it)->Delete();
    }
    m_callbackList.clear();
}

template<typename T1, typename T2, typename T3, typename T4>
TracedCallback<T1, T2, T3, T4>::TracedCallback() :
    m_callbackList(), m_name("-") {
}
template<typename T1, typename T2, typename T3, typename T4>
void TracedCallback<T1, T2, T3, T4>::Connect(const CallbackBase& callback) {
    Callback<void, T1, T2, T3, T4>* cb = new Callback<void, T1, T2, T3, T4>();
    cb->Assign(callback);
    m_callbackList.push_back(cb);
}
template<typename T1, typename T2, typename T3, typename T4>
void TracedCallback<T1, T2, T3, T4>::Disconnect(const CallbackBase& callback) {
    for (typename CallbackList::iterator i = m_callbackList.begin(); i != m_callbackList.end(); /* empty */) {
        if ((*i)->IsEqual(callback)) {
            (*i)->Delete();
            i = m_callbackList.erase(i);
        } else {
            ++i;
        }
    }
}
template<typename T1, typename T2, typename T3, typename T4>
void TracedCallback<T1, T2, T3, T4>::operator()(void) const {
    NS_LOG_INFO("TracedCallback " << m_name << " number of calls " << m_callbackList.size());
    for (typename CallbackList::const_iterator i = m_callbackList.begin(); i != m_callbackList.end(); ++i) {
        (*(*i))();
    }
}
template<typename T1, typename T2, typename T3, typename T4>
void TracedCallback<T1, T2, T3, T4>::operator()(T1 a1) const {
    NS_LOG_INFO("TracedCallback " << m_name << " number of calls " << m_callbackList.size());
    for (typename CallbackList::const_iterator i = m_callbackList.begin(); i != m_callbackList.end(); i++) {
        (*i)->operator()(a1);
    }
}
template<typename T1, typename T2, typename T3, typename T4>
void TracedCallback<T1, T2, T3, T4>::operator()(T1 a1, T2 a2) const {
    NS_LOG_INFO("TracedCallback " << m_name << " number of calls " << m_callbackList.size());
    for (typename CallbackList::const_iterator i = m_callbackList.begin(); i != m_callbackList.end(); i++) {
        (*(*i))(a1, a2);
    }
}
template<typename T1, typename T2, typename T3, typename T4>
void TracedCallback<T1, T2, T3, T4>::operator()(T1 a1, T2 a2, T3 a3) const {
    NS_LOG_INFO("TracedCallback " << m_name << " number of calls " << m_callbackList.size());
    for (typename CallbackList::const_iterator i = m_callbackList.begin(); i != m_callbackList.end(); i++) {
        (*(*i))(a1, a2, a3);
    }
}
template<typename T1, typename T2, typename T3, typename T4>
void TracedCallback<T1, T2, T3, T4>::operator()(T1 a1, T2 a2, T3 a3, T4 a4) const {
    NS_LOG_INFO("TracedCallback " << m_name << " number of calls " << m_callbackList.size());
    for (typename CallbackList::const_iterator i = m_callbackList.begin(); i != m_callbackList.end(); i++) {
        (*(*i))(a1, a2, a3, a4);
    }
}
template<typename T1, typename T2, typename T3, typename T4>
void TracedCallback<T1, T2, T3, T4>::SetName(const std::string& name) {
    m_name = name;
}

} /* namespace application */
} /* namespace protocol */

#endif /* TRACED_CALLBACK_H */
