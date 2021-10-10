#pragma once

#include <Gluon/Widgets/Widget.h>

struct GluonWindow : public GluonWidget
{
	static GluonWidget* Create() { return new GluonWindow{}; }

	GluonWindow();

	bool WindowResized(i32 w, i32 h) override;

	std::string title           = "Gluon";
	glm::vec4   backgroundColor = MuColor::Chocolate;

private:
	void ParsePropertyInternal(Parser::Node::Ptr node, u32 nodeHash) override;
	void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) override;

	void PreEvaluate() override;
	void PostEvaluate() override;
};