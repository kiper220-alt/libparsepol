/*
 * libparsepol - POL Registry file parser
 *
 * Copyright (C) 2024 BaseALT Ltd.
 * Copyright (C) 2020 Korney Yakovlevich
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <cstring>
#include <fstream>
#include <iostream>

#include <parser.hpp>

void testCase1()
{
    std::ifstream file("../rsc/case1.pol", std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Can't open file `rsc/case1.pol`!" << std::endl;
        return;
    }

    auto parser = createPregParser();

    std::optional<PolicyFile> data = parser->parse(file);

    if (!data.has_value() || !data->body.has_value()) {
        std::cerr << "Can't parse file `rsc/case1.pol`!" << std::endl;
        return;
    }
    assert(std::get<std::string>(data->body->instructions[0].data) == std::string("'r[e]d'"));
}
