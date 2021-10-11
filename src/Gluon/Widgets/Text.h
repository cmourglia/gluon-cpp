#pragma once

#include <Gluon/Widgets/Widget.h>

struct GluonText : public GluonWidget
{
	static GluonWidget* create() { return new GluonText{}; }

	std::string content;
	std::string font;
	u32         pointSize = 16;
	glm::vec4   textColor = GluonColor::Black;

private:
	void parse_property_internal(Parser::Node::Ptr node,
	                             u32               node_hash) override;
	void build_render_infos_internal(
	    std::vector<RectangleInfo>* result) override;
};