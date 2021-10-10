#pragma once

#include <Gluon/Widgets/Widget.h>

struct GluonText : public GluonWidget
{
	static GluonWidget* Create() { return new GluonText{}; }

	std::string content;
	std::string font;
	u32         pointSize = 16;
	glm::vec4   textColor = MuColor::Black;

private:
	void ParsePropertyInternal(Parser::Node::Ptr node, u32 nodeHash) override;
	void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) override;
};