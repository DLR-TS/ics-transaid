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

#ifndef NS3_VECTOR_H
#define NS3_VECTOR_H

#include <iostream>

namespace baseapp {
namespace application {

class Vector2D {
public:
    /**
     * \param _x x coordinate of vector
     * \param _y y coordinate of vector
     *
     * Create vector (_x, _y)
     */
    Vector2D(double _x, double _y);
    /**
     * Create vector vector (0.0, 0.0)
     */
    Vector2D();
    /**
     * x coordinate of vector
     */
    double x;
    /**
     * y coordinate of vector
     */
    double y;
};

/**
 * \param a one point
 * \param b another point
 * \returns the cartesian distance between a and b.
 */
double CalculateDistance(const Vector2D& a, const Vector2D& b);

std::ostream& operator <<(std::ostream& os, const Vector2D& vector);
std::istream& operator >>(std::istream& is, Vector2D& vector);

} /* namespace application */
} /* namespace protocol */

#endif /* NS3_VECTOR_H */
