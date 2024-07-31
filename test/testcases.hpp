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
#include <vector>

#include <parser.hpp>

void testCase(std::string filename)
{
    std::ifstream file("../rsc/" + filename, std::ios::in | std::ios::binary);
    std::stringstream stream;

    auto parser = createPregParser();
    auto pol = parser->parse(file);

    // After parsing file in fail state becouse reached end of file
    file.clear();

    file.seekg(0, std::ios::end);
    size_t length1 = static_cast<size_t>(file.tellg());
    size_t length2 = 0;
    file.seekg(0, std::ios::beg);

    parser->write(stream, pol);
    length2 = static_cast<size_t>(stream.tellp());

    std::vector<uint8_t> original = bufferToVector(file, length1);
    std::vector<uint8_t> rewrited = bufferToVector(stream, length2);

    if (original != rewrited) {
        std::cerr << "ERROR: `" << filename << "` is not rewrited correctly." << std::endl;
        std::cerr.flush();
        throw std::runtime_error("error: `" + filename + "` detect error in parser.");
    }
    std::cout << "`" << filename << "` is rewrite: OK" << std::endl;
}
