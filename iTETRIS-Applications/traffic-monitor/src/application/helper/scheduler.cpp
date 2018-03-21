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

#include "scheduler.h"

using namespace std;

namespace protocol
{
	namespace application
	{
		//  Event
		event_id Event::CurrentId = 0;

		Event::Event(double time, EventCallBack * callBack) :
				m_id(++CurrentId), m_time(time), m_callBack(callBack)
		{
		}

		Event::~Event()
		{
			delete m_callBack;
		}

		//  EventOrdering
		bool EventOrdering::operator()(const Event* left, const Event* right)
		{
			return left->m_time < right->m_time;
		}

		//  Scheduler
		multiset<Event*, EventOrdering> Scheduler::m_list;
		event_id Scheduler::m_currentInvoke = 0;
		double Scheduler::m_currentTime = 0;

		void Scheduler::Cancel(event_id & id)
		{
			if (id == 0)
				return;
			for (multiset<Event*>::iterator it = m_list.begin(); it != m_list.end(); ++it)
			{
				if ((*it)->m_id == id)
				{
					//Can't cancel the current invocation here. Will be removed at the end of the invocation by the Notify
					if (id != m_currentInvoke)
					{
						delete *it;
						m_list.erase(it);
					}
					//Set it to zero so subsequent calls will avoid entering the loop
					id = 0;
					return;
				}
			}
		}

		int Scheduler::Notify(int currentTime)
		{
			NS_LOG_INFO("[Scheduler] Notify time: " << currentTime << ". Current size: " << m_list.size());
			int number = 0;
			for (multiset<Event*>::iterator it = m_list.begin(); it != m_list.end();)
			{
				Event* e = *it;
				if (currentTime >= e->m_time)
				{
					//I need to save the id of the current invocation so I don't remove it in Cancel
					m_currentInvoke = e->m_id;
					m_currentTime = e->m_time;
					e->m_callBack->invoke();
					++number;
					delete e;
					m_list.erase(it++);
				} else
				{
					//The elements are ordered by time so I can exit the loop the first time the condition is not satisfied
					break;
				}
			}
			//Zero is invalid as Id
			m_currentInvoke = 0;
			m_currentTime = currentTime;
			NS_LOG_INFO("[Scheduler] " << number << " have been executed.");
			if (m_list.size() > 0)
			{
				Event* begin = *(m_list.begin());
				NS_LOG_INFO(
						"[" << currentTime << "][Scheduler] next at " << begin->m_time << " in " << (begin->m_time - currentTime));
			}
			return number;
		}

		bool Scheduler::IsRunning(event_id id)
		{
			if (id == 0)
				return false;
			for (multiset<Event*>::iterator it = m_list.begin(); it != m_list.end(); ++it)
			{
				if ((*it)->m_id == id)
				{
					return true;
				}
			}
			return false;
		}

		double Scheduler::GetCurrentTime()
		{
			return m_currentTime;
		}

	} /* namespace application */
} /* namespace protocol */
