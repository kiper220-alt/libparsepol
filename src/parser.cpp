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
#include <iconv.h>
#include <parser.hpp>
#include <iconv.h>

#define must_present(target, source) \
    {                                \
        auto data = source;          \
        if (!data.has_value()) {     \
            return {};               \
        }                            \
        target = *data;              \
    }

#define check_stream(target) \
    {                        \
        if (target.fail()) { \
            return {};       \
        }                    \
    }


/*!
* \brief Valid POL Registery file header
*/
static const char valid_header[5] = { 0x50, 0x52, 0x65, 0x67, 0x01 };

// TODO: move to a separate file `bufferToUint16` `bufferToUint32` `bufferToUint32BE` and etc.
// TODO: Implement all the missing functions like `uint64Tobuffer`, `bufferToUint16BE` and etc.
static inline uint16_t bufferToUint16(const char *type_buffer)
{
    uint16_t num = static_cast<uint16_t>(static_cast<unsigned char>(type_buffer[1]) << 8
                                         | static_cast<unsigned char>(type_buffer[0]));
    return num;
}

static inline uint32_t bufferToUint32(const char *type_buffer)
{
    uint32_t num = static_cast<uint32_t>(static_cast<unsigned char>(type_buffer[3]) << 24
                                         | static_cast<unsigned char>(type_buffer[2]) << 16
                                         | static_cast<unsigned char>(type_buffer[1]) << 8
                                         | static_cast<unsigned char>(type_buffer[0]));
    return num;
}
static inline uint32_t bufferToUint32BE(const char *type_buffer)
{
    uint32_t num = static_cast<uint32_t>(static_cast<unsigned char>(type_buffer[0]) << 24
                                         | static_cast<unsigned char>(type_buffer[1]) << 16
                                         | static_cast<unsigned char>(type_buffer[2]) << 8
                                         | static_cast<unsigned char>(type_buffer[3]));
    return num;
}
static inline uint32_t bufferToUint64(const char *type_buffer)
{
    uint32_t num = static_cast<uint32_t>(static_cast<unsigned char>(type_buffer[7]) << 56
                                         | static_cast<unsigned char>(type_buffer[6]) << 48
                                         | static_cast<unsigned char>(type_buffer[5]) << 40
                                         | static_cast<unsigned char>(type_buffer[4]) << 32
                                         | static_cast<unsigned char>(type_buffer[3]) << 24
                                         | static_cast<unsigned char>(type_buffer[2]) << 16
                                         | static_cast<unsigned char>(type_buffer[1]) << 8
                                         | static_cast<unsigned char>(type_buffer[0]));
    return num;
}
static inline uint32_t bufferToUint64BE(const char *type_buffer)
{
    uint32_t num = static_cast<uint32_t>(static_cast<unsigned char>(type_buffer[0]) << 56
                                         | static_cast<unsigned char>(type_buffer[1]) << 48
                                         | static_cast<unsigned char>(type_buffer[2]) << 40
                                         | static_cast<unsigned char>(type_buffer[3]) << 32
                                         | static_cast<unsigned char>(type_buffer[4]) << 24
                                         | static_cast<unsigned char>(type_buffer[5]) << 16
                                         | static_cast<unsigned char>(type_buffer[6]) << 8
                                         | static_cast<unsigned char>(type_buffer[7]));
    return num;
}

/*!
 * \brief Check regex `(.{size})\]` and return first group as std::string
 */
static inline std::optional<std::string> getString(std::istream &stream, PolicyRegType type,
                                                   uint32_t size)
{
    std::string data;
    char buff[2];
    const uint8_t &sym = *buff;

    data.resize(size);

    stream.read(const_cast<char *>(data.c_str()), size);
    check_stream(stream);

    if (sym != ']') {
        return {};
    }

    return data;
}

/*!
 * \brief Check regex `(.{size})\]` and return first group as std::vector<uint8_t>
 */
static inline std::optional<std::vector<uint8_t>> getOctet(std::istream &stream, PolicyRegType type,
                                                           uint32_t size)
{
    std::vector<uint8_t> data;
    char buff[2];
    const uint8_t &sym = *buff;

    data.resize(size);

    stream.read(const_cast<char *>((char *)(data.data())), size);
    check_stream(stream);

    if (sym != ']') {
        return {};
    }

    return data;
}

/*!
 * \brief Check regex `(.{4})\]` and return first group as uint32_t
 */
static inline std::optional<uint32_t> getUint32(std::istream &stream, PolicyRegType type,
                                                           uint32_t size)
{
    char buff[6];
    const uint8_t &sym = buff[4];

    if (size != 4)
    {
        return {};
    }

    stream.read(buff, size);
    check_stream(stream);

    if (sym != ']') {
        return {};
    }

    return bufferToUint32(buff);
}

/*!
 * \brief Match regex `[\x20-\x7E]`
 */
static inline bool isValueCharacter(uint8_t sym)
{
    return sym >= 0x20 && sym <= 0x7E;
}

/*!
 * \brief Check regex `\x50\x52\x65\x67`
 */
static inline bool parseHeader(std::istream &stream)
{
    char header[5];
    const uint &signature = *header;
    const uint &normal_signature = *valid_header;

    stream.read(header, 5);
    check_stream(stream);

    return signature == normal_signature && header[4] == valid_header[4];
}

/*!
 * \brief Check regex `(.{4});` and return first group as uint32_t
 */
static inline std::optional<uint32_t> getSize(std::istream &stream)
{
    char data[6];
    const uint8_t &sym = data[5];

    stream.read(data, 6);
    check_stream(stream);

    uint32_t num = static_cast<uint32_t>(
            static_cast<unsigned char>(data[3]) << 24 | static_cast<unsigned char>(data[2]) << 16
            | static_cast<unsigned char>(data[1]) << 8 | static_cast<unsigned char>(data[0]));

    if (sym != ';') {
        return {};
    }

    return num;
}

/*!
 * \brief Check 32bit regex `([\x1\x2\x3\x4\x5\x6\x7\x8\x9\xA\xB\xC]);` and return first group as
 * Type
 */
static inline std::optional<PolicyRegType> getType(std::istream &stream)
{
    // identicaly to get size, but with convert to PolicyRegType.
    uint32_t num;
    must_present(num, getSize(stream));

    switch (static_cast<PolicyRegType>(num)) {
    case PolicyRegType::REG_SZ:
    case PolicyRegType::REG_EXPAND_SZ:
    case PolicyRegType::REG_BINARY:
    case PolicyRegType::REG_DWORD_LITTLE_ENDIAN:
    case PolicyRegType::REG_DWORD_BIG_ENDIAN:
    case PolicyRegType::REG_LINK:
    case PolicyRegType::REG_MULTI_SZ:
    case PolicyRegType::REG_RESOURCE_LIST:
    case PolicyRegType::REG_FULL_RESOURCE_DESCRIPTOR:
    case PolicyRegType::REG_RESOURCE_REQUIREMENTS_LIST:
    case PolicyRegType::REG_QWORD:
    case PolicyRegType::REG_QWORD_LITTLE_ENDIAN:
        break;
    default:
        return {};
    }

    return static_cast<PolicyRegType>(num);
}

/*!
 * \brief Matches regex `([\x20-\x7E]+);` and return first group as result
 */
static inline std::optional<std::string> getKeypath(std::istream &stream)
{
    std::string keyPath;
    char data[2];
    const uint8_t &sym = *data;

    // Key in specs [\x20-\x5B\x5D-\x7E](exclude '\'), when keypath include '\' like delimeter
    do {
        stream.read(data, 2);
        check_stream(stream);
    } while (sym >= 0x20 && sym <= 0x7E);

    if (sym != ';' || !keyPath.empty()) {
        return {};
    }
    return { keyPath };
}

/*!
 * \brief Matches regex `([\x20-\x7E]{1,259});` and return first group as result
 */
static inline std::optional<std::string> getValue(std::istream &stream)
{
    std::string result;

    // ValueCharacter symbols identical to KeyPath symbols
    must_present(result, getKeypath(stream));

    // Check maximum value length
    if (result.length() > 259) {
        return {};
    }

    return { std::move(result) };
}

/*!
 * \brief Matches ABNF `'[' KeyPath ';' Value ';' Type ';' Size ';' Data ']'` and return reduced
 * structure
 */
static inline std::optional<PolicyInstruction> getInstruction(std::istream &stream)
{
    PolicyInstruction instruction;
    char sym[2];
    uint32_t dataSize;

    stream.read(sym, 2);
    check_stream(stream);

    if (*sym != '[') {
        return {};
    }

    must_present(instruction.key, getKeypath(stream));
    must_present(instruction.value, getValue(stream));
    must_present(instruction.type, getType(stream));
    must_present(dataSize, getSize(stream));
}

PolicyFile parse(std::istream &stream)
{
    PolicyBody body;

    if (!parseHeader(stream)) {
        return {};
    }

    return { body };
}
bool write(const PolicyFile &file, std::istream &stream)
{
    return false;
}
