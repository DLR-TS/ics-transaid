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
#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <set>
#include <vector>
#include "current-time.h"
#include "log/log.h"

namespace baseapp
{
  namespace application
  {

    typedef unsigned event_id;

    class EventCallBack
    {
    public:
      virtual ~EventCallBack()
      {
      }
      virtual void invoke() = 0;
    };

    template<class Class, class Parameter = void>
    class EventCallBackImpl : public EventCallBack
    {
      typedef void (Class::*Method)(Parameter);
    public:
      EventCallBackImpl(Class* instance, Method method, Parameter arg)
      {
        m_instance = instance;
        m_method = method;
        m_arg = arg;
      }
      void invoke()
      {
        (m_instance->*m_method)(m_arg);
      }
    protected:
      Class* m_instance;
      Method m_method;
      Parameter m_arg;
    };

    template<class Class>
    class EventCallBackImpl<Class> : public EventCallBack
    {
      typedef void (Class::*Method)();
    public:
      EventCallBackImpl(Class* instance, Method method)
      {
        m_instance = instance;
        m_method = method;
      }
      void invoke()
      {
        (m_instance->*m_method)();
      }
    private:
      Class* m_instance;
      Method m_method;
    };

    class Event
    {
    public:
      Event(double time, EventCallBack * callBack);
      virtual ~Event();

      event_id m_id;
      double m_time;
      EventCallBack * m_callBack;
    private:
      static event_id CurrentId;
    };

    struct EventOrdering
    {
      bool operator()(const Event* left, const Event* right);
    };

    class Scheduler
    {
    public:
      static void Cancel(event_id & id);
      static int Notify(int currentParameterime);
      static bool IsRunning(event_id id);
      static double GetCurrentTime();
      template<typename Method, class Class>

      /// @brief this schedules the given event aftert the given time (in ms.) from the time of scheduling.
      static event_id Schedule(double time, Method function, Class* instance)
      {
        time += CurrentTime::Now() > 0 ? CurrentTime::Now() : 0;
        NS_LOG_INFO("Schedule at " << time);
        EventCallBack* cb = new EventCallBackImpl<Class>(instance, function);
        Event* e = new Event(time, cb);
        m_list.insert(e);
        return e->m_id;
      }
      template<typename Method, class Class, typename Parameter>
      static event_id Schedule(double time, Method function, Class* instance, Parameter arg)
      {
        time += CurrentTime::Now() > 0 ? CurrentTime::Now() : 0;
        NS_LOG_INFO("Schedule at " << time);
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
  } /* namespace application */
} /* namespace protocol */

#endif /* SCHEDULER_H_ */
