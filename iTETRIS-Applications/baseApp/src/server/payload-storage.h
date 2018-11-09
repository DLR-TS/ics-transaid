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

#ifndef MESSAGESTORAGE_H_
#define MESSAGESTORAGE_H_

#include <map>
#include <string>
#include "payload.h"

namespace baseapp
{
namespace server
{

enum StoragePolicy
{
  kDeleteOnRead = (int) 'd', kMultipleRead = (int) 'm'
};

class PayloadStorage
{
public:
  PayloadStorage();
  virtual ~PayloadStorage();

  std::string insert(const Payload* payload, const StoragePolicy policy = kDeleteOnRead);
  bool find(const std::string & key, Payload * & payload);
  bool erase(const std::string & key);
  bool eraseAndDelete(const std::string & key);
  bool hasKey(const std::string & key) const;
  int expiredPayloadCleanUp(const int oldTimeStep);
  StoragePolicy asPolicy(const std::string & key) const;
  int size() const
  {
    return m_storage.size();
  }

private:
  std::string createKey(const StoragePolicy policy) const;
  char asChar(const StoragePolicy policy) const;
  unsigned getIndex(const std::string & key) const;

private:
  std::map<unsigned, const Payload*> m_storage;
  unsigned m_counter;
};

} /* namespace server */
} /* namespace protocol */

#endif /* MESSAGESTORAGE_H_ */
