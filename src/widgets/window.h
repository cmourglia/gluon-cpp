#pragma once

#include "widgets/widget.h"

struct GluonWindow : public GluonWidget
{
	static GluonWidget* Create()
	{
		return new GluonWindow{};
	}

	GluonWindow();

	virtual bool WindowResized(i32 w, i32 h) override;

	std::string title           = "Gluon";
	glm::vec4   backgroundColor = MuColor::Chocolate;

private:
	virtual void ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash) override;
	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) override;

	virtual void PreEvaluate() override;
	virtual void PostEvaluate() override;
};