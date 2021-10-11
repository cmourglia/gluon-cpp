#pragma once

#include <Gluon/Widgets/Widget.h>

struct GluonRectangle : public GluonWidget
{
	static GluonWidget* create() { return new GluonRectangle{}; }

	glm::vec4 fill_color;
	glm::vec4 border_color;

private:
	virtual void parse_property_internal(Parser::Node::Ptr node,
	                                     const u32         node_hash) override;
	virtual void build_render_infos_internal(
	    std::vector<RectangleInfo>* result) override;
};
