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
#define check_stream_bool(target) \
    {                             \
        if (target.fail()) {      \
            return false;         \
        }                         \
    }
#define check_sym(stream, sym)              \
    {                                       \
        char buff[2];                       \
        const unsigned &uint8_t = *buff;    \
        stream.read(buff, 2);               \
        if (target.fail() || buff != sym) { \
            return {};                      \
        }                                   \
    }
