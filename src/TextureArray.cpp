/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-28 13:41:22
*/

#include "TextureArray.hpp"
#include <ext/alloc_traits.h>  // for __alloc_traits<>::value_type
#include "Debug.hpp"           // for glCheckError

TextureArray::TextureArray(const std::string& name, glm::ivec2 s, GLenum target, GLenum fi, unsigned capacity)
    : Texture(name)
{
    _size = s;
    _target = target;
    _internal_format = fi;
    _capacity = capacity;
    _textures.resize(capacity);
}

std::shared_ptr<TextureArray> TextureArray::create(const std::string& name, glm::ivec2 s, GLenum target, GLenum fi, unsigned capacity)
{
    auto t = std::shared_ptr<TextureArray>(new TextureArray(name, s, target, fi, capacity));
    glGenTextures(1, &t->_glid);
    glObjectLabel(GL_TEXTURE, t->_glid, -1, t->name().c_str());
    glBindTexture(t->_target, t->_glid);
    glTexStorage3D(t->_target, 1, t->_internal_format, t->_size.x, t->_size.y, t->_capacity);
    glBindTexture(t->_target, 0);
    t->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    t->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    t->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    t->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    t->_array.resize(capacity);
    _textures.push_back(std::static_pointer_cast<Texture>(t));
    glCheckError();
    return (t);
}

/*int			TextureArray::add(Texture *texture)
{
	auto	search = std::find(_textures.begin(), _textures.end(), texture);
	if (search != _textures.end())
		return (search - _textures.begin());
	if (_textures.size() == _capacity)
		return (-1); //Texture array is full
	_textures.push_back(texture);
	return (_textures.size() - 1);
}*/

void TextureArray::set(std::shared_ptr<Texture> texture, int index)
{
    _array.at(index) = texture;
}

void TextureArray::load()
{
    if (_loaded)
        return;
    glBindTexture(_target, _glid);
    for (auto index = 0u; index < _textures.size(); index++) {
        auto t = _textures.at(index);
        if (nullptr == t || nullptr == t->data())
            continue;
        GLint level = 0; // Which mipmap is to be used in texture ?
        GLint xoffset = 0;
        GLint yoffset = 0;
        GLint zoffset = index; // Which texture is to be set ?
        GLsizei depth = 1; // Set only one texture
        glTexSubImage3D(_target, level, xoffset, yoffset, zoffset, t->size().x, t->size().y, depth, t->format(), t->data_format(), t->data());
    }
    glBindTexture(_target, 0);
    _loaded = true;
    glCheckError();
}