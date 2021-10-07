#include "widgets/rectangle.h"

#include "widgets/hashes.h"

namespace Utils
{
glm::vec4 ExtractColor(const std::vector<Token>& tokens);
}

void GluonRectangle::ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash)
{
	if (nodeHash == NodeHash::Color)
	{
		assert(!node->children.empty());
		fillColor = Utils::ExtractColor(node->children[0]->associatedTokens);
	}
}

void GluonRectangle::BuildRenderInfosInternal(std::vector<RectangleInfo>* result)
{
	RectangleInfo rect = {
	    .position  = pos,
	    .size      = size,
	    .fillColor = fillColor,
	};

	result->push_back(rect);
}
