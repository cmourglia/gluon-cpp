#pragma once

#include "Gluon/Widgets/Widget.h"

struct GluonRectangle : public GluonWidget
{
	static GluonWidget* Create()
	{
		return new GluonRectangle{};
	}

	glm::vec4 fillColor;
	glm::vec4 borderColor;

private:
	virtual void ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash) override;
	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) override;
};
