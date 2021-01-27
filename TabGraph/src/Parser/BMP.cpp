/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:17
*/

#include "Parser/BMP.hpp"
#include "Parser/InternalTools.hpp" // for t_bmp_parser, t_bmp_info, t_bmp_...
#include "Texture/Image.hpp" // for Texture2D

#include <GL/glew.h> // for GLubyte, GLenum, GL_BGR, GL_BGRA
#include <glm/glm.hpp> // for s_vec2, glm::vec2
#include <errno.h> // for errno
#include <fcntl.h> // for O_BINARY, O_CREAT, O_RDWR
#include <stdexcept> // for runtime_error
#include <stdio.h> // for fclose, fread, fopen, fseek, SEE...
#include <string.h> // for memset, strerror
#include <sys/stat.h> // for S_IRWXG, S_IRWXO, S_IRWXU

#ifdef _WIN32
#include <io.h>
#ifndef R_OK
#define R_OK 4
#endif
#else
#include <sys/io.h>
#endif // for write, access, close, open, R_OK

#ifndef O_BINARY
#define O_BINARY 0x0004
#endif

#pragma pack(1)
struct t_bmp_info {
    uint32_t    header_size{ sizeof(t_bmp_info) };
    int32_t     width;
    int32_t     height;
    uint16_t    color_planes;
    uint16_t    bpp;
    uint32_t    compression_method;
    uint32_t    size;
    int32_t     horizontal_resolution;
    int32_t     vertical_resolution;
    uint32_t    totalColors{ 0 };
    uint32_t    important_colors{ 0 };
};

struct t_bmp_header {
    std::byte    type[2];
    uint32_t     size;
    std::byte    reserved1[2]{ std::byte(0), std::byte(0) };
    std::byte    reserved2[2]{ std::byte(0), std::byte(0) };
    uint32_t     data_offset{ sizeof(t_bmp_header) + sizeof(t_bmp_info) };
};
#pragma pack()

struct t_bmp_parser {
    FILE* fd;
    t_bmp_info info;
    t_bmp_header header;
    std::vector<std::byte> data { };
    unsigned size_read;
};

static void prepare_header(t_bmp_header* header, t_bmp_info* info, std::shared_ptr<Image> t)
{
    header->type[0] = std::byte(0x42);
    header->type[1] = std::byte(0x4D);
    header->size = header->data_offset + (t->GetSize().x * t->GetSize().y * 4);
    info->width = t->GetSize().x;
    info->height = t->GetSize().y;
    info->color_planes = 1;
    info->bpp = 32;//t->GetPixelDescription().GetSize(); //t->bpp();
    info->size = t->GetSize().x * t->GetSize().y * 4;
    info->horizontal_resolution = 0x0ec4;
    info->vertical_resolution = 0x0ec4;
}

void BMP::save(std::shared_ptr<Image> image, const std::string& imagepath)
{
    t_bmp_header header;
    t_bmp_info info;
    GLubyte* padding;
    int fd;

    prepare_header(&header, &info, image);
#ifdef _WIN32
    fd = open(imagepath.c_str(), O_RDWR | O_CREAT | O_BINARY);
#else
    fd = open(imagepath.c_str(), O_RDWR | O_CREAT | O_BINARY,
        S_IRWXU | S_IRWXG | S_IRWXO);
#endif
    write(fd, &header, sizeof(t_bmp_header));
    write(fd, &info, sizeof(t_bmp_info));
    for (auto y = 0u; y < image->GetSize().y; ++y) {
        for (auto x = 0u; x < image->GetSize().x; ++x) {
            auto floatColor{ image->GetColor(glm::ivec2(x, y)) };
            glm::vec<4, uint8_t> byteColor{ glm::clamp(floatColor, 0.f, 1.f) };
            write(fd, &byteColor[0], 4);
        }
    }
    //We should not need padding
    /*padding = new GLubyte[int(info.size - int64_t(t->Size().x * t->Size().y * t->bpp() / 8))]();
    write(fd, padding, info.size - (t->GetSize().x * t->GetSize().y * t->bpp() / 8));
    delete[] padding;*/
    close(fd);
}

static void convert_bmp(t_bmp_parser* parser)
{
    std::vector<std::byte> pixel_temp{ parser->data.size() };
    std::byte rgba[4];
    int i[3];

    i[0] = 0;
    i[1] = -1;
    while (++i[1] < parser->info.width) {
        i[2] = -1;
        while (++i[2] < parser->info.height) {
            rgba[0] = parser->data[i[0] + 1];
            rgba[1] = parser->data[i[0] + 2];
            rgba[2] = parser->data[i[0] + 3];
            rgba[3] = parser->data[i[0] + 0];
            pixel_temp[i[0] + 0] = rgba[0];
            pixel_temp[i[0] + 1] = rgba[1];
            pixel_temp[i[0] + 2] = rgba[2];
            pixel_temp[i[0] + 3] = rgba[3];
            i[0] += (parser->info.bpp / 8);
        }
    }
    parser->data = pixel_temp;
}

static int read_data(t_bmp_parser* p, const std::filesystem::path& path)
{
    unsigned data_size;

    if (access(path.string().c_str(), R_OK) != 0) {
        throw std::runtime_error(strerror(errno));
    }
    if ((p->fd = fopen(path.string().c_str(), "rb")) == nullptr) {
        throw std::runtime_error(strerror(errno));
    }
    auto readReturn = fread(&p->header, 1, sizeof(p->header), p->fd);
    if (readReturn != sizeof(p->header) || p->header.type[0] != std::byte(0x42) || p->header.type[1] != std::byte(0x4D)) {
        fclose(p->fd);
        throw std::runtime_error("Wrong Header");
    }
    readReturn = fread(&p->info, 1, sizeof(p->info), p->fd);
    if (readReturn != sizeof(p->info)) {
        fclose(p->fd);
        throw std::runtime_error("Wrong Info");
    }
    if (p->info.width * p->info.bpp / 8 % 2)
        data_size = 4 * p->info.width * p->info.height;
    else
        data_size = p->info.bpp / 8 * p->info.width * p->info.height;
    fseek(p->fd, p->header.data_offset, SEEK_SET);
    p->data.resize(data_size);
    p->size_read = fread(p->data.data(), sizeof(std::byte), data_size, p->fd);
    fclose(p->fd);
    if (p->info.bpp == 32) {
        convert_bmp(p);
    }
    return (0);
}

Pixel::SizedFormat GetBMPPixelFormat(GLubyte bpp) {
    switch (bpp)
    {
    case 8:
        return Pixel::SizedFormat::Uint8_NormalizedR;
    case 24:
        return Pixel::SizedFormat::Uint8_NormalizedRGB;
    case 32:
        return Pixel::SizedFormat::Uint8_NormalizedRGBA;
    default:
        return Pixel::SizedFormat::Unknown;
    }
}

void BMP::parse(std::shared_ptr<Image> image)
{
    t_bmp_parser parser;
    GLenum format[2];

    try {
        read_data(&parser, image->GetPath());
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Error parsing ") + image->GetPath().string() + " : " + e.what());
    }
    image->SetSize(glm::ivec2(parser.info.width, parser.info.height));
    image->SetPixelDescription(GetBMPPixelFormat(parser.info.bpp));
    image->SetData(parser.data);
    image->SetLoaded(true);
}
