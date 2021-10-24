#pragma once

#include <Gluon/Widgets/Gluon_Widget.h>

struct ZRectangle : public ZWidget
{
	static ZWidget* create()
	{
		return new ZRectangle{};
	}

	glm::vec4 FillColor;
	glm::vec4 BorderColor;

private:
	virtual void ParserPropertyInternal(Parser::ZNode::Ptr Node, const u32 node_hash) override;
	virtual void BuildRenderInfosInternal(Beard::Array<RectangleInfo>* Result) override;
};
