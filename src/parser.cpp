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
#include <sstream>
#include <vector>

#include <binary.hpp>
#include <common.hpp>
#include <parser.hpp>

/*!
 * \brief Valid POL Registery file header
 */
static const char valid_header[8] = { 0x50, 0x52, 0x65, 0x67, 0x01, 0x00, 0x00, 0x00 };

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
     * \brief Check regex `\x50\x52\x65\x67\x01\x00\x00\x00`
     */
    void parseHeader(std::istream &stream);
    /*!
     * \brief Check regex `(.{4})` and return first group as uint32_t (LE, it will be converted to
     * native)
     */
    uint32_t getSize(std::istream &stream);
    /*!
     * \brief Convert binary data from stream to PolicyData
     */
    PolicyData getData(std::istream &stream, PolicyRegType type, uint32_t size);
    /*!
     * \brief Check 32bit LE regex `([\x1\x2\x3\x4\x5\x6\x7\x8\x9\xA\xB\xC])` and return first
     * group as Type
     */
    PolicyRegType getType(std::istream &stream);
    /*!
     * \brief Matches regex `([\x20-\x5B\x5D-\x7E]\x00)+` and return
     * string as result (UTF-16LE will be converted to UTF-8)
     */
    std::string getKey(std::istream &stream);
    /*!
     * \brief Matches regex
     * `((:?([\x20-\x5B\x5D-\x7E]\x00)+)(:?\x5C\x00([\x20-\x5B\x5D-\x7E]\x00)+)+)` and return first
     * group as result
     */
    std::string getKeypath(std::istream &stream);
    /*!
     * \brief Matches regex `((:?[\x20-\x7E]\x00){1,259})` and return first group as result
     * (UTF-16LE will be converted to UTF-8)
     */
    std::string getValue(std::istream &stream);
    /*!
     * \brief Matches ABNF `LBracket KeyPath SC Value SC Type SC Size SC Data RBracket`,
     * where LBracket `\x5B\x00`, RBracket `\x5D\x00`, SC `\x3B\x00`. Return reduced structure
     */
    PolicyInstruction getInstruction(std::istream &stream);

    /*!
     * \brief Put `\x50\x52\x65\x67\x01\x00\x00\x00` into stream
     */
    void writeHeader(std::ostream &stream);
    /*!
     * \brief Put instruction, with ABNF
     * `LBracket KeyPath SC Value SC Type SC Size SC Data RBracket`,
     * where LBracket `\x5B\x00`, RBracket `\x5D\x00`, SC `\x3B\x00`, into stream.
     */
    void writeInstruction(std::ostream &stream, const PolicyInstruction &instruction);

public:
    PRegParserPrivate()
    {
        this->m_iconv_read_id = ::iconv_open("UTF-8", "UTF-16LE");
        this->m_iconv_write_id = ::iconv_open("UTF-16LE", "UTF-8");
    }

    virtual PolicyFile parse(std::istream &stream) override
    {
        PolicyBody body;

        parseHeader(stream);

        {
            // make eof on last byte
            char c;
            stream.read(&c, 1);
        }
        while (!stream.eof()) {
            stream.seekg(-1, std::ios::cur);
            auto instruction = getInstruction(stream);

            body.instructions.emplace_back(instruction);
            {
                // make eof on last byte
                char c;
                stream.read(&c, 1);
            }
        }

        return { body };
    }

    virtual bool write(std::ostream &stream, const PolicyFile &file) override
    {
        if (!file.body.has_value()) {
            return true;
        }

        writeHeader(stream);
        for (const auto &instruction : file.body->instructions) {
            writeInstruction(stream, instruction);
        }

        return true;
    }

    virtual ~PRegParserPrivate()
    {
        ::iconv_close(this->m_iconv_read_id);
        ::iconv_close(this->m_iconv_write_id);
    }

private:
    ::iconv_t m_iconv_read_id;
    ::iconv_t m_iconv_write_id;
};

void PRegParserPrivate::parseHeader(std::istream &stream)
{
    char header[8];
    stream.read(header, 8);
    check_stream(stream);

    const uint32_t signature = *reinterpret_cast<uint32_t *>(&header[0]);
    const uint32_t version = *reinterpret_cast<uint32_t *>(&header[4]);
    const uint32_t normal_signature = *reinterpret_cast<const uint32_t *>(&valid_header[0]);
    const uint32_t normal_version = *reinterpret_cast<const uint32_t *>(&valid_header[4]);

    if (signature != normal_signature && version != normal_version) {
        throw std::runtime_error("corrupted PReg file.");
    }
}

uint32_t PRegParserPrivate::getSize(std::istream &stream)
{
    return bufferToIntegral<uint32_t, true>(stream);
}

PolicyRegType PRegParserPrivate::getType(std::istream &stream)
{
    uint32_t num = bufferToIntegral<uint32_t, true>(stream);

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
    case PolicyRegType::REG_QWORD_LITTLE_ENDIAN:
    case PolicyRegType::REG_QWORD_BIG_ENDIAN:
        break;
    default:
        return {};
    }

    return static_cast<PolicyRegType>(num);
}

std::string PRegParserPrivate::getKey(std::istream &stream)
{
    std::string key;
    char16_t data;

    stream.read(reinterpret_cast<char *>(&data), 2);
    check_stream(stream);
    data = leToNative(data);

    while (data >= 0x20 && data <= 0x7E && data != 0x5C) {
        key.push_back(static_cast<char>(data));

        stream.read(reinterpret_cast<char *>(&data), 2);
        check_stream(stream);
        data = leToNative(data);
    }

    // Key from Keypath must contain 1 or more symbols.
    if (key.empty() || (data != 0 && data != 0x5C)) {
        throw std::runtime_error("corrupted PReg file.");
    }

    // Remove last symbol
    stream.seekg(-2, std::ios::cur);

    return { std::move(key) };
}

std::string PRegParserPrivate::getKeypath(std::istream &stream)
{
    std::string keyPath;
    char16_t sym = 0;

    while (true) {
        auto key = getKey(stream);

        keyPath.append(key);

        stream.read(reinterpret_cast<char *>(&sym), 2);
        check_stream(stream);

        // End of Keypath
        if (sym == 0) {
            break;
        }

        if (sym != 0x5C) {
            throw std::runtime_error("corrupted PReg file.");
        }

        keyPath.push_back('\\');
    }

    return { keyPath };
}

std::string PRegParserPrivate::getValue(std::istream &stream)
{
    std::string result;
    char16_t data;

    stream.read(reinterpret_cast<char *>(&data), 2);
    check_stream(stream);
    data = leToNative(data);

    // Key in specs [\x20-\x5B\x5D-\x7E](exclude '\'), when keypath include '\' like delimeter
    while (data >= 0x20 && data <= 0x7E) {
        // Key from Keypath must contain 1 or more symbols.

        // Check maximum value length
        if (result.length() == 259) {
            throw std::runtime_error("corrupted PReg file.");
        }

        result.push_back(data);

        stream.read(reinterpret_cast<char *>(&data), 2);
        check_stream(stream);
        data = leToNative(data);
    }

    if (data != 0 || result.empty()) {
        return {};
    }

    return { std::move(result) };
}

PolicyData PRegParserPrivate::getData(std::istream &stream, PolicyRegType type, uint32_t size)
{
    switch (type) {
    case PolicyRegType::REG_NONE:
        throw std::runtime_error("this case cannot be called in this place. WTF???");
    case PolicyRegType::REG_SZ:
    case PolicyRegType::REG_EXPAND_SZ:
    case PolicyRegType::REG_LINK:
        return { bufferToString(stream, size) };

    case PolicyRegType::REG_BINARY:
        return { bufferToVector(stream, size) };

    case PolicyRegType::REG_DWORD_LITTLE_ENDIAN:
        return { bufferToIntegral<uint32_t, true>(stream) };
    case PolicyRegType::REG_DWORD_BIG_ENDIAN:
        return { bufferToIntegral<uint32_t, false>(stream) };

    case PolicyRegType::REG_MULTI_SZ:
    case PolicyRegType::REG_RESOURCE_LIST:
    case PolicyRegType::REG_FULL_RESOURCE_DESCRIPTOR: // ????
    case PolicyRegType::REG_RESOURCE_REQUIREMENTS_LIST:
        return { bufferToStrings(stream, size) };

    case PolicyRegType::REG_QWORD_LITTLE_ENDIAN:
        return { bufferToIntegral<uint64_t, true>(stream) };
    case PolicyRegType::REG_QWORD_BIG_ENDIAN:
        return { bufferToIntegral<uint64_t, false>(stream) };
        break;
    }
    return {};
}

PolicyInstruction PRegParserPrivate::getInstruction(std::istream &stream)
{
    PolicyInstruction instruction;
    uint32_t dataSize;

    check_sym(stream, '[');

    instruction.key = getKeypath(stream);

    check_sym(stream, ';');

    instruction.value = getValue(stream);

    check_sym(stream, ';');

    instruction.type = getType(stream);

    check_sym(stream, ';');

    dataSize = getSize(stream);

    check_sym(stream, ';');

    instruction.data = getData(stream, instruction.type, dataSize);

    check_sym(stream, ']');

    return instruction;
}

std::optional<std::stringstream> getDataStream(const PolicyData &data, PolicyRegType type)
{
    std::stringstream stream;

    switch (type) {
    case PolicyRegType::REG_SZ:
    case PolicyRegType::REG_EXPAND_SZ:
    case PolicyRegType::REG_LINK:
        stringToBuffer(stream, std::get<std::string>(data));
        break;

    case PolicyRegType::REG_BINARY:
        vectorToBuffer(stream, std::get<std::vector<uint8_t>>(data));
        break;

    case PolicyRegType::REG_DWORD_LITTLE_ENDIAN:
        integralToBuffer<uint32_t, true>(stream, std::get<uint32_t>(data));
        break;
    case PolicyRegType::REG_DWORD_BIG_ENDIAN:
        integralToBuffer<uint32_t, false>(stream, std::get<uint32_t>(data));
        break;

    case PolicyRegType::REG_MULTI_SZ:
    case PolicyRegType::REG_RESOURCE_LIST:
    case PolicyRegType::REG_FULL_RESOURCE_DESCRIPTOR: // ????
    case PolicyRegType::REG_RESOURCE_REQUIREMENTS_LIST:
        stringsToBuffer(stream, std::get<std::vector<std::string>>(data));
        break;

    case PolicyRegType::REG_QWORD_LITTLE_ENDIAN:
        integralToBuffer<uint64_t, true>(stream, std::get<uint64_t>(data));
        break;
    case PolicyRegType::REG_QWORD_BIG_ENDIAN:
        integralToBuffer<uint64_t, false>(stream, std::get<uint64_t>(data));
        break;

    case PolicyRegType::REG_NONE:
    default:
        return {};
    }
    return stream;
}

void PRegParserPrivate::writeHeader(std::ostream &stream)
{
    stream.write(valid_header, sizeof(valid_header));
}

void PRegParserPrivate::writeInstruction(std::ostream &stream, const PolicyInstruction &instruction)
{
    write_sym(stream, '[');

    stringToBuffer(stream, instruction.key);

    write_sym(stream, ';');

    stringToBuffer(stream, instruction.value);

    write_sym(stream, ';');

    integralToBuffer<uint32_t, true>(stream, static_cast<uint32_t>(instruction.type));

    write_sym(stream, ';');

    auto dataStream = getDataStream(instruction.data, instruction.type);

    integralToBuffer<uint32_t, true>(stream, static_cast<uint32_t>(dataStream->tellp()));

    write_sym(stream, ';');

    stream << dataStream->str();
    check_stream(stream);

    write_sym(stream, ']');
}

std::unique_ptr<PRegParser> createPregParser()
{
    return std::make_unique<PRegParserPrivate>();
}
