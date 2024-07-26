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
 * \brief Convert uint16_t to binary data
 */
static inline void uint16ToBuffer(char *buffer, uint16_t data);

/*!
 * \brief Convert uint16_t to binary data with invert byte order
 */
static inline void uint16BEToBuffer(char *buffer, uint16_t data);

/*!
 * \brief Convert uint32_t to binary data
 */
static inline void uint32ToBuffer(char *buffer, uint32_t data);

/*!
 * \brief Convert uint32_t to binary data with invert byte order
 */
static inline void uint32BEToBuffer(char *buffer, uint32_t data);

/*!
 * \brief Convert uint64_t to binary data
 */
static inline void uint64ToBuffer(char *buffer, uint64_t data);

/*!
 * \brief Convert uint64_t to binary data with invert byte order
 */
static inline void uint64BEToBuffer(char *buffer, uint64_t data);
