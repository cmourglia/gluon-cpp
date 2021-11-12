#pragma once

#include <Gluon/Widgets/widget.h>

struct ZText : public ZWidget
{
	static ZWidget* create()
	{
		return new ZText{};
	}

	std::string content;
	std::string font;
	u32         pointSize = 16;
	glm::vec4   textColor = ZColor::Black;

private:
	void ParserPropertyInternal(Parser::ZNode::Ptr Node, u32 node_hash) override;
	void BuildRenderInfosInternal(beard::array<RectangleInfo>* Result) override;
};