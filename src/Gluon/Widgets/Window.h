#pragma once

#include <Gluon/Widgets/Widget.h>

struct GluonWindow : public GluonWidget
{
	static GluonWidget* create() { return new GluonWindow{}; }

	GluonWindow();

	bool window_resized(i32 w, i32 h) override;

	std::string title            = "Gluon";
	glm::vec4   background_color = GluonColor::Chocolate;

private:
	void parse_property_internal(Parser::Node::Ptr node,
	                             u32               node_hash) override;
	void build_render_infos_internal(
	    std::vector<RectangleInfo>* result) override;

	void pre_evaluate() override;
	void post_evaluate() override;
};