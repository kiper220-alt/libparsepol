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

void testCase1()
{
    std::ifstream file("../rsc/case1.pol", std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "can't open file `rsc/case1.pol`!" << std::endl;
        return;
    }

    auto parser = createPregParser();

    PolicyFile data = parser->parse(file);

    if (!data.body.has_value()) {
        std::cerr << "parser is invalid: can't parse file `rsc/case1.pol`" << std::endl;
        std::cerr << "parser is invalid: internal error" << std::endl;
        std::cerr.flush();
        assert(0);
    }
    if (data.body->instructions[0].key != std::string("Software\\BaseALT\\Policies\\gsettings")) {
        std::cerr << "parser is invalid: can't parse file `rsc/case1.pol`" << std::endl;
        std::cerr << "parser is invalid: invalid instruction KeyPath" << std::endl;
        std::cerr.flush();
        assert(0);
    }
    if (data.body->instructions[0].value != std::string("org.mate.background.secondary-color")) {
        std::cerr << "parser is invalid: can't parse file `rsc/case1.pol`" << std::endl;
        std::cerr << "parser is invalid: invalid instruction Value" << std::endl;
        std::cerr.flush();
        assert(0);
    }
    if (data.body->instructions[0].type != PolicyRegType::REG_SZ) {
        std::cerr << "parser is invalid: can't parse file `rsc/case1.pol`" << std::endl;
        std::cerr << "parser is invalid: invalid instruction size" << std::endl;
        std::cerr.flush();
        assert(0);
    }
    if (std::get<std::string>(data.body->instructions[0].data) != std::string("'r[e]d'")) {
        std::cerr << "parser is invalid: can't parse file `rsc/case1.pol`" << std::endl;
        std::cerr << "parser is invalid: invalid instruction data" << std::endl;
        std::cerr.flush();
        assert(0);
    }

    std::cerr << "read from file `rsc/case1.pol`: OK" << std::endl;
}

bool equal(const PolicyFile &a, const PolicyFile &b)
{
    if (!a.body.has_value() || !b.body.has_value()) {
        return false;
    }
    if (b.body->instructions.size() != a.body->instructions.size()) {
        std::cerr << "error: `" << a.body->instructions.size() << "` != `"
                  << b.body->instructions.size() << "`" << std::endl;
        return false;
    }

    for (size_t i = 0; i < a.body->instructions.size(); i++) {
        if (a.body->instructions[i].key != b.body->instructions[i].key) {
            std::cerr << "error: `" << a.body->instructions[i].key << "` != `"
                      << b.body->instructions[i].key << "`" << std::endl;
            return false;
        }
        if (a.body->instructions[i].value != b.body->instructions[i].value) {
            std::cerr << "error: `" << a.body->instructions[i].value << "` != `"
                      << b.body->instructions[i].value << "`" << std::endl;
            return false;
        }
        if (a.body->instructions[i].type != b.body->instructions[i].type) {
            std::cerr << "error: `" << static_cast<int>(a.body->instructions[i].type) << "` != `"
                      << static_cast<int>(b.body->instructions[i].type) << "`" << std::endl;
            return false;
        }
        if (a.body->instructions[i].data != b.body->instructions[i].data) {
            return false;
        }
    }

    return true;
}

void testCase2()
{
    std::stringstream stream;
    PolicyFile pol1 = { .body{ PolicyBody{
                       { PolicyInstruction{ "Test\\Path", "value1", PolicyRegType::REG_SZ,
                                            std::string("'r[e]d'") },
                         PolicyInstruction{ "Test\\Path", "value1", PolicyRegType::REG_SZ,
                                            std::string("Привет Мир!") },
                         PolicyInstruction{ "Test\\Path", "value1",
                                            PolicyRegType::REG_DWORD_LITTLE_ENDIAN,
                                            uint32_t(123321) },
                         PolicyInstruction{ "Test\\Path", "value1", PolicyRegType::REG_MULTI_SZ,
                                            std::vector<std::string>({ "a", "b", "c" }) } } } } },
               pol2;

    auto parser = createPregParser();

    if (!parser->write(stream, pol1)) {
        std::cerr << "parser is invalid: can't write to stream" << std::endl;
        return;
    }

    pol2 = parser->parse(stream);

    if (!equal(pol1, pol2)) {
        std::cerr << "parser is invalid: error in parser or serializator" << std::endl;
        assert(0);
    }

    std::cerr << "test case 2: OK" << std::endl;
}

void testCase3()
{
    std::ifstream file("../rsc/case2.pol", std::ios::in | std::ios::binary);

    auto parser = createPregParser();
    auto pol = parser->parse(file);
    std::cout << pol.body->instructions[3].key << std::endl;
}
