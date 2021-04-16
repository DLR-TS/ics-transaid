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

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

namespace baseapp {
namespace server {
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
class CircularBuffer {
public:
    CircularBuffer(unsigned short size) :
        m_size(size), m_currentIndex(0), m_firstTime(true) {
        m_buffer = new T[m_size];
    }

    virtual ~CircularBuffer() {
        delete[] m_buffer;
    }

    bool addValue(const T newValue, T& replacedValue) {
        bool result = !m_firstTime;
        if (result) {
            replacedValue = m_buffer[m_currentIndex];
        }
        insert(newValue);
        return result;
    }

    void push_front(const T value) {
        insert(value);
    }

    T at(const unsigned short index) const {
        unsigned short idx = (m_currentIndex - 1 - index + m_size) % m_size;
        return m_buffer[idx];
    }

    T front() const {
        return at(0);
    }

    unsigned short size() const {
        if (m_firstTime) {
            return m_currentIndex;
        }
        return m_size;
    }

    void clear() {
        m_currentIndex = 0;
        m_firstTime = true;
    }

private:
    T* m_buffer;
    unsigned short m_size;
    unsigned short m_currentIndex;
    bool m_firstTime;

    inline void insert(const T& value) {
        m_buffer[m_currentIndex++] = value;
        if (m_currentIndex == m_size) {
            m_currentIndex = 0;
            m_firstTime = false;
        }
    }
};

template<class T>
class CircularBuffer<T*> {
public:
    CircularBuffer(unsigned short size) :
        m_size(size), m_currentIndex(0), m_firstTime(true) {
        m_buffer = new T*[m_size];
        //Need to initialize at NULL so I can use the delete on any item even if it was not assigned
        for (unsigned short i = 0; i < m_size; ++i) {
            m_buffer[i] = NULL;
        }
    }

    virtual ~CircularBuffer() {
        deleteAll();
        delete m_buffer;
    }

    bool addValue(T* newValue, T*& replacedValue) {
        bool result = !m_firstTime;
        if (result) {
            replacedValue = m_buffer[m_currentIndex];
        }
        insert(newValue);
        return result;
    }

    void push_front(T* value) {
        delete m_buffer[m_currentIndex];
        insert(value);
    }

    T* at(const unsigned short index) const {
        unsigned short idx = (m_currentIndex - 1 - index + m_size) % m_size;
        return m_buffer[idx];
    }

    T* front() const {
        return at(0);
    }

    unsigned short size() const {
        if (m_firstTime) {
            return m_currentIndex;
        }
        return m_size;
    }

    void clear() {
        deleteAll();
        m_currentIndex = 0;
        m_firstTime = true;
    }

private:
    T** m_buffer;
    const unsigned short m_size;
    unsigned short m_currentIndex;
    bool m_firstTime;

    inline void deleteAll() {
        for (unsigned short i = 0; i < m_size; ++i) {
            delete m_buffer[i];
            m_buffer[i] = NULL;
        }
    }

    inline void insert(T* value) {
        m_buffer[m_currentIndex++] = value;
        if (m_currentIndex == m_size) {
            m_currentIndex = 0;
            m_firstTime = false;
        }
    }
};

} /* namespace server */
} /* namespace protocol */

#endif /* RING_BUFFER_H_ */
