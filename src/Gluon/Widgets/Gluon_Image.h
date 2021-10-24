#pragma once

#include <Gluon/Widgets/Gluon_Widget.h>

struct NSVGimage;
struct ZImage : public ZWidget
{
	enum class FitMode
	{
		Stretch,
		Fit,
		Crop,
	};

	static ZWidget* create()
	{
		return new ZImage{};
	}

	virtual ~ZImage();

	virtual void PostEvaluate() override;

	std::string ImageURL;
	glm::vec2   ImageSize = {0.0f, 0.0f};

	FitMode fitMode = FitMode::Stretch;

	glm::vec4 imageTint = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
	ImageInfo ImageInfo = {};

private:
	virtual void ParserPropertyInternal(Parser::ZNode::Ptr Node, const u32 node_hash) override;
	virtual void BuildRenderInfosInternal(Beard::Array<RectangleInfo>* Result) override;
};
