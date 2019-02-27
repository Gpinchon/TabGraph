/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextureArray.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/17 15:56:13 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 16:59:41 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Texture.hpp"
#include <vector>

class TextureArray : public Texture {
public:
    static std::shared_ptr<TextureArray> create(const std::string& name, VEC2 s, GLenum target, GLenum fi, unsigned capacity);
    virtual void set(std::shared_ptr<Texture>, int index);
    //virtual int		add(Texture *texture);
    virtual void load();

protected:
    TextureArray(const std::string& name, VEC2 s, GLenum target, GLenum fi, unsigned capacity);
    unsigned _capacity{ 0 };
    std::vector<std::shared_ptr<Texture>> _array;
};
