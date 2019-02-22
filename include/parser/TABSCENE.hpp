/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TABSCENE.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anonymous <anonymous@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/30 19:06:10 by anonymous         #+#    #+#             */
/*   Updated: 2018/07/30 19:10:43 by anonymous        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

/*
** TABSCENE parsing interface
*/
class TABSCENE {
public:
    static void parse(const std::string& path);

private:
    virtual void abstract() = 0;
};