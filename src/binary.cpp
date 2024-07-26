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
#include <binary.hpp>

/*!
 * \brief Convert binary data to uint16_t
 */
static inline uint16_t bufferToUint16(const char *buffer)
{
    auto _buffer = reinterpret_cast<const unsigned char *>(buffer);
    uint16_t num = static_cast<uint16_t>(_buffer[1] << 8 | _buffer[0]);
    return num;
}

/*!
 * \brief Convert binary data to uint16_t with invert byte order
 */
static inline uint16_t bufferToUint16BE(const char *buffer)
{
    auto _buffer = reinterpret_cast<const unsigned char *>(buffer);
    uint16_t num = static_cast<uint16_t>(_buffer[0] << 8 | _buffer[1]);
    return num;
}

/*!
 * \brief Convert binary data to uint32_t
 */
static inline uint32_t bufferToUint32(const char *buffer)
{
    auto _buffer = reinterpret_cast<const unsigned char *>(buffer);
    uint32_t num = static_cast<uint32_t>(_buffer[3] << 24 | _buffer[2] << 16 | _buffer[1] << 8
                                         | _buffer[0]);
    return num;
}

/*!
 * \brief Convert binary data to uint32_t with invert byte order
 */
static inline uint32_t bufferToUint32BE(const char *buffer)
{
    auto _buffer = reinterpret_cast<const unsigned char *>(buffer);
    uint32_t num = static_cast<uint32_t>(_buffer[0] << 24 | _buffer[1] << 16 | _buffer[2] << 8
                                         | _buffer[3]);
    return num;
}

/*!
 * \brief Convert binary data to uint64_t
 */
static inline uint64_t bufferToUint64(const char *buffer)
{
    auto _buffer = reinterpret_cast<const unsigned char *>(buffer);
    uint32_t num = static_cast<uint32_t>(_buffer[7] << 56 | _buffer[6] << 48 | _buffer[5] << 40
                                         | _buffer[4] << 32 | _buffer[3] << 24 | _buffer[2] << 16
                                         | _buffer[1] << 8 | _buffer[0]);
    return num;
}

/*!
 * \brief Convert binary data to uint64_t with invert byte order
 */
static inline uint64_t bufferToUint64BE(const char *buffer)
{
    auto _buffer = reinterpret_cast<const unsigned char *>(buffer);
    uint32_t num = static_cast<uint32_t>(_buffer[0] << 56 | _buffer[1] << 48 | _buffer[2] << 40
                                         | _buffer[3] << 32 | _buffer[4] << 24 | _buffer[5] << 16
                                         | _buffer[6] << 8 | _buffer[7]);
    return num;
}

/*!
 * \brief Convert uint16_t to binary data
 */
static inline void uint16ToBuffer(char *buffer, uint16_t data)
{
    auto _buffer = reinterpret_cast<unsigned char *>(buffer);
    _buffer[0] = data & 0xFF;
    _buffer[1] = (data >> 8) & 0xFF;
}

/*!
 * \brief Convert uint16_t to binary data with invert byte order
 */
static inline void uint16BEToBuffer(char *buffer, uint16_t data)
{
    auto _buffer = reinterpret_cast<unsigned char *>(buffer);
    _buffer[1] = data & 0xFF;
    _buffer[0] = (data >> 8) & 0xFF;
}

/*!
 * \brief Convert uint32_t to binary data
 */
static inline void uint32ToBuffer(char *buffer, uint32_t data)
{
    auto _buffer = reinterpret_cast<unsigned char *>(buffer);
    _buffer[0] = data & 0xFF;
    _buffer[1] = (data >> 8) & 0xFF;
    _buffer[2] = (data >> 16) & 0xFF;
    _buffer[3] = (data >> 24) & 0xFF;
}

/*!
 * \brief Convert uint32_t to binary data with invert byte order
 */
static inline void uint32BEToBuffer(char *buffer, uint32_t data)
{
    auto _buffer = reinterpret_cast<unsigned char *>(buffer);
    _buffer[3] = data & 0xFF;
    _buffer[2] = (data >> 8) & 0xFF;
    _buffer[1] = (data >> 16) & 0xFF;
    _buffer[0] = (data >> 24) & 0xFF;
}

/*!
 * \brief Convert uint64_t to binary data
 */
static inline void uint64ToBuffer(char *buffer, uint64_t data)
{
    auto _buffer = reinterpret_cast<unsigned char *>(buffer);
    _buffer[0] = data & 0xFF;
    _buffer[1] = (data >> 8) & 0xFF;
    _buffer[2] = (data >> 16) & 0xFF;
    _buffer[3] = (data >> 24) & 0xFF;
    _buffer[4] = (data >> 32) & 0xFF;
    _buffer[5] = (data >> 40) & 0xFF;
    _buffer[6] = (data >> 48) & 0xFF;
    _buffer[7] = (data >> 56) & 0xFF;
}

/*!
 * \brief Convert uint64_t to binary data with invert byte order
 */
static inline void uint64BEToBuffer(char *buffer, uint64_t data)
{
    auto _buffer = reinterpret_cast<unsigned char *>(buffer);
    _buffer[7] = data & 0xFF;
    _buffer[6] = (data >> 8) & 0xFF;
    _buffer[5] = (data >> 16) & 0xFF;
    _buffer[4] = (data >> 24) & 0xFF;
    _buffer[3] = (data >> 32) & 0xFF;
    _buffer[2] = (data >> 40) & 0xFF;
    _buffer[1] = (data >> 48) & 0xFF;
    _buffer[0] = (data >> 56) & 0xFF;
}
