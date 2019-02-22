/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cubemap.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 10:47:55 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Texture.hpp"
#include <array>

class Cubemap : public Texture {
public:
    static std::shared_ptr<Cubemap> create(const std::string&);
    static std::shared_ptr<Cubemap> create(const std::string&, std::shared_ptr<Texture> fromTexture);
    static std::shared_ptr<Cubemap> parse(const std::string&, const std::string&);
    static std::shared_ptr<Cubemap> get(unsigned index);
    static std::shared_ptr<Cubemap> get_by_name(const std::string&);
    void load();
    void unload();
    std::shared_ptr<Texture> side(unsigned index);
    void set_side(unsigned index, std::shared_ptr<Texture>);

private:
    static std::vector<std::shared_ptr<Cubemap>> _cubemaps;
    std::array<std::weak_ptr<Texture>, 6> _sides;
    Cubemap(const std::string&);
};