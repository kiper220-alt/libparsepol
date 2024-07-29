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
#include <common.hpp>
#include <parser.hpp>

/*!
 * \brief Valid POL Registery file header
 */
static const char valid_header[5] = { 0x50, 0x52, 0x65, 0x67, 0x01 };

/*!
 * \brief Match regex `[\x20-\x7E]`
 */
static inline bool isValueCharacter(uint8_t sym)
{
    return sym >= 0x20 && sym <= 0x7E;
}

class PRegParserPrivate : public PRegParser
{
private:
    /*!
     * \brief Check regex `\x50\x52\x65\x67`
     */
    bool parseHeader(std::istream &stream);
    /*!
     * \brief Check regex `(.{4});` and return first group as uint32_t
     */
    std::optional<uint32_t> getSize(std::istream &stream);
    /*!
     * \brief Check 32bit regex `([\x1\x2\x3\x4\x5\x6\x7\x8\x9\xA\xB\xC]);` and return first group
     * as Type
     */
    std::optional<PolicyRegType> getType(std::istream &stream);
    /*!
     * \brief Matches regex `([\x20-\x7E]+);` and return first group as result
     */
    std::optional<std::string> getKeypath(std::istream &stream);
    /*!
     * \brief Matches regex `([\x20-\x7E]{1,259});` and return first group as result
     */
    std::optional<std::string> getValue(std::istream &stream);
    /*!
     * \brief Matches ABNF `'[' KeyPath ';' Value ';' Type ';' Size ';' Data ']'` and return reduced
     * structure
     */
    std::optional<PolicyInstruction> getInstruction(std::istream &stream);

public:
    PRegParserPrivate()
    {
        this->m_iconv_read_id = ::iconv_open("UTF-8", "UTF-16LE");
        this->m_iconv_write_id = ::iconv_open("UTF-16LE", "UTF-8");
    }

    virtual PolicyFile parse(std::istream &stream) override
    {
        PolicyBody body;

        if (!parseHeader(stream)) {
            return {};
        }

        return { body };
    }

    virtual bool write(const PolicyFile &file, std::istream &stream) override { return false; }

    virtual ~PRegParserPrivate()
    {
        ::iconv_close(this->m_iconv_read_id);
        ::iconv_close(this->m_iconv_write_id);
    }

private:
    ::iconv_t m_iconv_read_id;
    ::iconv_t m_iconv_write_id;
};

bool PRegParserPrivate::parseHeader(std::istream &stream)
{
    char header[5];
    const uint &signature = *header;
    const uint &normal_signature = *valid_header;

    stream.read(header, 5);
    check_stream_bool(stream);

    return signature == normal_signature && header[4] == valid_header[4];
}

std::optional<uint32_t> PRegParserPrivate::getSize(std::istream &stream)
{
    uint32_t size;

    must_present(size, bufferToUint32(stream));
    check_sym(stream, ';');

    return size;
}

std::optional<PolicyRegType> PRegParserPrivate::getType(std::istream &stream)
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

std::optional<std::string> PRegParserPrivate::getKeypath(std::istream &stream)
{
    std::string keyPath;
    char data[2];
    const uint8_t &sym = *data;
    bool next_delimeter = false; // first character can'nt be delimeter

    stream.read(data, 2);
    check_stream(stream);

    // Key in specs [\x20-\x5B\x5D-\x7E](exclude '\'), when keypath include '\' like delimeter
    while (sym >= 0x20 && sym <= 0x7E) {
        // Key from Keypath must contain 1 or more symbols.
        if (sym == 0x5C) {
            if (!next_delimeter) {
                return {};
            }
            // Next character cannot be delimeter
            next_delimeter = false;
        } else {
            // Next character can be delimeter
            next_delimeter = true;
        }

        keyPath.push_back(sym);

        stream.read(data, 2);
        check_stream(stream);
    }

    if (sym != ';' || !keyPath.empty()) {
        return {};
    }
    return { keyPath };
}

std::optional<std::string> PRegParserPrivate::getValue(std::istream &stream)
{
    std::string result;
    char data[2];
    const uint8_t &sym = *data;

    stream.read(data, 2);
    check_stream(stream);

    // Key in specs [\x20-\x5B\x5D-\x7E](exclude '\'), when keypath include '\' like delimeter
    while (sym >= 0x20 && sym <= 0x7E) {
        // Key from Keypath must contain 1 or more symbols.

        // Check maximum value length
        if (result.length() == 259) {
            return {};
        }

        result.push_back(sym);

        stream.read(data, 2);
        check_stream(stream);
    }

    if (sym != ';' || !result.empty()) {
        return {};
    }

    return { std::move(result) };
}

std::optional<PolicyInstruction> PRegParserPrivate::getInstruction(std::istream &stream)
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

    return {};
}

std::unique_ptr<PRegParser> createPregParser()
{
    return std::make_unique<PRegParserPrivate>();
}
