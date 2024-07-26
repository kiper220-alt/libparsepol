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
#include <cinttypes>

// TODO: Implement all the missing functions like `uint64Tobuffer`.

/*!
 * \brief Convert binary data to uint16_t
 */
static inline uint16_t bufferToUint16(const char *buffer);

/*!
 * \brief Convert binary data to uint16_t with invert byte order
 */
static inline uint16_t bufferToUint16BE(const char *buffer);

/*!
 * \brief Convert binary data to uint32_t
 */
static inline uint32_t bufferToUint32(const char *buffer);

/*!
 * \brief Convert binary data to uint32_t with invert byte order
 */
static inline uint32_t bufferToUint32BE(const char *buffer);

/*!
 * \brief Convert binary data to uint64_t
 */
static inline uint64_t bufferToUint64(const char *buffer);

/*!
 * \brief Convert binary data to uint64_t with invert byte order
 */
static inline uint64_t bufferToUint64BE(const char *buffer);

/*!
 * \brief Convert binary data to uint16_t
 */
static inline uint16_t bufferToUint16(const char *buffer);

/*!
 * \brief Convert binary data to uint16_t with invert byte order
 */
static inline uint16_t bufferToUint16BE(const char *type_buffer);

/*!
 * \brief Convert binary data to uint32_t
 */
static inline uint32_t bufferToUint32(const char *type_buffer);

/*!
 * \brief Convert binary data to uint32_t with invert byte order
 */
static inline uint32_t bufferToUint32BE(const char *type_buffer);

/*!
 * \brief Convert binary data to uint64_t
 */
static inline uint64_t bufferToUint64(const char *type_buffer);

/*!
 * \brief Convert binary data to uint64_t with invert byte order
 */
static inline uint64_t bufferToUint64BE(const char *type_buffer);
