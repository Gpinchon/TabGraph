/*
 * @Author: gpi
 * @Date:   2019-02-22 16:13:28
 * @Last Modified by:   gpi
 * @Last Modified time: 2019-08-09 14:15:14
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
struct FBXHeader
{
    char fileMagic[21];
    unsigned char hex[2];
    uint32_t version;
};
#pragma pack()

typedef std::variant<Byte *, char *, float *, double *, int32_t *, int64_t *>
    FBXArrayData;

struct FBXArray
{
    uint32_t length;
    uint32_t encoding;
    uint32_t compressedLength;
    FBXArrayData data;
    template <typename T>
    operator T() const { return std::get<T>(data); }
    operator std::string() const { return std::get<char *>(data); };
};

typedef std::variant<Byte, float, double, int16_t, int32_t, int64_t, FBXArray>
    FBXPropertyData;

struct FBXProperty
{
    unsigned char typeCode;
    FBXPropertyData data;
    virtual void Print() const;
    template <typename T>
    operator T() { return std::get<T>(data); }
    operator std::string() { return std::get<FBXArray>(data); }
};

void FBXProperty::Print() const
{
    std::cout << "  Property(";
    switch (typeCode)
    {
    case ('C'):
        std::cout << "bool, " << std::get<Byte>(data);
        break;
    case ('Y'):
        std::cout << "int16_t, " << std::get<int16_t>(data);
        break;
    case ('I'):
        std::cout << "int32_t, " << std::get<int32_t>(data);
        break;
    case ('L'):
        std::cout << "int64_t, " << std::get<int64_t>(data);
        break;
    case ('F'):
        std::cout << "float, " << std::get<float>(data);
        break;
    case ('D'):
        std::cout << "double, " << std::get<double>(data);
        break;
    case ('S'):
        std::cout << "string, \"" << std::string(FBXArray(*this)) << "\"";
        break;
    case ('R'):
        std::cout << "Byte *, " << std::get<FBXArray>(data).length;
        break;
    case ('b'):
        std::cout << "bool *, " << std::get<FBXArray>(data).length;
        break;
    case ('i'):
        std::cout << "int32_t *, " << std::get<FBXArray>(data).length;
        break;
    case ('l'):
        std::cout << "int64_t *, " << std::get<FBXArray>(data).length;
        break;
    case ('f'):
        std::cout << "float *, " << std::get<FBXArray>(data).length;
        break;
    case ('d'):
        std::cout << "double *, " << std::get<FBXArray>(data).length;
        break;
    }
    std::cout << ");\n";
}

struct FBXNode
{
    std::string name{""};
    std::vector<FBXProperty> properties;
    std::map<std::string, std::vector<FBXNode *>> nodes;
    /** @return a vector containing all nodes with this name */
    std::vector<FBXNode *> &Nodes(const std::string &name);
    /** @return the first node named with this name */
    FBXNode *Node(const std::string &name);
    /** @return the property at this index */
    FBXProperty &Property(const size_t index);
    virtual void Print() const;
};

std::vector<FBXNode *> &FBXNode::Nodes(const std::string &name)
{
    return nodes[name];
}

FBXNode *FBXNode::Node(const std::string &name)
{
    auto nodesVector = Nodes(name);
    if (nodesVector.size() > 0)
        return nodesVector.at(0);
    return nullptr;
}

void FBXNode::Print() const
{
    std::cout << "Node (\"" << name << "\", " << properties.size() << ") {\n";
    for (auto property : properties)
        property.Print();
    for (const auto subNodes : nodes)
    {
        for (const auto &subNode : subNodes.second)
            subNode->Print();
    }
    std::cout << "};" << std::endl;
}

FBXProperty &FBXNode::Property(const size_t index)
{
    return properties.at(index);
}

struct FBXDocument : public FBXNode
{
    FBXHeader header;
    std::string path;
    void Print() const override;
};

void FBXDocument::Print() const
{
    std::cout << "/*\n"
              << "** File : " << path << "\n"
              << "** Header :\n"
              << "** FileMagic : \"" << header.fileMagic << "\"\n"
              << "** Version :   " << header.version << "\n"
              << "*/\n"
              << std::endl;
    FBXNode::Print();
}

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

template <typename T>
FBXArray ParseArray(FILE *fd)
{
    FBXArray array;

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

FBXArray parseRawData(FILE *fd)
{
    FBXArray rawData;
    fread(&rawData.length, sizeof(unsigned), 1, fd);
    rawData.data = new Byte[rawData.length];
    fread(std::get<Byte *>(rawData.data), rawData.length, 1, fd);
    return rawData;
}

FBXArray parseString(FILE *fd)
{
    FBXArray rawString;
    fread(&rawString.length, sizeof(unsigned), 1, fd);
    rawString.data = new char[rawString.length + 1];
    memset(std::get<char *>(rawString.data), 0, rawString.length + 1);
    fread(std::get<char *>(rawString.data), rawString.length, 1, fd);
    return rawString;
}

FBXProperty ParseProperty(FILE *fd)
{
    FBXProperty property;

    fread(&property, 1, 1, fd);
    switch (property.typeCode)
    {
    case ('Y'):
    {
        int16_t data;
        fread(&data, sizeof(int16_t), 1, fd);
        property.data = data;
        break;
    }
    case ('C'):
    {
        Byte data;
        fread(&data, sizeof(Byte), 1, fd);
        property.data = data;
        break;
    }
    case ('I'):
    {
        int32_t data;
        fread(&data, sizeof(int32_t), 1, fd);
        property.data = data;
        break;
    }
    case ('F'):
    {
        float data;
        fread(&data, sizeof(float), 1, fd);
        property.data = data;
        break;
    }
    case ('D'):
    {
        double data;
        fread(&data, sizeof(double), 1, fd);
        property.data = data;
        break;
    }
    case ('L'):
    {
        int64_t data;
        fread(&data, sizeof(int64_t), 1, fd);
        property.data = data;
        break;
    }
    case ('R'):
        property.data = parseRawData(fd);
        break;
    case ('S'):
        property.data = parseString(fd);
        break;
    case ('f'):
        property.data = ParseArray<float>(fd);
        break;
    case ('d'):
        property.data = ParseArray<double>(fd);
        break;
    case ('l'):
        property.data = ParseArray<int64_t>(fd);
        break;
    case ('i'):
        property.data = ParseArray<int32_t>(fd);
        break;
    case ('b'):
        property.data = ParseArray<Byte>(fd);
        break;
    default:
        throw std::runtime_error(std::string("Unknown FBX property type at ") + std::to_string(ftell(fd)));
    }
    return (property);
}

FBXNode *ParseNode(FILE *fd)
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

    auto node = new FBXNode;
    name = new char[int(nameLen + 1)];
    memset(name, 0, nameLen + 1);
    fread(name, 1, nameLen, fd);
    node->name = name;
    for (unsigned i = 0; i < numProperties; i++)
    {
        node->properties.push_back(ParseProperty(fd));
    }
    while (ftell(fd) != long(endOffset))
    {
        FBXNode *subNode = ParseNode(fd);
        if (subNode == nullptr)
            break;
        node->nodes[subNode->name].push_back(subNode);
    }
    return (node);
}

FBXDocument parseBin(const std::string &path)
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
    fread(&document.header, 27, 1, fd);
    if (strncmp(document.header.fileMagic, "Kaydara FBX Binary  ", 20))
    {
        fclose(fd);
        throw std::runtime_error("Invalid FBX header at : " + path);
    }
    is64bits = document.header.version >= 7500;
    for (FBXNode *node = nullptr; (node = ParseNode(fd)) != nullptr;)
        document.Nodes(node->name).push_back(node);
    fclose(fd);
    return document;
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
    layerElementUV->Node("UV")->Print();
    auto UV = layerElementUV->Node("UV");
    if (UV == nullptr)
        return uv;
    FBXArray UVArray(UV->Property(0));
    for (auto i = 0u; i < UVArray.length / 2; i++)
    {
        auto vec2 = glm::vec2(
            std::get<double *>(UVArray.data)[i * 2 + 0],
            std::get<double *>(UVArray.data)[i * 2 + 1]);
        uv.push_back(vec2);
    }
    auto UVIndex = layerElementUV->Node("UVIndex");
    if (UVIndex != nullptr)
    {
        std::vector<glm::vec2> realUV;
        FBXArray UVIndexArray(UVIndex->Property(0));
        for (auto i = 0u; i < UVIndexArray.length; i++)
        {
            auto index = std::get<int32_t *>(UVIndexArray.data)[i];
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
    std::cout << std::string(layerElementNormal->Node("MappingInformationType")->Property(0)) << std::endl;
    auto normals(layerElementNormal->Node("Normals"));
    FBXArray vnArray(normals->Property(0));
    std::cout << normals->properties.at(0).typeCode << std::endl;
    std::cout << normals->properties.at(0).data.index() << std::endl;
    std::cout << vnArray.data.index() << std::endl;
    for (auto i = 0u; i < vnArray.length / 3; i++)
    {
        CVEC4 n;
        n.x = std::get<double *>(vnArray.data)[i * 3 + 0];
        n.y = std::get<double *>(vnArray.data)[i * 3 + 1];
        n.z = std::get<double *>(vnArray.data)[i * 3 + 2];
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
    FBXArray vArray(vertices->Property(0));
    for (auto i = 0u; i < vArray.length; i++)
    {
        auto vec3 = glm::vec3(
            std::get<double *>(vArray.data)[i * 3 + 0],
            std::get<double *>(vArray.data)[i * 3 + 1],
            std::get<double *>(vArray.data)[i * 3 + 2]);
        v.push_back(vec3);
    }
    if (polygonVertexIndex == nullptr)
        return v;
    std::vector<int32_t> vi;
    FBXArray viArray(polygonVertexIndex->Property(0));
    std::cout << "viArray " << viArray.data.index() << std::endl;
    for (auto i = 0u; i < viArray.length; i++)
    {
        vi.push_back(std::get<int32_t *>(viArray.data)[i]);
    }
    std::vector<glm::vec3> realV;
    std::vector<int32_t> polygonIndex;
    for (const auto i : vi)
    {
        if (i < 0)
        {
            polygonIndex.push_back(abs(i) - 1);
            realV.push_back(v.at(polygonIndex.at(0)));
            realV.push_back(v.at(polygonIndex.at(1)));
            realV.push_back(v.at(polygonIndex.at(2)));
            if (polygonIndex.size() == 4)
            {
                realV.push_back(v.at(polygonIndex.at(2)));
                realV.push_back(v.at(polygonIndex.at(3)));
                realV.push_back(v.at(polygonIndex.at(0)));
            }
            polygonIndex.clear();
        }
        else
        {
            polygonIndex.push_back(i);
        }
    }
    std::cout << "Parsing Done ! " << realV.size() << std::endl;
    return realV;
}

std::shared_ptr<Mesh> FBX::parseMesh(const std::string &name, const std::string &path)
{
    auto document = parseBin(path);
    document.Print();
    auto mesh(Mesh::create(name));
    auto mtl = Material::create("default_fbx");
    mtl->albedo = glm::vec3(0.5);
    mtl->roughness = 0.5;
    for (const auto &objects : document.Nodes("Objects"))
    {
        if (objects == nullptr)
            continue;
        for (const auto &geometry : objects->Nodes("Geometry"))
        {
            if (geometry == nullptr)
                continue;
            auto geometryId(std::to_string(int64_t(geometry->Property(0))));
            auto vgroup(Vgroup::create(geometryId)); //first property is Geometry ID

            vgroup->vt = parseUV(geometry->Node("layerElementUV"));
            vgroup->vn = parseNormals(geometry->Node("LayerElementNormal"));
            vgroup->v = parseVertices(geometry->Node("Vertices"),
                                      geometry->Node("PolygonVertexIndex"));
            vgroup->set_material(mtl);
            auto meshChild(Mesh::create(geometryId));
            meshChild->add(vgroup);
            mesh->add_child(meshChild);
            //mesh->add(vgroup);
        }
    }
    return mesh;
}
