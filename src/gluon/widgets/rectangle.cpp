#include <Gluon/Widgets/rectangle.h>

#include <Gluon/Widgets/hashes.h>

namespace utils
{
glm::vec4 ExtractColor(const beard::array<Token>& tokens);
}

void Rectangle::ParserPropertyInternal(parser::Node::Ptr node, const u32 node_hash)
{
    if (node_hash == static_cast<u32>(NodeHash::kColor))
    {
        ASSERT(!node->children.is_empty(), "No Children is bad for a property");
        fill_color = utils::ExtractColor(node->children[0]->associated_tokens);
    }
}

void Rectangle::BuildRenderInfosInternal(beard::array<RectangleInfo>* result)
{
    RectangleInfo rect = {
        .position   = pos,
        .size       = size,
        .fill_color = fill_color,
    };

    result->add(rect);
}
