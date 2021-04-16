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

#ifndef SPLIT_H_
#define SPLIT_H_

namespace baseapp {

//struct split
//{
enum empties_t {
    empties_ok, no_empties
};
//};

template<typename Container>
Container& split(Container& result, const typename Container::value_type& s,
                 const typename Container::value_type& delimiters, /*split::*/empties_t empties = /*split::*/no_empties) {
    result.clear();
    size_t current;
    size_t next = -1;
    do {
        if (empties == /*split::*/  no_empties) {
            next = s.find_first_not_of(delimiters, next + 1);
            if (next == Container::value_type::npos) {
                break;
            }
            next -= 1;
        }
        current = next + 1;
        next = s.find_first_of(delimiters, current);
        result.push_back(s.substr(current, next - current));
    } while (next != Container::value_type::npos);
    return result;
}

} /* namespace protocol */
#endif /* SPLIT_H_ */
