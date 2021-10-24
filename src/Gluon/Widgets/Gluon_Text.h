#pragma once

#include <Gluon/Widgets/Gluon_Widget.h>

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
	void BuildRenderInfosInternal(Beard::Array<RectangleInfo>* Result) override;
};