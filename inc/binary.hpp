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

#ifndef PREGPARSER_BINARY
#define PREGPARSER_BINARY

#include <iconv.h>
#include <optional>
#include <cinttypes>

/*!
 * \brief Get uint16_t from istream (binary)
 */
static inline std::optional<uint16_t> bufferToUint16(std::istream &buffer);

/*!
 * \brief Get uint16_t from istream (binary) (invert byte order)
 */
static inline std::optional<uint16_t> bufferToUint16BE(std::istream &buffer);

/*!
 * \brief Get uint32_t from istream (binary)
 */
static inline std::optional<uint32_t> bufferToUint32(std::istream &buffer);

/*!
 * \brief Get uint32_t from istream (binary) (invert byte order)
 */
static inline std::optional<uint32_t> bufferToUint32BE(std::istream &buffer);

/*!
 * \brief Get uint64_t from istream (binary)
 */
static inline std::optional<uint64_t> bufferToUint64(std::istream &buffer);

/*!
 * \brief Get uint64_t from istream (binary) (invert byte order)
 */
static inline std::optional<uint64_t> bufferToUint64BE(std::istream &buffer);

/*!
 * \brief Put uint16_t into ostream (binary)
 */
static inline bool uint16ToBuffer(std::ostream &buffer, uint16_t data);

/*!
 * \brief Put uint16_t into ostream (binary) (invert byte order)
 */
static inline bool uint16BEToBuffer(std::ostream &buffer, uint16_t data);

/*!
 * \brief Put uint32_t into ostream (binary)
 */
static inline bool uint32ToBuffer(std::ostream &buffer, uint32_t data);

/*!
 * \brief Put uint32_t into ostream (binary) (invert byte order)
 */
static inline bool uint32BEToBuffer(std::ostream &buffer, uint32_t data);

/*!
 * \brief Put uint64_t into ostream (binary)
 */
static inline bool uint64ToBuffer(std::ostream &buffer, uint64_t data);

/*!
 * \brief Put uint64_t into ostream (binary) (invert byte order)
 */
static inline bool uint64BEToBuffer(std::ostream &buffer, uint64_t data);
#endif // PREGPARSER_BINARY
