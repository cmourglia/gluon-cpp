#pragma once

#include <Gluon/Widgets/Widget.h>

struct NSVGimage;
struct GluonImage : public GluonWidget
{
	enum class FitMode
	{
		Stretch,
		Fit,
		Crop,
	};

	static GluonWidget* create() { return new GluonImage{}; }

	virtual ~GluonImage();

	virtual void post_evaluate() override;

	std::string imageUrl;
	glm::vec2   imageSize = {0.0f, 0.0f};

	FitMode fitMode = FitMode::Stretch;

	glm::vec4 imageTint  = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
	ImageInfo image_info = {};

private:
	virtual void parse_property_internal(Parser::Node::Ptr node,
	                                     const u32         node_hash) override;
	virtual void build_render_infos_internal(
	    std::vector<RectangleInfo>* result) override;
};
