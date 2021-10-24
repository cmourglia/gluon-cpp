#include <Gluon/Widgets/Gluon_Rectangle.h>

#include <Gluon/Widgets/Gluon_Hashes.h>

namespace Utils
{
glm::vec4 ExtractColor(const Beard::Array<ZToken>& Tokens);
}

void ZRectangle::ParserPropertyInternal(Parser::ZNode::Ptr Node, const u32 node_hash)
{
	if (node_hash == static_cast<u32>(ENodeHash::Color))
	{
		ASSERT(!Node->Children.IsEmpty(), "No Children is bad for a property");
		FillColor = Utils::ExtractColor(Node->Children[0]->AssociatedTokens);
	}
}

void ZRectangle::BuildRenderInfosInternal(Beard::Array<RectangleInfo>* Result)
{
	RectangleInfo rect = {
	    .Position  = Pos,
	    .Size      = Size,
	    .FillColor = FillColor,
	};

	Result->Add(rect);
}
