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
#include "vector.h"
#include <cmath>
#include <sstream>

namespace baseapp {

namespace application {

Vector2D::Vector2D(double _x, double _y) :
    x(_x), y(_y) {
}

Vector2D::Vector2D() :
    x(0.0), y(0.0) {
}

double CalculateDistance(const Vector2D& a, const Vector2D& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double distance = std::sqrt(dx * dx + dy * dy);
    return distance;
}

std::ostream& operator <<(std::ostream& os, const Vector2D& vector) {
    os << vector.x << ":" << vector.y;
    return os;
}

std::istream& operator >>(std::istream& is, Vector2D& vector) {
    char c1;
    is >> vector.x >> c1 >> vector.y;
    if (c1 != ':') {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

} /* namespace application */
} /* namespace protocol */

