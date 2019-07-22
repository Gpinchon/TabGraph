/*
 * @Author: gpi
 * @Date:   2019-02-22 16:13:28
 * @Last Modified by:   gpi
 * @Last Modified time: 2019-07-22 10:52:12
 */

#include "parser/FBX.hpp"
#include <errno.h>   // for errno
#include <io.h>      // for access, R_OK
#include <iostream>  // for operator<<, basic_ostream, cout, ostream, char_...
#include <map>       // for allocator, map
#include <stdexcept> // for runtime_error
#include <stdint.h>  // for int32_t, int64_t, uint32_t, uint64_t, int16_t
#include <stdio.h>   // for fread, FILE, ftell, fclose, fopen
#include <string.h>  // for memset, strerror, strncmp
#include <utility>   // for pair
#include <variant>
#include <vector>  // for vector
#include <zconf.h> // for Byte
#include <zlib.h>  // for z_stream, Z_NULL, inflate, inflateEnd, Z_NO_FLUSH

#pragma pack(1)
typedef std::variant<Byte *, char *, float *, double *, int32_t *, int64_t *>
    FBXArrayData;

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

typedef std::variant<Byte, char, bool, float, double, int16_t, int32_t, int64_t,
                     FBXArray, FBXRawData>
    FBXPropertyData;

struct FBXProperty {
    unsigned char typeCode;
    FBXPropertyData data;
};

struct FBXNode {
    uint64_t endOffset;
    uint64_t numProperties;
    uint64_t propertyListLen;
    unsigned char nameLen;
    char *name;
    std::vector<FBXProperty *> properties;
    std::map<std::string, FBXNode *> nodes;
};

struct FBXHeader {
    char fileMagic[21];
    unsigned char hex[2];
    uint32_t version;
};
#pragma pack()

struct FBXDocument {
    FBXHeader header;
    std::map<std::string, FBXNode *> nodes;
    std::string path;
    void print();
};

bool is64bits = false;

template <typename T> void DecompressArray(FBXArray *array) {
    auto data = new T[array->length];
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    infstream.avail_in = array->compressedLength;      // size of input
    infstream.next_in = std::get<Byte *>(array->data); // input char array
    infstream.avail_out = array->length * sizeof(T);   // size of output
    infstream.next_out = (Byte *)data;                 // output char array
    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);
    array->data = (Byte *)data;
    array->encoding = 0;
    array->compressedLength = 0;
}

FBXArray *ParseArray(unsigned char typeCode, FILE *fd) {
    auto array = new FBXArray;

    fread(array, 12, 1, fd);
    if (array->encoding == 0) {
        switch (typeCode) {
        case ('f'):
            array->data = new float[array->length];
            fread(std::get<float *>(array->data), array->length, sizeof(float),
                  fd);
            break;
        case ('d'):
            array->data = new double[array->length];
            fread(std::get<double *>(array->data), array->length,
                  sizeof(double), fd);
            break;
        case ('l'):
            array->data = new int64_t[array->length];
            fread(std::get<int64_t *>(array->data), array->length,
                  sizeof(int64_t), fd);
            break;
        case ('i'):
            array->data = new int32_t[array->length];
            fread(std::get<int32_t *>(array->data), array->length,
                  sizeof(int32_t), fd);
            break;
        case ('b'):
            array->data = new Byte[array->length];
            fread(std::get<Byte *>(array->data), array->length, sizeof(Byte),
                  fd);
            break;
        }
    } else {
        array->data = new Byte[array->compressedLength];
        fread(std::get<Byte *>(array->data), array->compressedLength, 1, fd);
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
    std::cout << "			ArrayLength : " << array->length
              << "\n";
    std::cout << "			ArrayEncoding : " << array->encoding
              << "\n";
    std::cout << "			CompressedLength : "
              << array->compressedLength << "\n";
    return (array);
}

FBXRawData *parseRawData(FILE *fd) {
    auto rawData = new FBXRawData;
    fread(&rawData->length, sizeof(unsigned), 1, fd);
    rawData->data = new Byte[rawData->length];
    fread(std::get<Byte *>(rawData->data), rawData->length, 1, fd);
    return (rawData);
}

FBXRawData *parseString(FILE *fd) {
    auto rawString = new FBXRawData;
    fread(&rawString->length, sizeof(unsigned), 1, fd);
    rawString->data = new char[rawString->length + 1];
    memset(std::get<char *>(rawString->data), 0, rawString->length + 1);
    fread(std::get<char *>(rawString->data), rawString->length, 1, fd);
    return (rawString);
}

FBXProperty *ParseProperty(FILE *fd) {
    auto property = new FBXProperty;

    fread(property, 1, 1, fd);
    switch (property->typeCode) {
    case ('Y'): {
        int16_t data;
        fread(&data, sizeof(short), 1, fd);
        property->data = data;
        break;
    }
    case ('C'): {
        Byte data;
        fread(&data, sizeof(Byte), 1, fd);
        property->data = data;
        break;
    }
    case ('I'): {
        int32_t data;
        fread(&data, sizeof(int32_t), 1, fd);
        property->data = data;
        break;
    }
    case ('F'): {
        float data;
        fread(&data, sizeof(float), 1, fd);
        property->data = data;
        break;
    }
    case ('D'): {
        double data;
        fread(&data, sizeof(double), 1, fd);
        property->data = data;
        break;
    }
    case ('L'): {
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
    case ('d'):
    case ('l'):
    case ('i'):
    case ('b'):
        property->data = ParseArray(property->typeCode, fd);
        break;
    default:
        throw std::runtime_error(std::string("Unknown FBX property type at ") +
                                 std::to_string(ftell(fd)));
    }
    return (property);
}

FBXNode *ParseNode(FILE *fd) {
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
    if (n.endOffset == 0 && n.numProperties == 0 && n.propertyListLen == 0 &&
        n.nameLen == 0) {
        return (nullptr);
    }
    if (n.nameLen == 0) {
        return (ParseNode(fd)); // this is top/invalid node, ignore it
    }

    auto *node = new FBXNode(n);
    node->name = new char[int(node->nameLen + 1)];
    memset(node->name, 0, node->nameLen + 1);
    fread(node->name, 1, node->nameLen, fd);
    for (unsigned i = 0; i < node->numProperties; i++) {
        node->properties.push_back(ParseProperty(fd));
    }
    while (ftell(fd) != long(n.endOffset)) {
        FBXNode *subNode = ParseNode(fd);
        if (subNode == nullptr)
            break;
        node->nodes[subNode->name] = subNode;
    }
    return (node);
}

void FBX::parseBin(const std::string &path) {
    FILE *fd;
    FBXDocument document;

    if (access(path.c_str(), R_OK) != 0) {
        throw std::runtime_error(std::string("Can't access ") + path + " : " +
                                 strerror(errno));
    }
    if ((fd = fopen(path.c_str(), "rb")) == nullptr) {
        throw std::runtime_error(std::string("Can't open ") + path + " : " +
                                 strerror(errno));
    }
    document.path = path;
    fread(&document.header, sizeof(FBXHeader), 1, fd);
    if (strncmp(document.header.fileMagic, "Kaydara FBX Binary  ", 20)) {
        fclose(fd);
        throw std::runtime_error("Invalid FBX header at : " + path);
    }
    is64bits = document.header.version >= 7500;
    for (FBXNode *node = nullptr; (node = ParseNode(fd)) != nullptr;)
        document.nodes[node->name] = node;
    fclose(fd);
    document.print();
}

void printProperty(FBXProperty *property) {
    std::cout << "	Property(";
    switch (property->typeCode) {
    case ('C'):
        std::cout << "bool, " << std::get<bool>(property->data);
        break;
    case ('Y'):
        std::cout << "int16_t, " << std::get<int16_t>(property->data);
        break;
    case ('I'):
        std::cout << "int32_t, " << std::get<int32_t>(property->data);
        break;
    case ('L'):
        std::cout << "int64_t, " << std::get<int64_t>(property->data);
        break;
    case ('F'):
        std::cout << "float, " << std::get<float>(property->data);
        break;
    case ('D'):
        std::cout << "double, " << std::get<double>(property->data);
        break;
    case ('S'):
        std::cout << "string, \""
                  << std::get<char *>(std::get<FBXRawData>(property->data).data)
                  << "\"";
        break;
    case ('R'):
        std::cout << "Byte *, " << std::get<FBXRawData>(property->data).length;
        break;
    case ('b'):
        std::cout << "bool *, " << std::get<FBXArray>(property->data).length;
        break;
    case ('i'):
        std::cout << "int32_t *, " << std::get<FBXArray>(property->data).length;
        break;
    case ('l'):
        std::cout << "int64_t *, " << std::get<FBXArray>(property->data).length;
        break;
    case ('f'):
        std::cout << "float *, " << std::get<FBXArray>(property->data).length;
        break;
    case ('d'):
        std::cout << "double *, " << std::get<FBXArray>(property->data).length;
        break;
    }
    std::cout << ");\n";
}

void printNode(FBXNode *node) {
    std::cout << "Node (\"" << node->name << "\", " << node->numProperties
              << ") {\n";
    for (auto property : node->properties)
        printProperty(property);
    for (auto subNode : node->nodes)
        printNode(subNode.second);
    std::cout << "};" << std::endl;
}

void FBXDocument::print() {
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