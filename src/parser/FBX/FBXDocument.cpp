/*
* @Author: gpinchon
* @Date:   2019-08-10 11:52:02
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 20:25:21
*/

#include "Parser/FBX/FBXDocument.hpp"
#include "Parser/FBX.hpp"
#include "Parser/FBX/FBXNode.hpp"
#include "Parser/FBX/FBXProperty.hpp"
#include "Mesh/MeshParser.hpp"
#include <errno.h> // for errno
#include <iostream> // for operator<<, basic_ostream, cout, ostream, char_...
#include <map> // for allocator, map
#include <stdexcept> // for runtime_error
#include <stdint.h> // for int32_t, int64_t, uint32_t, uint64_t, int16_t
#include <stdio.h> // for fread, FILE, ftell, fclose, fopen
#include <string.h> // for memset, strerror, strncmp
#include <utility> // for pair
#include <variant>
#include <vector> // for vector
#include <zconf.h> // for Byte
#include <zlib.h> // for z_stream, Z_NULL, inflate, inflateEnd, Z_NO_FLUSH

#ifdef _WIN32
#include <io.h>
#else
#include <sys/io.h>
#endif // for access, R_OK

namespace FBX
{
#pragma pack(1)
struct Header
{
    char fileMagic[21];
    unsigned char hex[2];
    uint32_t version;
};
#pragma pack()
} // namespace FBX

using namespace FBX;

bool is64bits = false;

Document::Document()
    : header(new Header)
{
}

void Document::Print() const
{
    std::cout << "/*\n"
              << "** File : " << path << "\n"
              << "** Header :\n"
              << "** FileMagic : \"" << header->fileMagic << "\"\n"
              << "** Version :   " << header->version << "\n"
              << "*/\n"
              << std::endl;
    Node::Print();
}

template <typename T>
void DecompressArray(Array *array)
{
    auto data = new T[array->length];
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    infstream.avail_in = array->compressedLength; // size of input
    infstream.next_in = std::get<Byte *>(array->data); // input char array
    infstream.avail_out = array->length * sizeof(T); // size of output
    infstream.next_out = (Byte *)data; // output char array
    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);
    array->data = (T *)data;
    array->encoding = 0;
    array->compressedLength = 0;
}

template <typename T>
Array ParseArray(FILE *fd)
{
    Array array;

    fread(&array.length, sizeof(array.length), 1, fd);
    fread(&array.encoding, sizeof(array.encoding), 1, fd);
    fread(&array.compressedLength, sizeof(array.compressedLength), 1, fd);
    if (array.encoding == 0)
    {
        array.data = new T[array.length];
        fread(std::get<T *>(array.data), array.length, sizeof(T), fd);
    }
    else
    {
        array.data = new Byte[array.compressedLength];
        fread(std::get<Byte *>(array.data), array.compressedLength, 1, fd);
        DecompressArray<T>(&array);
    }
    return (array);
}

Array parseRawData(FILE *fd)
{
    Array rawData;
    fread(&rawData.length, sizeof(unsigned), 1, fd);
    rawData.data = new Byte[rawData.length];
    fread(std::get<Byte *>(rawData.data), rawData.length, 1, fd);
    return rawData;
}

Array parseString(FILE *fd)
{
    Array rawString;
    fread(&rawString.length, sizeof(unsigned), 1, fd);
    rawString.data = new char[rawString.length + 1];
    memset(std::get<char *>(rawString.data), 0, rawString.length + 1);
    fread(std::get<char *>(rawString.data), rawString.length, 1, fd);
    return rawString;
}

static inline auto ParseProperty(FILE *fd)
{
    auto property(Property::Create());

    fread(&property->typeCode, 1, 1, fd);
    switch (property->typeCode)
    {
    case ('Y'):
    {
        int16_t data;
        fread(&data, sizeof(int16_t), 1, fd);
        property->data = data;
        break;
    }
    case ('C'):
    {
        Byte data;
        fread(&data, sizeof(Byte), 1, fd);
        property->data = data;
        break;
    }
    case ('I'):
    {
        int32_t data;
        fread(&data, sizeof(int32_t), 1, fd);
        property->data = data;
        break;
    }
    case ('F'):
    {
        float data;
        fread(&data, sizeof(float), 1, fd);
        property->data = data;
        break;
    }
    case ('D'):
    {
        double data;
        fread(&data, sizeof(double), 1, fd);
        property->data = data;
        break;
    }
    case ('L'):
    {
        int64_t data;
        fread(&data, sizeof(int64_t), 1, fd);
        property->data = data;
        break;
    }
    case ('R'):
        property->data = parseRawData(fd);
        break;
    case ('S'):
        property->data = parseString(fd);
        break;
    case ('f'):
        property->data = ParseArray<float>(fd);
        break;
    case ('d'):
        property->data = ParseArray<double>(fd);
        break;
    case ('l'):
        property->data = ParseArray<int64_t>(fd);
        break;
    case ('i'):
        property->data = ParseArray<int32_t>(fd);
        break;
    case ('b'):
        property->data = ParseArray<Byte>(fd);
        break;
    default:
        throw std::runtime_error(std::string("Unknown FBX property type at ") + std::to_string(ftell(fd)));
    }
    return (property);
}

std::shared_ptr<Node> ParseNode(FILE *fd)
{
    uint64_t length64bits;
    uint32_t length32bits;
    uint64_t endOffset = 0;
    uint64_t numProperties = 0;
    uint64_t propertyListLen = 0;
    uint64_t nameLen = 0;
    char *name = nullptr;

    if (is64bits)
    {
        fread(&length64bits, sizeof(uint64_t), 1, fd);
        endOffset = length64bits;
        fread(&length64bits, sizeof(uint64_t), 1, fd);
        numProperties = length64bits;
        fread(&length64bits, sizeof(uint64_t), 1, fd);
        propertyListLen = length64bits;
    }
    else
    {
        fread(&length32bits, sizeof(uint32_t), 1, fd);
        endOffset = length32bits;
        fread(&length32bits, sizeof(uint32_t), 1, fd);
        numProperties = length32bits;
        fread(&length32bits, sizeof(uint32_t), 1, fd);
        propertyListLen = length32bits;
    }
    fread(&nameLen, sizeof(unsigned char), 1, fd);
    if (endOffset == 0 && numProperties == 0 && propertyListLen == 0 && nameLen == 0)
        return (nullptr);
    if (nameLen == 0)
        return (ParseNode(fd)); // this is top/invalid node, ignore it

    auto node = Node::Create();
    name = new char[int(nameLen + 1)];
    memset(name, 0, nameLen + 1);
    fread(name, 1, nameLen, fd);
    node->SetName(name);
    for (unsigned i = 0; i < numProperties; i++)
    {
        node->properties.push_back(ParseProperty(fd));
    }
    while (ftell(fd) != long(endOffset))
    {
        auto subNode = ParseNode(fd);
        if (subNode == nullptr)
            break;
        node->SubNodes(subNode->Name()).push_back(subNode);
    }
    if (node->Name() == "Geometry"
        || node->Name() == "Material"
        || node->Name() == "Model"
        || node->Name() == "NodeAttribute"
        || node->Name() == "Document")
        Object::Add(node->Property(0), node);
    return (node);
}

/*static inline auto GetTypePaterns(FBX::Document &document) {
    std::map<std::string, 
    auto definitions(document.SubNodes("Definitions"));

}*/

Document *Document::Parse(const std::string &path)
{
    FILE *fd;
    auto document(new Document);

    if (access(path.c_str(), R_OK) != 0)
    {
        throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
    }
    if ((fd = fopen(path.c_str(), "rb")) == nullptr)
    {
        throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
    }
    document->path = path;
    fread(document->header, 27, 1, fd);
    if (strncmp(document->header->fileMagic, "Kaydara FBX Binary  ", 20))
    {
        fclose(fd);
        throw std::runtime_error("Invalid FBX header at : " + path);
    }
    is64bits = document->header->version >= 7500;
    for (std::shared_ptr<Node> node = nullptr; (node = ParseNode(fd)) != nullptr;)
        document->SubNodes(node->Name()).push_back(node);
    fclose(fd);
    return document;
}