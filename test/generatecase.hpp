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
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "./testcases.hpp"
#include <encoding.hpp>
#include <parser.hpp>

std::string generateRandomKey(size_t length)
{
    std::string key;
    key.resize(length);
    for (size_t i = 0; i < length; ++i) {
        // [0x20-\x5B] | [\x5D-\x7E]
        key[i] = (rand() % 0x5F) + 0x20;
        key[i] = key[i] >= 0x5c ? key[i] + 1 : key[i];
    }
    return key;
}

std::string generateRandomKeypath()
{
    std::string keyPath;
    keyPath += generateRandomKey((rand() % 99) + 1);

    while ((rand() % 5) >= 3) {
        keyPath += '\\';
        keyPath += generateRandomKey((rand() % 99) + 1);
    }

    return keyPath;
}

std::string generateRandomValue()
{
    std::string value;
    value.resize((rand() % 99) + 1);
    for (size_t i = 0; i < value.size(); ++i) {
        value[i] = (rand() % 0x5E) + 0x20;
    }
    return value;
}

PolicyRegType generateRandomType()
{
    switch (rand() % 7) {
    case 0:
        return PolicyRegType::REG_BINARY;
    case 1:
        return PolicyRegType::REG_DWORD_LITTLE_ENDIAN;
    case 2:
        return PolicyRegType::REG_DWORD_BIG_ENDIAN;
    case 3:
        return PolicyRegType::REG_QWORD_LITTLE_ENDIAN;
    case 4:
        return PolicyRegType::REG_QWORD_BIG_ENDIAN;
    case 5:
        return PolicyRegType::REG_SZ;
    case 6:
        return PolicyRegType::REG_MULTI_SZ;
    default:
        break;
    }
    return PolicyRegType::REG_BINARY;
}

PolicyData generateRandomData(PolicyRegType type)
{
    iconv_t conv = iconv_open("UTF-8", "UTF-32LE");
    if (conv == (decltype(conv))-1) {
        return {};
    }

    switch (type) {
    case PolicyRegType::REG_NONE:
        return {};
    case PolicyRegType::REG_SZ: {
        std::basic_string<char32_t> data;
        data.resize(rand() % 100);
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = (rand() % 0x5E) + 0x20;
        }
        return convert<char, char32_t>(data, conv);
    }

    case PolicyRegType::REG_MULTI_SZ: {
        std::vector<std::string> data1;
        size_t count = rand() % 100;
        for (size_t i = 0; i < count; ++i) {
            std::basic_string<char32_t> data;
            data.resize((rand() % 100) + 1);
            for (size_t i = 0; i < data.size(); ++i) {
                data[i] = (rand() % 0x5E) + 0x20;
            }
            data1.push_back(convert<char, char32_t>(data, conv));
        }
        return data1;
    }

    case PolicyRegType::REG_BINARY: {
        std::vector<uint8_t> data;
        size_t count = rand() % 100;
        for (size_t i = 0; i < count; ++i) {
            data.push_back((rand() % 255) + 1);
        }
        return data;
    }

    case PolicyRegType::REG_DWORD_LITTLE_ENDIAN:
        return uint32_t(rand() % 10'000'000);
    case PolicyRegType::REG_DWORD_BIG_ENDIAN:
        return uint32_t(rand() % 10'000'000);
    case PolicyRegType::REG_QWORD_LITTLE_ENDIAN:
        return uint64_t(rand() % 10'000'000);
    case PolicyRegType::REG_QWORD_BIG_ENDIAN:
        return uint64_t(rand() % 10'000'000);
    default:
        return {};
    }
}

void generateCase(size_t last)
{
    auto parser = createPregParser();
    size_t current = 0;
    // set seed
    srand(time(0));

    while (current <= last) {
        std::stringstream file;
        auto parser = createPregParser();

        // Generate case
        PolicyFile data;
        data.body = std::make_optional<PolicyBody>();
        size_t el = rand() % 100; // random number between 0 and 100
        for (size_t i = 0; i < el; i++) {
            PolicyInstruction instruction;
            instruction.key = generateRandomKeypath();
            instruction.value = generateRandomValue();
            instruction.type = generateRandomType();
            instruction.data = generateRandomData(instruction.type);
            data.body->instructions.push_back(instruction);
        }

        parser->write(file, data);
        file.seekg(0, std::ios::beg);

        auto test = parser->parse(file);
        if (!equal(data, test)) {
            std::cerr << "error: one of generated files detect error in parser." << std::endl;
            assert(0);
        }

        std::cout << "Generated " << current << " case: OK" << std::endl;

        ++current;
    }
}
