/************************************************************************
 ** This file is part of the network simulator Shawn.                  **
 ** Copyright (C) 2004-2007 by the SwarmNet (www.swarmnet.de) project  **
 ** Shawn is free software; you can redistribute it and/or modify it   **
 ** under the terms of the BSD License. Refer to the shawn-licence.txt **
 ** file in the root of the Shawn source tree for further details.     **
 ************************************************************************
 **                                                                    **
 ** \author Axel Wegener <wegener@itm.uni-luebeck.de>                  **
 ** \author Bjoern Hendriks <hendriks@ibr.cs.tu-bs.de>                 **
 **                                                                    **
 ************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "storage-ns3.h"

#ifdef BUILD_TCPIP

#include <iostream>
#include <iterator>
#include <sstream>
#include <cassert>


using namespace std;

//#define NULLITER static_cast<list<unsigned char>::iterator>(0)

namespace ics
{

// ----------------------------------------------------------------------
StorageNs3::StorageNs3()
{
    init();
}


// ----------------------------------------------------------------------
StorageNs3::StorageNs3(unsigned char packet[], int length)
{
    // Length is calculated, if -1, or given
    if (length == -1) length = sizeof(packet) / sizeof(unsigned char);

    store.reserve(length);
    // Get the content
    for (int i = 0; i < length; ++i) store.push_back(packet[i]);

    init();
}


// ----------------------------------------------------------------------
void StorageNs3::init()
{
    // Initialize local variables
    iter_ = store.begin();

    short a = 0x0102;
    unsigned char *p_a = reinterpret_cast<unsigned char*>(&a);
    bigEndian_ = (p_a[0] == 0x01); // big endian?
}


// ----------------------------------------------------------------------
StorageNs3::~StorageNs3() {}


// ----------------------------------------------------------------------
bool StorageNs3::valid_pos()
{
    return (iter_ != store.end());   // this implies !store.empty()
}


// ----------------------------------------------------------------------
unsigned int StorageNs3::position() const
{
    // According to C++ standard std::distance will simply compute the iterators
    // difference for random access iterators as std::vector provides.
    return std::distance(store.begin(), iter_);
}


// ----------------------------------------------------------------------
void StorageNs3::reset()
{
    store.clear();
    iter_ = store.begin();
}


// ----------------------------------------------------------------------
/**
* Reads a char form the array
* @return The read char (between 0 and 255)
*/
unsigned char StorageNs3::readChar() throw(std::invalid_argument)
{
    if (!valid_pos()) {
        throw std::invalid_argument("StorageNs3::readChar(): invalid position");
    }
    return readCharUnsafe();
}


// ----------------------------------------------------------------------
/**
*
*/
void StorageNs3::writeChar(unsigned char value) throw()
{
    store.push_back(value);
    iter_ = store.begin();
}


// ----------------------------------------------------------------------
/**
* Reads a byte form the array
* @return The read byte (between -128 and 127)
*/
int StorageNs3::readByte()	throw(std::invalid_argument)
{
    int i = static_cast<int>(readChar());
    if (i < 128) return i;
    else return (i - 256);
}


// ----------------------------------------------------------------------
/**
*
*/
void StorageNs3::writeByte(int value) throw(std::invalid_argument)
{
    if (value < -128 || value > 127) {
        throw std::invalid_argument("StorageNs3::writeByte(): Invalid value, not in [-128, 127]");
    }
    writeChar(static_cast<unsigned char>((value+256) % 256));
}


// ----------------------------------------------------------------------
/**
* Reads an unsigned byte form the array
* @return The read byte (between 0 and 255)
*/
int StorageNs3::readUnsignedByte()	throw(std::invalid_argument)
{
    return static_cast<int>(readChar());
}


// ----------------------------------------------------------------------
/**
*
*/
void StorageNs3::writeUnsignedByte(int value) throw(std::invalid_argument)
{
    if (value < 0 || value > 255) {
        throw std::invalid_argument("StorageNs3::writeUnsignedByte(): Invalid value, not in [0, 255]");
    }
    writeChar(static_cast<unsigned char>(value));
}


// -----------------------------------------------------------------------
/**
* Reads a string form the array
* @return The read string
*/
std::string StorageNs3::readString() throw(std::invalid_argument)
{
    int len = readInt();
    checkReadSafe(len);
    StorageType::const_iterator end = iter_;
    std::advance(end, len);
    const string tmp(iter_, end);
    iter_ = end;
    return tmp;
}


// ----------------------------------------------------------------------
/**
* Writes a string into the array;
* @param s		The string to be written
*/
void StorageNs3::writeString(const std::string &s) throw()
{
    writeInt(static_cast<int>(s.length()));

    store.insert(store.end(), s.begin(), s.end());
    iter_ = store.begin();
}


// -----------------------------------------------------------------------
/**
* Reads a string list form the array
* @return The read string
*/
std::vector<std::string> StorageNs3::readStringList() throw(std::invalid_argument)
{
    std::vector<std::string> tmp;
    const int len = readInt();
    tmp.reserve(len);
    for (int i = 0; i < len; i++) {
        tmp.push_back(readString());
    }
    return tmp;
}

/**
* Reads a int list form the array
* @return The read int list
*/
std::vector<int> StorageNs3::readIntList() throw(std::invalid_argument)
{
    std::vector<int> tmp;
    const int len = readInt();
    tmp.reserve(len);
    for (int i = 0; i < len; i++) {
        tmp.push_back(readInt());
    }
    return tmp;
}

/**
* Reads a float list form the array
* @return The read float list
*/
std::vector<float> StorageNs3::readFloatList() throw(std::invalid_argument)
{
    std::vector<float> tmp;
    const int len = readInt();
    tmp.reserve(len);
    for (int i = 0; i < len; i++) {
        tmp.push_back(readFloat());
    }
    return tmp;
}

// ----------------------------------------------------------------------
/**
* Writes a string into the array;
* @param s		The string to be written
*/
void StorageNs3::writeStringList(const std::vector<std::string> &s) throw()
{
    writeInt(static_cast<int>(s.size()));
    for (std::vector<std::string>::const_iterator it = s.begin(); it!=s.end() ; it++) {
        writeString(*it);
    }
}

/**
* Writes a int into the array;
* @param s		The int list to be written
*/
void StorageNs3::writeIntList(const std::vector<int> &s) throw()
{
    writeInt(static_cast<int>(s.size()));
    for (std::vector<int>::const_iterator it = s.begin(); it!=s.end() ; it++) {
        writeInt(*it);
    }
}

/**
* Writes a float into the array;
* @param s		The float list to be written
*/
void StorageNs3::writeFloatList(const std::vector<float> &s) throw()
{
    writeInt(static_cast<int>(s.size()));
    for (std::vector<float>::const_iterator it = s.begin(); it!=s.end() ; it++) {
        writeFloat(*it);
    }
}


// ----------------------------------------------------------------------
/**
* Restores an integer, which was split up in two bytes according to the
* specification, it must have been split by its row byte representation
* with MSBF-order
*
* @return the unspoiled integer value (between -32768 and 32767)
*/
int StorageNs3::readShort() throw(std::invalid_argument)
{
    short value = 0;
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    readByEndianess(p_value, 2);
    return value;
}


// ----------------------------------------------------------------------
void StorageNs3::writeShort(int value) throw(std::invalid_argument)
{
    if (value < -32768 || value > 32767) {
        throw std::invalid_argument("StorageNs3::writeShort(): Invalid value, not in [-32768, 32767]");
    }

    short svalue = static_cast<short>(value);
    unsigned char *p_svalue = reinterpret_cast<unsigned char*>(&svalue);
    writeByEndianess(p_svalue, 2);
}


// ----------------------------------------------------------------------
/**
* restores an integer, which was split up in four bytes acording to the
* specification, it must have been split by its row byte representation
* with MSBF-order
*
* @return the unspoiled integer value (between -2.147.483.648 and 2.147.483.647)
*/
int StorageNs3::readInt() throw(std::invalid_argument)
{
    int value = 0;
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    readByEndianess(p_value, 4);
    return value;
}


// ----------------------------------------------------------------------
void StorageNs3::writeInt(int value) throw()
{
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    writeByEndianess(p_value, 4);
}


// ----------------------------------------------------------------------
/**
* restores a float , which was split up in four bytes acording to the
* specification, it must have been split by its row byte representation
* with MSBF-order
*
* @return the unspoiled float value
*/
float StorageNs3::readFloat() throw(std::invalid_argument)
{
    float value = 0;
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    readByEndianess(p_value, 4);
    return value;
}


// ----------------------------------------------------------------------
void StorageNs3::writeFloat(float value) throw()
{
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    writeByEndianess(p_value, 4);
}


// ----------------------------------------------------------------------
void StorageNs3::writeDouble(double value) throw()
{
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    writeByEndianess(p_value, 8);
}


// ----------------------------------------------------------------------
double StorageNs3::readDouble() throw(std::invalid_argument)
{
    double value = 0;
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    readByEndianess(p_value, 8);
    return value;
}


// ----------------------------------------------------------------------
void StorageNs3::writePacket(unsigned char* packet, int length)
{
    store.insert(store.end(), &(packet[0]), &(packet[length]));
    iter_ = store.begin();   // reserve() invalidates iterators
}


// ----------------------------------------------------------------------
void StorageNs3::writeStorageNs3(StorageNs3& other)
{
    // the compiler cannot deduce to use a const_iterator as source
    store.insert<StorageType::const_iterator>(store.end(), other.iter_, other.store.end());
    iter_ = store.begin();
}


// ----------------------------------------------------------------------
void StorageNs3::checkReadSafe(unsigned int num) const  throw(std::invalid_argument)
{
    if (std::distance(iter_, store.end()) < num) {
        std::ostringstream msg;
        msg << "tcpip::Storage::readIsSafe: want to read "  << num << " bytes from Storage, "
        << "but only " << std::distance(iter_, store.end()) << " remaining";
        throw std::invalid_argument(msg.str());
    }
}


// ----------------------------------------------------------------------
unsigned char StorageNs3::readCharUnsafe()
{
    char hb = *iter_;
    ++iter_;
    return hb;
}


// ----------------------------------------------------------------------
void StorageNs3::writeByEndianess(const unsigned char * begin, unsigned int size)
{
    const unsigned char * end = &(begin[size]);
    if (bigEndian_)
        store.insert(store.end(), begin, end);
    else
        store.insert(store.end(), std::reverse_iterator<const unsigned char *>(end), std::reverse_iterator<const unsigned char *>(begin));
    iter_ = store.begin();
}


// ----------------------------------------------------------------------
void StorageNs3::readByEndianess(unsigned char * array, int size)
{
    checkReadSafe(size);
    if (bigEndian_) {
        for (int i = 0; i < size; ++i)
            array[i] = readCharUnsafe();
    } else {
        for (int i = size - 1; i >= 0; --i)
            array[i] = readCharUnsafe();
    }
}

}

#endif // BUILD_TCPIP

/*-----------------------------------------------------------------------
 * Source  $Source: $
 * Version $Revision: 426 $
 * Date    $Date: 2010-05-31 12:05:13 +0200 (lun, 31 may 2010) $
 *-----------------------------------------------------------------------
 * $Log: $
 *-----------------------------------------------------------------------*/
