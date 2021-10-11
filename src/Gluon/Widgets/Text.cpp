#include <Gluon/Widgets/Text.h>

namespace Utils
{
glm::vec4 extract_color(const std::vector<Token>& tokens);
}

void GluonText::parse_property_internal(Parser::Node::Ptr node, u32 node_hash)
{
	UNUSED(node);
	UNUSED(node_hash);
	// TODO
}

void GluonText::build_render_infos_internal(std::vector<RectangleInfo>* result)
{
	UNUSED(result);
	// TODO
}