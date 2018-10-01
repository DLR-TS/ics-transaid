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

#include "payload.h"
#include "current-time.h"
#include "log/log.h"

namespace testapp
{
namespace server
{

int Payload::PAYLOAD_ID = 0;

Payload::Payload(int size) :
    m_size(size)
{
  m_id = ++PAYLOAD_ID;
  m_timeStep = CurrentTime::Now();
}

Payload::Payload(int id, int size) :
    m_id(id), m_size(size)
{
  m_timeStep = CurrentTime::Now();
}

Payload::Payload(int id, int size, int timeStep) :
    m_id(id), m_timeStep(timeStep), m_size(size)
{}

Payload::~Payload()
{
  for (std::list<application::Header *>::const_iterator it = m_headerList.begin(); it != m_headerList.end(); ++it)
    delete *it;
  m_headerList.clear();
}

int Payload::size() const
{
  int size = m_size;
  for (std::list<application::Header *>::const_iterator it = m_headerList.begin(); it != m_headerList.end(); ++it)
    size += (*it)->GetSerializedSize();
  return size;
}

void Payload::addHeader(application::Header * header)
{
  m_headerList.push_front(header);
}

void Payload::getHeader(application::Header * &header, Position position) const
{
  if (position == PAYLOAD_FRONT)
    header = m_headerList.front();
  else
    header = m_headerList.back();
}

application::Header * Payload::getHeader(Position position) const
{
  if (position == PAYLOAD_FRONT)
    return m_headerList.front();
  else
    return m_headerList.back();
}

} /* namespace server */
} /* namespace protocol */
