#include <Gluon/Widgets/rectangle.h>

#include <Gluon/Widgets/hashes.h>

namespace Utils
{
glm::vec4 ExtractColor(const beard::array<ZToken>& Tokens);
}

void ZRectangle::ParserPropertyInternal(Parser::ZNode::Ptr Node, const u32 node_hash)
{
	if (node_hash == static_cast<u32>(ENodeHash::Color))
	{
		ASSERT(!Node->Children.is_empty(), "No Children is bad for a property");
		FillColor = Utils::ExtractColor(Node->Children[0]->AssociatedTokens);
	}
}

void ZRectangle::BuildRenderInfosInternal(beard::array<RectangleInfo>* Result)
{
	RectangleInfo rect = {
	    .Position  = Pos,
	    .Size      = Size,
	    .FillColor = FillColor,
	};

	Result->add(rect);
}
