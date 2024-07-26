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
static inline uint16_t bufferToUint16(const char *type_buffer)
{
    auto buffer = reinterpret_cast<const unsigned char *>(type_buffer);
    uint16_t num = static_cast<uint16_t>(buffer[1] << 8 | buffer[0]);
    return num;
}

/*!
* \brief Convert binary data to uint16_t with invert byte order
*/
static inline uint16_t bufferToUint16BE(const char *type_buffer)
{
    auto buffer = reinterpret_cast<const unsigned char *>(type_buffer);
    uint16_t num = static_cast<uint16_t>(buffer[0] << 8 | buffer[1]);
    return num;
}

/*!
* \brief Convert binary data to uint32_t
*/
static inline uint32_t bufferToUint32(const char *type_buffer)
{
    auto buffer = reinterpret_cast<const unsigned char *>(type_buffer);
    uint32_t num =
            static_cast<uint32_t>(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
    return num;
}

/*!
* \brief Convert binary data to uint32_t with invert byte order
*/
static inline uint32_t bufferToUint32BE(const char *type_buffer)
{
    auto buffer = reinterpret_cast<const unsigned char *>(type_buffer);
    uint32_t num =
            static_cast<uint32_t>(buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3]);
    return num;
}

/*!
* \brief Convert binary data to uint64_t
*/
static inline uint64_t bufferToUint64(const char *type_buffer)
{
    auto buffer = reinterpret_cast<const unsigned char *>(type_buffer);
    uint32_t num = static_cast<uint32_t>(buffer[7] << 56 | buffer[6] << 48 | buffer[5] << 40
                                         | buffer[4] << 32 | buffer[3] << 24 | buffer[2] << 16
                                         | buffer[1] << 8 | buffer[0]);
    return num;
}

/*!
* \brief Convert binary data to uint64_t with invert byte order
*/
static inline uint64_t bufferToUint64BE(const char *type_buffer)
{
    auto buffer = reinterpret_cast<const unsigned char *>(type_buffer);
    uint32_t num = static_cast<uint32_t>(buffer[0] << 56 | buffer[1] << 48 | buffer[2] << 40
                                         | buffer[3] << 32 | buffer[4] << 24 | buffer[5] << 16
                                         | buffer[6] << 8 | buffer[7]);
    return num;
}
