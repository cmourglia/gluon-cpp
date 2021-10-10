#include "Gluon/Widgets/Rectangle.h"

#include "Gluon/Widgets/Hashes.h"

namespace Utils
{
glm::vec4 ExtractColor(const std::vector<Token>& tokens);
}

void GluonRectangle::ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash)
{
	if (nodeHash == NodeHash::Color)
	{
		Assert(!node->children.empty(), "No children is bad for a property");
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
