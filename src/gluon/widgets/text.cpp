#include <Gluon/Widgets/text.h>

namespace Utils
{
glm::vec4 ExtractColor(const beard::array<ZToken>& Tokens);
}

void ZText::ParserPropertyInternal(Parser::ZNode::Ptr Node, u32 node_hash)
{
	UNUSED(Node);
	UNUSED(node_hash);
	// TODO
}

void ZText::BuildRenderInfosInternal(beard::array<RectangleInfo>* Result)
{
	UNUSED(Result);
	// TODO
}