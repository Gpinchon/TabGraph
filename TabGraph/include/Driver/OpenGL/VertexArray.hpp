/*
* @Author: gpinchon
* @Date:   2021-03-25 11:09:05
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-25 11:14:50
*/
#pragma once

#include <memory>

class TabGraph::Buffer::Accessor;

class VertexArray {
public:
    VertexArray();
    ~VertexArray();
    uint32_t GetHandle();
    VertexArray& Bind();
    VertexArray& BindAccessor(std::shared_ptr<TabGraph::Buffer::Accessor> accessor, int index);
    void Done();
    static void BindNone();

private:
    uint32_t _handle { 0 };
};