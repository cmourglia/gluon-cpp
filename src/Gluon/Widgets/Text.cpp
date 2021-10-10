#include <Gluon/Widgets/Text.h>

namespace Utils
{
glm::vec4 ExtractColor(const std::vector<Token>& tokens);
}

void GluonText::ParsePropertyInternal(Parser::Node::Ptr node, u32 nodeHash)
{
	UNUSED(node);
	UNUSED(nodeHash);
	// TODO
}

void GluonText::BuildRenderInfosInternal(std::vector<RectangleInfo>* result)
{
	UNUSED(result);
	// TODO
}