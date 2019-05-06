/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:08:03
*/

#include "parser/FBX.hpp"
#include "parser/InternalTools.hpp"
#include <iostream>
#include <map>
#include <zlib.h>

#pragma pack(1)
union FBXArrayData {
    Byte* toByte;
    char* toChar;
    float* toFloat;
    double* toDouble;
    int32_t* toInt32;
    int64_t* toInt64;
};

struct FBXArray {
    uint32_t length;
    uint32_t encoding;
    uint32_t compressedLength;
    FBXArrayData data;
};

struct FBXRawData {
    uint32_t length;
    FBXArrayData data;
};

union FBXPropertyData {
    Byte toByte;
    char toChar;
    bool toBool;
    float toFloat;
    double toDouble;
    int16_t toInt16;
    int32_t toInt32;
    int64_t toInt64;
    FBXArray* toArray;
    FBXRawData* toRawData;
};

struct FBXProperty {
    unsigned char typeCode;
    FBXPropertyData data;
};

struct FBXNode {
    uint64_t endOffset;
    uint64_t numProperties;
    uint64_t propertyListLen;
    unsigned char nameLen;
    char* name;
    std::vector<FBXProperty*> properties;
    std::map<std::string, FBXNode*> nodes;
};

struct FBXHeader {
    char fileMagic[21];
    unsigned char hex[2];
    uint32_t version;
};
#pragma pack()

struct FBXDocument {
    FBXHeader header;
    std::map<std::string, FBXNode*> nodes;
    std::string path;
    void print();
};

bool is64bits = false;

template <typename T>
void DecompressArray(FBXArray* array)
{
    auto data = new T[array->length];
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    infstream.avail_in = array->compressedLength; // size of input
    infstream.next_in = array->data.toByte; // input char array
    infstream.avail_out = array->length * sizeof(T); // size of output
    infstream.next_out = (Byte*)data; // output char array
    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);
    array->data.toByte = (Byte*)data;
    array->encoding = 0;
    array->compressedLength = 0;
}

FBXArray* ParseArray(unsigned char typeCode, FILE* fd)
{
    auto array = new FBXArray;

    fread(array, 12, 1, fd);
    if (array->encoding == 0) {
        switch (typeCode) {
        case ('f'):
            array->data.toFloat = new float[array->length];
            fread(array->data.toFloat, array->length, sizeof(float), fd);
            break;
        case ('d'):
            array->data.toDouble = new double[array->length];
            fread(array->data.toDouble, array->length, sizeof(double), fd);
            break;
        case ('l'):
            array->data.toInt64 = new int64_t[array->length];
            fread(array->data.toInt64, array->length, sizeof(int64_t), fd);
            break;
        case ('i'):
            array->data.toInt32 = new int32_t[array->length];
            fread(array->data.toInt32, array->length, sizeof(int32_t), fd);
            break;
        case ('b'):
            array->data.toByte = new Byte[array->length];
            fread(array->data.toByte, array->length, sizeof(Byte), fd);
            break;
        }
    } else {
        array->data.toByte = new Byte[array->compressedLength];
        fread(array->data.toByte, array->compressedLength, 1, fd);
        switch (typeCode) {
        case ('f'):
            DecompressArray<float>(array);
            break;
        case ('d'):
            DecompressArray<double>(array);
            break;
        case ('l'):
            DecompressArray<int64_t>(array);
            break;
        case ('i'):
            DecompressArray<int32_t>(array);
            break;
        case ('b'):
            DecompressArray<Byte>(array);
            break;
        }
    }
    std::cout << "			ArrayLength : " << array->length << "\n";
    std::cout << "			ArrayEncoding : " << array->encoding << "\n";
    std::cout << "			CompressedLength : " << array->compressedLength << "\n";
    return (array);
}

FBXRawData* parseRawData(FILE* fd)
{
    auto rawData = new FBXRawData;
    fread(&rawData->length, sizeof(unsigned), 1, fd);
    rawData->data.toByte = new Byte[rawData->length];
    fread(rawData->data.toByte, rawData->length, 1, fd);
    return (rawData);
}

FBXRawData* parseString(FILE* fd)
{
    auto rawString = new FBXRawData;
    fread(&rawString->length, sizeof(unsigned), 1, fd);
    rawString->data.toChar = new char[rawString->length + 1];
    memset(rawString->data.toChar, 0, rawString->length + 1);
    fread(rawString->data.toChar, rawString->length, 1, fd);
    return (rawString);
}

FBXProperty* ParseProperty(FILE* fd)
{
    auto property = new FBXProperty;

    fread(property, 1, 1, fd);
    switch (property->typeCode) {
    case ('Y'):
        fread(&property->data.toInt16, sizeof(short), 1, fd);
        break;
    case ('C'):
        fread(&property->data.toByte, sizeof(Byte), 1, fd);
        break;
    case ('I'):
        fread(&property->data.toInt32, sizeof(int32_t), 1, fd);
        break;
    case ('F'):
        fread(&property->data.toFloat, sizeof(float), 1, fd);
        break;
    case ('D'):
        fread(&property->data.toDouble, sizeof(double), 1, fd);
        break;
    case ('L'):
        fread(&property->data.toInt64, sizeof(int64_t), 1, fd);
        break;
    case ('R'):
        property->data.toRawData = parseRawData(fd);
        break;
    case ('S'):
        property->data.toRawData = parseString(fd);
        break;
    case ('f'):
    case ('d'):
    case ('l'):
    case ('i'):
    case ('b'):
        property->data.toArray = ParseArray(property->typeCode, fd);
        break;
    default:
        throw std::runtime_error(std::string("Unknown FBX property type at ") + std::to_string(ftell(fd)));
    }
    return (property);
}

FBXNode* ParseNode(FILE* fd)
{
    FBXNode n;
    uint64_t length64bits;
    uint32_t length32bits;

    n.endOffset = 0;
    n.numProperties = 0;
    n.propertyListLen = 0;
    if (is64bits) {
        fread(&length64bits, sizeof(uint64_t), 1, fd);
        n.endOffset = length64bits;
        fread(&length64bits, sizeof(uint64_t), 1, fd);
        n.numProperties = length64bits;
        fread(&length64bits, sizeof(uint64_t), 1, fd);
        n.propertyListLen = length64bits;
    } else {
        fread(&length32bits, sizeof(uint32_t), 1, fd);
        n.endOffset = length32bits;
        fread(&length32bits, sizeof(uint32_t), 1, fd);
        n.numProperties = length32bits;
        fread(&length32bits, sizeof(uint32_t), 1, fd);
        n.propertyListLen = length32bits;
    }
    fread(&n.nameLen, sizeof(unsigned char), 1, fd);
    if (n.endOffset == 0 && n.numProperties == 0 && n.propertyListLen == 0 && n.nameLen == 0) {
        return (nullptr);
    }
    if (n.nameLen == 0) {
        return (ParseNode(fd)); //this is top/invalid node, ignore it
    }

    auto* node = new FBXNode(n);
    node->name = new char[int(node->nameLen + 1)];
    memset(node->name, 0, node->nameLen + 1);
    fread(node->name, 1, node->nameLen, fd);
    for (unsigned i = 0; i < node->numProperties; i++) {
        node->properties.push_back(ParseProperty(fd));
    }
    while (ftell(fd) != long(n.endOffset)) {
        FBXNode* subNode = ParseNode(fd);
        if (subNode == nullptr)
            break;
        node->nodes[subNode->name] = subNode;
    }
    return (node);
}

void FBX::parseBin(const std::string& path)
{
    FILE* fd;
    FBXDocument document;

    if (access(path.c_str(), R_OK) != 0) {
        throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
    }
    if ((fd = fopen(path.c_str(), "rb")) == nullptr) {
        throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
    }
    document.path = path;
    fread(&document.header, sizeof(FBXHeader), 1, fd);
    if (strncmp(document.header.fileMagic, "Kaydara FBX Binary  ", 20))
        throw std::runtime_error("Invalid FBX header at : " + path);
    is64bits = document.header.version >= 7500;
    for (FBXNode* node = nullptr; (node = ParseNode(fd)) != nullptr;)
        document.nodes[node->name] = node;
    fclose(fd);
    document.print();
}

void printProperty(FBXProperty* property)
{
    std::cout << "	Property(";
    switch (property->typeCode) {
    case ('C'):
        std::cout << "bool, " << property->data.toBool;
        break;
    case ('Y'):
        std::cout << "int16_t, " << property->data.toInt16;
        break;
    case ('I'):
        std::cout << "int32_t, " << property->data.toInt32;
        break;
    case ('L'):
        std::cout << "int64_t, " << property->data.toInt64;
        break;
    case ('F'):
        std::cout << "float, " << property->data.toFloat;
        break;
    case ('D'):
        std::cout << "double, " << property->data.toDouble;
        break;
    case ('S'):
        std::cout << "string, \"" << property->data.toRawData->data.toChar << "\"";
        break;
    case ('R'):
        std::cout << "Byte *, " << property->data.toRawData->length;
        break;
    case ('b'):
        std::cout << "bool *, " << property->data.toArray->length;
        break;
    case ('i'):
        std::cout << "int32_t *, " << property->data.toArray->length;
        break;
    case ('l'):
        std::cout << "int64_t *, " << property->data.toArray->length;
        break;
    case ('f'):
        std::cout << "float *, " << property->data.toArray->length;
        break;
    case ('d'):
        std::cout << "double *, " << property->data.toArray->length;
        break;
    }
    std::cout << ");\n";
}

void printNode(FBXNode* node)
{
    std::cout << "Node (\"" << node->name << "\", " << node->numProperties << ") {\n";
    for (auto property : node->properties)
        printProperty(property);
    for (auto subNode : node->nodes)
        printNode(subNode.second);
    std::cout << "};" << std::endl;
}

void FBXDocument::print()
{
    std::cout << "/*\n"
              << "** File : " << path << "\n"
              << "** Header :\n"
              << "** FileMagic : \"" << header.fileMagic << "\"\n"
              << "** Version :   " << header.version << "\n"
              << "*/\n"
              << std::endl;
    for (auto node : nodes)
        printNode(node.second);
}