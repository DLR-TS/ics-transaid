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
 * Author Vasilios Karagounis
  ***************************************************************************************/
#ifndef JSON_READER_H
#define JSON_READER_H

//or add "-I$PWD/$srcdir/../utils/json/include" or "-I$PWD/$srcdir/../utils/json/single_include" in AM_CPPFLAGS in configure
#include "../../../utils/json/single_include/nlohmann/json.hpp"

#include <string>
#include <sstream>

using json = nlohmann::json;

class JsonReader {
public:

    static int Open(const std::string& _name);
    static void Close();

    static json& get() {
        return object;
    }

private:
    JsonReader();
    ~JsonReader();

    static json object;
    static JsonReader* m_instance;
};

#endif //JSON_READER_H
