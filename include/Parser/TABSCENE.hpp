/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:25:51
*/

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