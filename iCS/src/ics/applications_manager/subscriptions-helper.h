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
/****************************************************************************/
/// @file    subscriptions-helper.h
/// @author  Jerome Haerri, Pasquale Cataldi (EURECOM)
/// @date    December 15th, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

#ifndef SUBSCRIPTIONS_HELPER_H_
#define SUBSCRIPTIONS_HELPER_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include "../../utils/ics/iCStypes.h"
#include "foreign/tcpip/storage.h"
#include "../../utils/ics/geometric/Shapes.h"

namespace ics {

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SubscriptionsHelper
 * @brief Helper class for buffer manipulations within subscriptions
 */
class SubscriptionsHelper {
public:

    /**
     * @brief Reads a short (2-bytes).
     * @param[in] message Pointer to the message buffer from where to read the value.
     * @param[in] index Pointer to the variable that contains the starting index for the value to be read.
     * @return Short value.
     */
    static short readShort(std::vector<unsigned char>* m_msg, unsigned int* index);

    /**
     * @brief Reads an int (4-bytes).
     * @param[in] message Pointer to the message buffer from where to read the value.
     * @param[in] index Pointer to the variable that contains the starting index for the value to be read.
     * @return Int value.
     */
    static int readInt(std::vector<unsigned char>* m_msg, unsigned int* index);

    /**
     * @brief Reads a string, whose length is specified in the first read byte.
     * @param[in] message Pointer to the message buffer from where to read the value.
     * @param[in] index Pointer to the variable that contains the starting index for the value to be read.
     * @return String value.
     */
    static float readFloat(std::vector<unsigned char>* m_msg, unsigned int* index);

    /**
     * @brief Reads an float (4-bytes).
     * @param[in] message Pointer to the message buffer from where to read the value.
     * @param[in] index Pointer to the variable that contains the starting index for the value to be read.
     * @return Float value.
     */
    static std::string readString(std::vector<unsigned char>* m_msg, unsigned int* index);

    /**
     * @brief Converts the areas expressed as TLV in a data structure.
     * @param[in] message Pointer to the message buffer from where to read the value.
     * @param[in] index Pointer to the variable that contains the starting index for the value to be read.
     * @return Vector of area structures.
     */
    static std::vector<ics_types::TArea> readBlockAreas(std::vector<unsigned char>* m_msg, unsigned int* index);

    /**
     * @brief Converts the areas expressed as TLV in a data structure.
     * @param[in] message Pointer to the message buffer from where to read the value.
     * @return Vector of area structures.
     */
    static std::vector<ics_types::TArea> readBlockAreas(tcpip::Storage* m_msg);

    /**
     * @brief Helper function for reinterpretCast for Floating values
     * @param[in] Float floating value to be cast to a vector of char
     * @return std::vector<unsigned char> vector of chars
     */
    static std::vector<unsigned char> reinterpretFloat(float value);

    static std::vector<unsigned char> reinterpretDouble(double value);
    static std::vector<unsigned char> reinterpretInt(int value);
    static std::vector<unsigned char> reinterpreteShort(short value);
    static std::vector<unsigned char> reinterpreteString(const std::string s);

    static std::vector<unsigned char> writeByEndianess(const unsigned char* begin, unsigned int size);

    static bool checkPointInsideCircle(const ics_types::Circle circle, const ics_types::Point2D point);
    static bool checkCanReceiveGeoBroadcast(const ics_types::Circle circleA, const ics_types::Circle circleB,
                                            const ics_types::Point2D point);
};

}
#endif /* SUBSCRIPTIONS_HELPER_H_ */
