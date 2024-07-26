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
#ifndef PREGPARSER_PARSER
#define PREGPARSER_PARSER

#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <istream>
#include <memory>

enum class PolicyRegType {
    REG_NONE,
    /* Null-terminated-string */
    REG_SZ = 1,
    REG_EXPAND_SZ = 2,

    /* Any kind of binary data */
    REG_BINARY = 3,
    /* 32-bit number */
    REG_DWORD_LITTLE_ENDIAN = 4,

    /* 32-bit number in NBO format */
    REG_DWORD_BIG_ENDIAN = 5,

    /* A null-terminated Unicode string that contains the target path of a
     * symbolic link. */
    REG_LINK = 6,

    /* Sequence of null-terminated strings terminated by null-terminator */
    REG_MULTI_SZ = 7,
    REG_RESOURCE_LIST = 8,
    REG_FULL_RESOURCE_DESCRIPTOR = 9,
    REG_RESOURCE_REQUIREMENTS_LIST = 10,

    /* 64-bit number */
    REG_QWORD = 11,
    REG_QWORD_LITTLE_ENDIAN = 12,
};

typedef std::variant<std::string, std::vector<uint8_t>, uint32_t, uint64_t> PolicyData;

typedef struct PolicyInstruction
{
    std::string key{};
    std::string value{};
    PolicyRegType type{};
    PolicyData data{};
} PolicyInstruction;

typedef struct PolicyBody
{
    std::vector<PolicyInstruction> instructions{};
} PolicyBody;

typedef struct PolicyFile
{
    std::optional<PolicyBody> body{};
} PolicyFile;

class PRegParser
{
public:
    virtual PolicyFile parse(std::istream &stream) = 0;
    virtual bool write(const PolicyFile &file, const std::istream &stream) = 0;
    virtual ~PRegParser() = default;
};

std::unique_ptr<PRegParser> createPregParser();

#endif // PREGPARSER_PARSER
