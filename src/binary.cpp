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
#include <common.hpp>
#include <binary.hpp>
#include <iostream>

/*!
 * \brief Get string from istream (binary)
 */
std::optional<std::string> bufferToString(std::istream &buffer, size_t size, iconv_t conv)
{
    return {};
}

/*!
 * \brief Get uint16_t from istream (binary)
 */
std::optional<uint16_t> bufferToUint16(std::istream &buffer)
{
    unsigned char tmp[2];

    buffer.read(reinterpret_cast<char *>(tmp), 2);
    check_stream(buffer);

    uint16_t num = static_cast<uint16_t>(tmp[1] << 8 | tmp[0]);
    return num;
}

/*!
 * \brief Get uint16_t from istream (binary) (invert byte order)
 */
std::optional<uint16_t> bufferToUint16BE(std::istream &buffer)
{
    unsigned char tmp[2];

    buffer.read(reinterpret_cast<char *>(tmp), 2);
    check_stream(buffer);

    uint16_t num = static_cast<uint16_t>(tmp[0] << 8 | tmp[1]);
    return num;
}

/*!
 * \brief Get uint32_t from istream (binary)
 */
std::optional<uint32_t> bufferToUint32(std::istream &buffer)
{
    unsigned char tmp[4];

    buffer.read(reinterpret_cast<char *>(tmp), 4);
    check_stream(buffer);

    uint32_t num = static_cast<uint32_t>(tmp[3] << 24 | tmp[2] << 16 | tmp[1] << 8 | tmp[0]);
    return num;
}

/*!
 * \brief Get uint32_t from istream (binary) (invert byte order)
 */
std::optional<uint32_t> bufferToUint32BE(std::istream &buffer)
{
    unsigned char tmp[4];

    buffer.read(reinterpret_cast<char *>(tmp), 4);
    check_stream(buffer);

    uint32_t num = static_cast<uint32_t>(tmp[0] << 24 | tmp[1] << 16 | tmp[2] << 8 | tmp[3]);
    return num;
}

/*!
 * \brief Get uint64_t from istream (binary)
 */
std::optional<uint64_t> bufferToUint64(std::istream &buffer)
{
    unsigned char tmp[4];

    buffer.read(reinterpret_cast<char *>(tmp), 8);
    check_stream(buffer);

    uint64_t num = static_cast<uint32_t>(uint64_t(tmp[7]) << 56 | uint64_t(tmp[6]) << 48
                                         | uint64_t(tmp[5]) << 40 | uint64_t(tmp[4]) << 32
                                         | uint64_t(tmp[3]) << 24 | uint64_t(tmp[2]) << 16
                                         | uint64_t(tmp[1]) << 8 | uint64_t(tmp[0]));
    return num;
}

/*!
 * \brief Get uint64_t from istream (binary) (invert byte order)
 */
std::optional<uint64_t> bufferToUint64BE(std::istream &buffer)
{
    unsigned char tmp[4];

    buffer.read(reinterpret_cast<char *>(tmp), 8);
    check_stream(buffer);

    uint64_t num = static_cast<uint32_t>(uint64_t(tmp[0]) << 56 | uint64_t(tmp[1]) << 48
                                         | uint64_t(tmp[2]) << 40 | uint64_t(tmp[3]) << 32
                                         | uint64_t(tmp[4]) << 24 | uint64_t(tmp[5]) << 16
                                         | uint64_t(tmp[6]) << 8 | uint64_t(tmp[7]));
    return num;
}

/*!
 * \brief Put uint16_t into ostream (binary)
 */
bool uint16ToBuffer(std::ostream &buffer, uint16_t data)
{
    unsigned char tmp[2];
    tmp[0] = data & 0xFF;
    tmp[1] = (data >> 8) & 0xFF;

    buffer.write(reinterpret_cast<const char *>(tmp), 2);
    check_stream_bool(buffer);
    return true;
}

/*!
 * \brief Put uint16_t into ostream (binary) (invert byte order)
 */
bool uint16BEToBuffer(std::ostream &buffer, uint16_t data)
{
    unsigned char tmp[2];
    tmp[1] = data & 0xFF;
    tmp[0] = (data >> 8) & 0xFF;

    buffer.write(reinterpret_cast<const char *>(tmp), 2);
    check_stream_bool(buffer);
    return true;
}

/*!
 * \brief Put uint32_t into ostream (binary)
 */
bool uint32ToBuffer(std::ostream &buffer, uint32_t data)
{
    unsigned char tmp[4];
    tmp[0] = data & 0xFF;
    tmp[1] = (data >> 8) & 0xFF;
    tmp[2] = (data >> 16) & 0xFF;
    tmp[3] = (data >> 24) & 0xFF;

    buffer.write(reinterpret_cast<const char *>(tmp), 4);
    check_stream_bool(buffer);
    return true;
}

/*!
 * \brief Put uint32_t into ostream (binary) (invert byte order)
 */
bool uint32BEToBuffer(std::ostream &buffer, uint32_t data)
{
    unsigned char tmp[4];
    tmp[3] = data & 0xFF;
    tmp[2] = (data >> 8) & 0xFF;
    tmp[1] = (data >> 16) & 0xFF;
    tmp[0] = (data >> 24) & 0xFF;

    buffer.write(reinterpret_cast<const char *>(tmp), 4);
    check_stream_bool(buffer);
    return true;
}

/*!
 * \brief Put uint64_t into ostream (binary)
 */
bool uint64ToBuffer(std::ostream &buffer, uint64_t data)
{
    unsigned char tmp[8];
    tmp[0] = data & 0xFF;
    tmp[1] = (data >> 8) & 0xFF;
    tmp[2] = (data >> 16) & 0xFF;
    tmp[3] = (data >> 24) & 0xFF;
    tmp[4] = (data >> 32) & 0xFF;
    tmp[5] = (data >> 40) & 0xFF;
    tmp[6] = (data >> 48) & 0xFF;
    tmp[7] = (data >> 56) & 0xFF;

    buffer.write(reinterpret_cast<const char *>(tmp), 8);
    check_stream_bool(buffer);
    return true;
}

/*!
 * \brief Put uint64_t into ostream (binary) (invert byte order)
 */
bool uint64BEToBuffer(std::ostream &buffer, uint64_t data)
{
    unsigned char tmp[8];
    tmp[7] = data & 0xFF;
    tmp[6] = (data >> 8) & 0xFF;
    tmp[5] = (data >> 16) & 0xFF;
    tmp[4] = (data >> 24) & 0xFF;
    tmp[3] = (data >> 32) & 0xFF;
    tmp[2] = (data >> 40) & 0xFF;
    tmp[1] = (data >> 48) & 0xFF;
    tmp[0] = (data >> 56) & 0xFF;

    buffer.write(reinterpret_cast<const char *>(tmp), 8);
    check_stream_bool(buffer);
    return true;
}
