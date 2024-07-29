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

#include <cinttypes>
#include <iostream>
#include <optional>
#include <vector>

#include <encoding.hpp>

/*!
 * \brief Get string from istream (binary)
 * if conv == nullptr, then conv will be initialized inside by `iconv_open("UTF-8", "UTF-16LE")`
 * \return on any error return empty optional
 * \warning string in buffer must be ended with '\0'
 * \warning `conv` must be initialized by `iconv_open("UTF-8", "UTF-16LE")`
 * \warning if `conv` is (size_t)-1, then function will return empty optional
 */
std::optional<std::string> bufferToString(std::istream &buffer, size_t size,
                                          iconv_t conv = nullptr);
/*!
 * \brief Put string from istream (binary)
 * if conv == nullptr, then conv will be initialized inside by `iconv_open("UTF-8", "UTF-16LE")`
 * \return Size of writed string. On any error return (size_t)-1
 * \warning string in buffer will be ended with '\0'
 * \warning `conv` must be initialized by `iconv_open("UTF-16LE", "UTF-8")`
 * \warning if `conv` is (size_t)-1, then function will return (size_t)-1
 */
size_t stringToBuffer(std::ostream &buffer, const std::string &data, iconv_t conv = nullptr);


/*!
 * \brief Get strings from istream (binary)
 * if conv == nullptr, then conv will be initialized inside by `iconv_open("UTF-8", "UTF-16LE")`
 * \return on any error return empty optional
 * \warning every strings in buffer must be ended with '\0' (last included)
 * \warning `conv` must be initialized by `iconv_open("UTF-8", "UTF-16LE")`
 * \warning if `conv` is (size_t)-1, then function will return empty optional
 */
std::optional<std::vector<std::string>> bufferToStrings(std::istream &buffer, size_t size,
                                          iconv_t conv = nullptr);

/*!
 * \brief Put string from istream (binary)
 * if conv == nullptr, then conv will be initialized inside by `iconv_open("UTF-8", "UTF-16LE")`
 * \return Size of writed strings. On any error return (size_t)-1
 * \warning every string in buffer will be ended with '\0' (last included)
 * \warning `conv` must be initialized by `iconv_open("UTF-16LE", "UTF-8")`
 * \warning if `conv` is (size_t)-1, then function will return (size_t)-1
 */
size_t StringsToBuffer(std::ostream &buffer, std::vector<std::string> &data,
                                          iconv_t conv = nullptr);

/*!
 * \brief Get vector of raw data from istream (binary)
 * \return on any error return empty optional
 */
std::optional<std::vector<uint8_t>> bufferToVector(std::istream &buffer, size_t size);

/*!
 * \brief Put vector of raw data to istream (binary)
 * \return on any error return false. On success return true.
 */
bool vectorToBuffer(std::ostream &buffer, std::vector<uint8_t> &data);

/*!
 * \brief Get integral number from istream (binary)
 */
template <typename T, bool LE = true,
          typename = std::enable_if_t<std::is_integral_v<T>
                                      && sizeof(T) <= sizeof(unsigned long long)>>
std::optional<T> bufferToIntegral(std::istream &buffer)
{
    T num = 0;

    buffer.read(reinterpret_cast<char *>(&num), sizeof(T));
    if (buffer.fail()) {
        return {};
    }
    if constexpr (LE) {
        return leToNative<T>(num);
    } else {
        return beToNative<T>(num);
    }

    return num;
}

/*!
 * \brief Put integral number to ostream (binary)
 */
template <typename T, bool LE = true,
          typename = std::enable_if_t<std::is_integral_v<T>
                                      && sizeof(T) <= sizeof(unsigned long long)>>
bool integralToBuffer(std::ostream &buffer, T num)
{
    if constexpr (LE) {
        num = nativeToLe<T>(num);
    } else {
        num = nativeToBe<T>(num);
    }

    buffer.write(reinterpret_cast<char *>(&num), sizeof(T));
    if (buffer.fail()) {
        return false;
    }

    return true;
}

/*!
 * \brief Get uint16_t from istream (binary)
 */
std::optional<uint16_t> bufferToUint16(std::istream &buffer);

/*!
 * \brief Get uint16_t from istream (binary) (invert byte order)
 */
std::optional<uint16_t> bufferToUint16BE(std::istream &buffer);

/*!
 * \brief Get uint32_t from istream (binary)
 */
std::optional<uint32_t> bufferToUint32(std::istream &buffer);

/*!
 * \brief Get uint32_t from istream (binary) (invert byte order)
 */
std::optional<uint32_t> bufferToUint32BE(std::istream &buffer);

/*!
 * \brief Get uint64_t from istream (binary)
 */
std::optional<uint64_t> bufferToUint64(std::istream &buffer);

/*!
 * \brief Get uint64_t from istream (binary) (invert byte order)
 */
std::optional<uint64_t> bufferToUint64BE(std::istream &buffer);

/*!
 * \brief Put uint16_t into ostream (binary)
 */
bool uint16ToBuffer(std::ostream &buffer, uint16_t data);

/*!
 * \brief Put uint16_t into ostream (binary) (invert byte order)
 */
bool uint16BEToBuffer(std::ostream &buffer, uint16_t data);

/*!
 * \brief Put uint32_t into ostream (binary)
 */
bool uint32ToBuffer(std::ostream &buffer, uint32_t data);

/*!
 * \brief Put uint32_t into ostream (binary) (invert byte order)
 */
bool uint32BEToBuffer(std::ostream &buffer, uint32_t data);

/*!
 * \brief Put uint64_t into ostream (binary)
 */
bool uint64ToBuffer(std::ostream &buffer, uint64_t data);

/*!
 * \brief Put uint64_t into ostream (binary) (invert byte order)
 */
bool uint64BEToBuffer(std::ostream &buffer, uint64_t data);
#endif // PREGPARSER_BINARY
