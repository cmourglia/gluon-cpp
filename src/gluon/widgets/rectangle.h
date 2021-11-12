#pragma once

#include <Gluon/Widgets/widget.h>

struct Rectangle : public Widget
{
    static Widget* create()
    {
        return new Rectangle{};
    }

    glm::vec4 fill_color;
    glm::vec4 border_color;

private:
    virtual void ParserPropertyInternal(parser::Node::Ptr node, const u32 node_hash) override;
    virtual void BuildRenderInfosInternal(beard::array<RectangleInfo>* result) override;
};
