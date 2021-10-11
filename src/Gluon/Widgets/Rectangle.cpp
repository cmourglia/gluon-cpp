#include <Gluon/Widgets/Rectangle.h>

#include <Gluon/Widgets/Hashes.h>

namespace Utils
{
glm::vec4 extract_color(const std::vector<Token>& tokens);
}

void GluonRectangle::parse_property_internal(Parser::Node::Ptr node,
                                             const u32         node_hash)
{
	if (node_hash == NodeHash::Color)
	{
		ASSERT(!node->children.empty(), "No children is bad for a property");
		fill_color = Utils::extract_color(node->children[0]->associated_tokens);
	}
}

void GluonRectangle::build_render_infos_internal(
    std::vector<RectangleInfo>* result)
{
	RectangleInfo rect = {
	    .position   = pos,
	    .size       = size,
	    .fill_color = fill_color,
	};

	result->push_back(rect);
}
