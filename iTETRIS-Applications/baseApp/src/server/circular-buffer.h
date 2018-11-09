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

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

namespace baseapp
{
namespace server
{
// stand-alone helper struct to check if a template is a pointer
//template<typename T>
//struct is_pointer
//{
//  static const bool value = false;
//};
//template<typename T>
//struct is_pointer<T*>
//{
//  static const bool value = true;
//};

// implemented here because otherwise I have to include the cpp file....
template<class T>
class CircularBuffer
{
public:
  CircularBuffer(unsigned short size) :
      m_size(size), m_currentIndex(0), m_firstTime(true)
  {
    m_buffer = new T[m_size];
  }

  virtual ~CircularBuffer()
  {
    delete m_buffer;
  }

  bool addValue(const T newValue, T & replacedValue)
  {
    bool result = !m_firstTime;
    if (result)
      replacedValue = m_buffer[m_currentIndex];
    insert(newValue);
    return result;
  }

  void push_front(const T value)
  {
    insert(value);
  }

  T at(const unsigned short index) const
  {
    unsigned short idx = (m_currentIndex - 1 - index + m_size) % m_size;
    return m_buffer[idx];
  }

  T front() const
  {
    return at(0);
  }

  unsigned short size() const
  {
    if (m_firstTime)
      return m_currentIndex;
    return m_size;
  }

  void clear()
  {
    m_currentIndex = 0;
    m_firstTime = true;
  }

private:
  T * m_buffer;
  unsigned short m_size;
  unsigned short m_currentIndex;
  bool m_firstTime;

  inline void insert(const T & value)
  {
    m_buffer[m_currentIndex++] = value;
    if (m_currentIndex == m_size)
    {
      m_currentIndex = 0;
      m_firstTime = false;
    }
  }
};

template<class T>
class CircularBuffer<T*>
{
public:
  CircularBuffer(unsigned short size) :
      m_size(size), m_currentIndex(0), m_firstTime(true)
  {
    m_buffer = new T*[m_size];
    //Need to initialize at NULL so I can use the delete on any item even if it was not assigned
    for (unsigned short i = 0; i < m_size; ++i)
      m_buffer[i] = NULL;
  }

  virtual ~CircularBuffer()
  {
    deleteAll();
    delete m_buffer;
  }

  bool addValue(T* newValue, T* & replacedValue)
  {
    bool result = !m_firstTime;
    if (result)
      replacedValue = m_buffer[m_currentIndex];
    insert(newValue);
    return result;
  }

  void push_front(T* value)
  {
    delete m_buffer[m_currentIndex];
    insert(value);
  }

  T* at(const unsigned short index) const
  {
    unsigned short idx = (m_currentIndex - 1 - index + m_size) % m_size;
    return m_buffer[idx];
  }

  T* front() const
  {
    return at(0);
  }

  unsigned short size() const
  {
    if (m_firstTime)
      return m_currentIndex;
    return m_size;
  }

  void clear()
  {
    deleteAll();
    m_currentIndex = 0;
    m_firstTime = true;
  }

private:
  T ** m_buffer;
  const unsigned short m_size;
  unsigned short m_currentIndex;
  bool m_firstTime;

  inline void deleteAll()
  {
    for (unsigned short i = 0; i < m_size; ++i)
    {
      delete m_buffer[i];
      m_buffer[i] = NULL;
    }
  }

  inline void insert(T* value)
  {
    m_buffer[m_currentIndex++] = value;
    if (m_currentIndex == m_size)
    {
      m_currentIndex = 0;
      m_firstTime = false;
    }
  }
};

} /* namespace server */
} /* namespace protocol */

#endif /* RING_BUFFER_H_ */
