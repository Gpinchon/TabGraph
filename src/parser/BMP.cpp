/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 17:35:50
*/

#include "parser/BMP.hpp"
#include <GL/glew.h>                 // for GLubyte, GLenum, GL_BGR, GL_BGRA
#include <bits/exception.h>          // for exception
#include <errno.h>                   // for errno
#include <fcntl.h>                   // for O_BINARY, O_CREAT, O_RDWR
#include <io.h>                      // for write, access, close, open, R_OK
#include <stdio.h>                   // for fclose, fread, fopen, fseek, SEE...
#include <string.h>                  // for memset, strerror
#include <sys/stat.h>                // for S_IRWXG, S_IRWXO, S_IRWXU
#include <stdexcept>                 // for runtime_error
#include "Texture.hpp"               // for Texture
#include "parser/InternalTools.hpp"  // for t_bmp_parser, t_bmp_info, t_bmp_...
#include "glm"                     // for s_vec2, glm::vec2

static void prepare_header(t_bmp_header* header, t_bmp_info* info, std::shared_ptr<Texture> t)
{
    memset(header, 0, sizeof(t_bmp_header));
    memset(info, 0, sizeof(t_bmp_info));
    header->type = 0x4D42;
    header->data_offset = sizeof(t_bmp_header) + sizeof(t_bmp_info);
    header->size = header->data_offset + (t->size().x * t->size().y * 4);
    info->header_size = sizeof(t_bmp_info);
    info->width = t->size().x;
    info->height = t->size().y;
    info->color_planes = 1;
    info->bpp = t->bpp();
    info->size = t->size().x * t->size().y * 4;
    info->horizontal_resolution = 0x0ec4;
    info->vertical_resolution = 0x0ec4;
}

void BMP::save(std::shared_ptr<Texture> t, const std::string& imagepath)
{
    t_bmp_header header;
    t_bmp_info info;
    GLubyte* padding;
    int fd;

    prepare_header(&header, &info, t);
    fd = open(imagepath.c_str(), O_RDWR | O_CREAT | O_BINARY,
        S_IRWXU | S_IRWXG | S_IRWXO);
    write(fd, &header, sizeof(t_bmp_header));
    write(fd, &info, sizeof(t_bmp_info));
    write(fd, t->data(), (t->size().x * t->size().y * t->bpp() / 8));
    padding = new GLubyte[int(info.size - (t->size().x * t->size().y * t->bpp() / 8))]();
    write(fd, padding, info.size - (t->size().x * t->size().y * t->bpp() / 8));
    delete[] padding;
    close(fd);
}

static void convert_bmp(t_bmp_parser* parser)
{
    GLubyte* pixel_temp;
    GLubyte rgba[4];
    int i[3];

    auto dataSize = parser->info.bpp / 8 * parser->info.width * parser->info.height;
    pixel_temp = new GLubyte[dataSize];
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
    delete[] parser->data;
    parser->data = pixel_temp;
}

static int read_data(t_bmp_parser* p, const std::string& path)
{
    unsigned data_size;

    if (access(path.c_str(), R_OK) != 0) {
        throw std::runtime_error(strerror(errno));
    }
    if ((p->fd = fopen(path.c_str(), "rb")) == nullptr) {
        throw std::runtime_error(strerror(errno));
    }
    auto readReturn = fread(&p->header, 1, sizeof(p->header), p->fd);
    if (readReturn != sizeof(p->header) || p->header.type != 0x4D42) {
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
    p->data = new GLubyte[data_size];
    p->size_read = fread(p->data, sizeof(unsigned char), data_size, p->fd);
    fclose(p->fd);
    if (p->info.bpp == 32) {
        convert_bmp(p);
    }
    return (0);
}

static void get_format(GLubyte bpp, GLenum* format, GLenum* internal_format)
{
    if (bpp == 8) {
        *format = GL_RED;
        *internal_format = GL_COMPRESSED_RED;
    } else if (bpp == 24) {
        *format = GL_BGR;
        *internal_format = GL_COMPRESSED_RGB;
    } else if (bpp == 32) {
        *format = GL_BGRA;
        *internal_format = GL_COMPRESSED_RGBA;
    } else {
        *format = 0;
        *internal_format = 0;
    }
}

std::shared_ptr<Texture> BMP::parse(const std::string& texture_name, const std::string& path)
{
    t_bmp_parser parser;
    GLenum format[2];

    try {
        read_data(&parser, path);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Error parsing ") + path + " : " + e.what());
    }
    get_format(parser.info.bpp, &format[0], &format[1]);
    auto t = Texture::create(texture_name, glm::vec2(parser.info.width, parser.info.height),
        GL_TEXTURE_2D, format[0], format[1], GL_UNSIGNED_BYTE, parser.data);
    t->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    return (t);
}
