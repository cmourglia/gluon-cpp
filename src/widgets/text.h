#pragma once

#include "widgets/widget.h"

struct GluonText : public GluonWidget
{
	static GluonWidget* Create()
	{
		return new GluonText{};
	}

	std::string content   = "";
	std::string font      = "";
	u32         pointSize = 16;
	glm::vec4   textColor = MuColor::Black;

private:
	virtual void ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash) override;
	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) override;
};