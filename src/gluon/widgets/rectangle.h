#pragma once

#include <Gluon/Widgets/widget.h>

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
	virtual void BuildRenderInfosInternal(beard::array<RectangleInfo>* Result) override;
};
