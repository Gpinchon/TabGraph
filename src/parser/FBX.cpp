/*
 * @Author: gpi
 * @Date:   2019-02-22 16:13:28
 * @Last Modified by:   gpi
 * @Last Modified time: 2019-08-06 17:49:48
 */

#include "parser/FBX.hpp"
#include "MeshParser.hpp"
#include <errno.h> // for errno
#include <io.h> // for access, R_OK
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

//Add this parser to MeshParser !
auto __fbxParser = MeshParser::add("fbx", FBX::parseMesh);

#pragma pack(1)
typedef std::variant<Byte *, char *, float *, double *, int32_t *, int64_t *>
    FBXArrayData;

struct FBXArray
{
    uint32_t length;
    uint32_t encoding;
    uint32_t compressedLength;
    FBXArrayData data;
};

struct FBXRawData
{
    uint32_t length;
    FBXArrayData data;
};

typedef std::variant<Byte, char, bool, float, double, int16_t, int32_t, int64_t,
                     FBXArray *, FBXRawData *>
    FBXPropertyData;

struct FBXProperty
{
    unsigned char typeCode;
    FBXPropertyData data;
};

struct FBXNode
{
    uint64_t endOffset;
    uint64_t numProperties;
    uint64_t propertyListLen;
    unsigned char nameLen;
    char *name;
    std::vector<FBXProperty *> properties;
    std::map<std::string, FBXNode *> nodes;
};

struct FBXHeader
{
    char fileMagic[21];
    unsigned char hex[2];
    uint32_t version;
};
#pragma pack()

struct FBXDocument
{
    FBXHeader header;
    std::map<std::string, FBXNode *> nodes;
    std::string path;
    void print();
};

bool is64bits = false;

template <typename T>
void DecompressArray(FBXArray *array)
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

FBXArray *ParseArray(unsigned char typeCode, FILE *fd)
{
    auto array = new FBXArray;

    fread(array, 12, 1, fd);
    if (array->encoding == 0)
    {
        switch (typeCode)
        {
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
    }
    else
    {
        array->data = new Byte[array->compressedLength];
        fread(std::get<Byte *>(array->data), array->compressedLength, 1, fd);
        switch (typeCode)
        {
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

FBXRawData *parseRawData(FILE *fd)
{
    auto rawData = new FBXRawData;
    fread(&rawData->length, sizeof(unsigned), 1, fd);
    rawData->data = new Byte[rawData->length];
    fread(std::get<Byte *>(rawData->data), rawData->length, 1, fd);
    return (rawData);
}

FBXRawData *parseString(FILE *fd)
{
    auto rawString = new FBXRawData;
    fread(&rawString->length, sizeof(unsigned), 1, fd);
    rawString->data = new char[rawString->length + 1];
    memset(std::get<char *>(rawString->data), 0, rawString->length + 1);
    fread(std::get<char *>(rawString->data), rawString->length, 1, fd);
    return (rawString);
}

FBXProperty *ParseProperty(FILE *fd)
{
    auto property = new FBXProperty;

    fread(property, 1, 1, fd);
    switch (property->typeCode)
    {
    case ('Y'):
    {
        int16_t data;
        fread(&data, sizeof(short), 1, fd);
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
    case ('d'):
    case ('l'):
    case ('i'):
    case ('b'):
        property->data = ParseArray(property->typeCode, fd);
        break;
    default:
        throw std::runtime_error(std::string("Unknown FBX property type at ") + std::to_string(ftell(fd)));
    }
    return (property);
}

FBXNode *ParseNode(FILE *fd)
{
    FBXNode n;
    uint64_t length64bits;
    uint32_t length32bits;

    n.endOffset = 0;
    n.numProperties = 0;
    n.propertyListLen = 0;
    if (is64bits)
    {
        fread(&length64bits, sizeof(uint64_t), 1, fd);
        n.endOffset = length64bits;
        fread(&length64bits, sizeof(uint64_t), 1, fd);
        n.numProperties = length64bits;
        fread(&length64bits, sizeof(uint64_t), 1, fd);
        n.propertyListLen = length64bits;
    }
    else
    {
        fread(&length32bits, sizeof(uint32_t), 1, fd);
        n.endOffset = length32bits;
        fread(&length32bits, sizeof(uint32_t), 1, fd);
        n.numProperties = length32bits;
        fread(&length32bits, sizeof(uint32_t), 1, fd);
        n.propertyListLen = length32bits;
    }
    fread(&n.nameLen, sizeof(unsigned char), 1, fd);
    if (n.endOffset == 0 && n.numProperties == 0 && n.propertyListLen == 0 && n.nameLen == 0)
    {
        return (nullptr);
    }
    if (n.nameLen == 0)
    {
        return (ParseNode(fd)); // this is top/invalid node, ignore it
    }

    auto *node = new FBXNode(n);
    node->name = new char[int(node->nameLen + 1)];
    memset(node->name, 0, node->nameLen + 1);
    fread(node->name, 1, node->nameLen, fd);
    for (unsigned i = 0; i < node->numProperties; i++)
    {
        node->properties.push_back(ParseProperty(fd));
    }
    while (ftell(fd) != long(n.endOffset))
    {
        FBXNode *subNode = ParseNode(fd);
        if (subNode == nullptr)
            break;
        node->nodes[subNode->name] = subNode;
    }
    return (node);
}

#include "Material.hpp"
#include "Mesh.hpp"
#include "Vgroup.hpp"
#include <glm/glm.hpp>

static inline std::vector<glm::vec2> parseUV(FBXNode *layerElementUV)
{
    std::vector<glm::vec2> uv;
    if (layerElementUV == nullptr)
        return uv;
    auto UV = layerElementUV->nodes["UV"];
    if (UV == nullptr)
        return uv;
    auto UVArray = std::get<FBXArray *>(UV->properties.at(0)->data);
    for (auto i = 0u; i < UVArray->length / 2; i++)
    {
        auto vec2 = glm::vec2(
            std::get<double *>(UVArray->data)[i * 2 + 0],
            std::get<double *>(UVArray->data)[i * 2 + 1]);
        uv.push_back(vec2);
    }
    auto UVIndex = layerElementUV->nodes["UVIndex"];
    if (UVIndex != nullptr)
    {
        std::vector<glm::vec2> realUV;
        auto UVIndexArray = std::get<FBXArray *>(UVIndex->properties.at(0)->data);
        for (auto i = 0u; i < UVIndexArray->length; i++)
        {
            auto index = std::get<int32_t *>(UVIndexArray->data)[i];
            realUV.push_back(uv.at(index));
        }
        return realUV;
    }
    return uv;
}

static inline auto parseNormals(FBXNode *layerElementNormal)
{
    std::vector<CVEC4> vn;
    if (layerElementNormal == nullptr)
        return vn;
    auto normals(layerElementNormal->nodes["Normals"]);
    auto vnArray(std::get<FBXArray *>(normals->properties.at(0)->data));
    std::cout << normals->properties.at(0)->typeCode << std::endl;
    std::cout << normals->properties.at(0)->data.index() << std::endl;
    std::cout << vnArray->data.index() << std::endl;
    for (auto i = 0u; i < vnArray->length / 3; i++)
    {
        CVEC4 n;
        n.x = std::get<double *>(vnArray->data)[i * 3 + 0];
        n.y = std::get<double *>(vnArray->data)[i * 3 + 1];
        n.z = std::get<double *>(vnArray->data)[i * 3 + 2];
        n.w = 255;
        vn.push_back(n);
    }
    //layerElementNormal->nodes["MappingInformationType"]
    return vn;
}

static inline auto parseVertices(FBXNode *vertices, FBXNode *polygonVertexIndex)
{
    std::vector<glm::vec3> v;
    if (vertices == nullptr)
        return v;
    auto vArray = std::get<FBXArray *>(vertices->properties.at(0)->data);
    std::cout << vertices->properties.at(0)->typeCode << std::endl;
    std::cout << "vArray " << vArray->data.index() << std::endl;
    for (auto i = 0u; i < vArray->length; i++)
    {
        auto vec3 = glm::vec3(
            std::get<double *>(vArray->data)[i * 3 + 0],
            std::get<double *>(vArray->data)[i * 3 + 1],
            std::get<double *>(vArray->data)[i * 3 + 2]);
        v.push_back(vec3);
    }
    if (polygonVertexIndex == nullptr)
        return v;
    std::vector<int32_t> vi;
    auto viArray = std::get<FBXArray *>(polygonVertexIndex->properties.at(0)->data);
    std::cout << "viArray " << viArray->data.index() << std::endl;
    for (auto i = 0u; i < viArray->length; i++)
    {
        vi.push_back(std::get<int32_t *>(viArray->data)[i]);
    }
    std::vector<glm::vec3> realV;
    //for (auto i = 0u; i < vi.size(); i++)
    std::vector<int32_t> polygonIndex;
    for (const auto i : vi)
    {
        if (i < 0)
        {
            polygonIndex.push_back(abs(i - 1));
            realV.push_back(v.at(polygonIndex.at(0)));
            realV.push_back(v.at(polygonIndex.at(1)));
            realV.push_back(v.at(polygonIndex.at(2)));
            if (polygonIndex.size() == 4)
            {
                realV.push_back(v.at(polygonIndex.at(0)));
                realV.push_back(v.at(polygonIndex.at(1)));
                realV.push_back(v.at(polygonIndex.at(3)));
            }
            polygonIndex.clear();
        }
        else
        {
            polygonIndex.push_back(i);
        }
    }
    std::cout << "Parsing Done !" << realV.size() << std::endl;
    /*for (auto i : vi)
    {
        std::cout << i << std::endl;
        realV.push_back(v.at(i));
    }*/
    return realV;
}

std::shared_ptr<Mesh> FBX::parseMesh(const std::string &name, const std::string &path)
{
    FILE *fd;
    FBXDocument document;

    if (access(path.c_str(), R_OK) != 0)
    {
        throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
    }
    if ((fd = fopen(path.c_str(), "rb")) == nullptr)
    {
        throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
    }
    document.path = path;
    fread(&document.header, sizeof(FBXHeader), 1, fd);
    if (strncmp(document.header.fileMagic, "Kaydara FBX Binary  ", 20))
    {
        fclose(fd);
        throw std::runtime_error("Invalid FBX header at : " + path);
    }
    is64bits = document.header.version >= 7500;
    for (FBXNode *node = nullptr; (node = ParseNode(fd)) != nullptr;)
        document.nodes[node->name] = node;
    fclose(fd);
    //document.print();
    auto objects(document.nodes["Objects"]);
    if (objects == nullptr)
        return nullptr;
    auto geometry(objects->nodes["Geometry"]);
    if (geometry == nullptr)
        return nullptr;
    auto mesh(Mesh::create(name));
    auto vgroup(Vgroup::create("vgroup"));
    vgroup->vt = parseUV(
        geometry->nodes["layerElementUV"]);
    vgroup->vn = parseNormals(
        geometry->nodes["LayerElementNormal"]);
    vgroup->v = parseVertices(
        geometry->nodes["Vertices"],
        geometry->nodes["PolygonVertexIndex"]);
    auto mtl = Material::create("default_fbx");
    mtl->albedo = glm::vec3(0.5);
    mtl->roughness = 0.5;
    vgroup->set_material(mtl);
    mesh->add(vgroup);
    return mesh;
}

void printProperty(FBXProperty *property)
{
    std::cout << "	Property(";
    switch (property->typeCode)
    {
    case ('C'):
        std::cout << "bool, " << std::get<Byte>(property->data);
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
                  << std::get<char *>(std::get<FBXRawData *>(property->data)->data)
                  << "\"";
        break;
    case ('R'):
        std::cout << "Byte *, " << std::get<FBXRawData *>(property->data)->length;
        break;
    case ('b'):
        std::cout << "bool *, " << std::get<FBXArray *>(property->data)->length;
        break;
    case ('i'):
        std::cout << "int32_t *, " << std::get<FBXArray *>(property->data)->length;
        break;
    case ('l'):
        std::cout << "int64_t *, " << std::get<FBXArray *>(property->data)->length;
        break;
    case ('f'):
        std::cout << "float *, " << std::get<FBXArray *>(property->data)->length;
        break;
    case ('d'):
        std::cout << "double *, " << std::get<FBXArray *>(property->data)->length;
        break;
    }
    std::cout << ");\n";
}

void printNode(FBXNode *node)
{
    std::cout << "Node (\"" << node->name << "\", " << node->numProperties
              << ") {\n";
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