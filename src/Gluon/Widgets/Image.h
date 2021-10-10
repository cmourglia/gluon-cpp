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

	static GluonWidget* Create() { return new GluonImage{}; }

	virtual ~GluonImage();

	virtual void PostEvaluate() override;

	std::string imageUrl;
	glm::vec2   imageSize = {0.0f, 0.0f};

	FitMode fitMode = FitMode::Stretch;

	glm::vec4 imageTint = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
	ImageInfo imageInfo = {};

private:
	virtual void ParsePropertyInternal(Parser::Node::Ptr node,
	                                   const u32         nodeHash) override;
	virtual void BuildRenderInfosInternal(
	    std::vector<RectangleInfo>* result) override;
};
