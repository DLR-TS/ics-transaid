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

#include "payload-storage.h"
#include "../utils/log/log.h"
#include <sstream>

namespace protocol
{
namespace server
{

using namespace std;

PayloadStorage::PayloadStorage()
{
  m_counter = 0;
}

PayloadStorage::~PayloadStorage()
{
  if (m_storage.size() > 0)
  {
    for (map<unsigned, const Payload*>::const_iterator it = m_storage.begin(); it != m_storage.end(); ++it)
      delete (it->second);
    m_storage.clear();
  }
}

string PayloadStorage::insert(const Payload* payload, const StoragePolicy policy)
{
  m_storage.insert(make_pair(++m_counter, payload));
  return createKey(policy);
}

bool PayloadStorage::find(const string & key, Payload *& payload)
{
  map<unsigned, const Payload*>::iterator it = m_storage.find(getIndex(key));
  if (it == m_storage.end())
    return false;
  payload = (Payload *) it->second;
  if (asPolicy(key) == kDeleteOnRead)
    m_storage.erase(it);
  return true;
}

bool PayloadStorage::erase(const std::string & key)
{
  return m_storage.erase(getIndex(key));
}

bool PayloadStorage::eraseAndDelete(const string & key)
{
  map<unsigned, const Payload*>::iterator it = m_storage.find(getIndex(key));
  if (it == m_storage.end())
    return false;
  delete (it->second);
  m_storage.erase(it);
  return true;
}

bool PayloadStorage::hasKey(const string & key) const
{
  return m_storage.count(getIndex(key));
}

int PayloadStorage::expiredPayloadCleanUp(const int oldTimeStep)
{
  int number = 0;
  ostringstream log;
  log << "ExpiredPayloadCleanUp for time: " << oldTimeStep << ". Removing keys: ";
  for (map<unsigned, const Payload*>::iterator it = m_storage.begin(); it != m_storage.end();)
  {
    if (it->second->getTimeStep() <= oldTimeStep)
    {
      log << it->first << " ";
      ++number;
      delete (it->second);
      m_storage.erase(it++);
    }
    else
      ++it;
  }
  if (number == 0)
    log << "noting to remove ";
  log << "Current size: " << m_storage.size();
  Log::WriteLog(log);
  return number;
}

unsigned PayloadStorage::getIndex(const string & key) const
{
  unsigned index;
  istringstream(key.substr(1, key.size())) >> index;
  return index;
}

string PayloadStorage::createKey(const StoragePolicy policy) const
{
  ostringstream key;
  key << asChar(policy) << m_counter;
  return key.str();
}

char PayloadStorage::asChar(const StoragePolicy policy) const
{
  return (char) policy;
}

StoragePolicy PayloadStorage::asPolicy(const string & key) const
{
  switch (key[0])
  {
  case 'd':
    return kDeleteOnRead;
  case 'm':
    return kMultipleRead;
  default:
    return kDeleteOnRead;
  }
}

} /* namespace server */
} /* namespace protocol */
