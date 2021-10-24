#include <Gluon/Widgets/Gluon_Text.h>

namespace Utils
{
glm::vec4 ExtractColor(const Beard::Array<ZToken>& Tokens);
}

void ZText::ParserPropertyInternal(Parser::ZNode::Ptr Node, u32 node_hash)
{
	UNUSED(Node);
	UNUSED(node_hash);
	// TODO
}

void ZText::BuildRenderInfosInternal(Beard::Array<RectangleInfo>* Result)
{
	UNUSED(Result);
	// TODO
}