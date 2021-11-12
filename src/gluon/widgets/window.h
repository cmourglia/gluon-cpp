#pragma once

#include <Gluon/Widgets/widget.h>

struct ZWindow : public ZWidget
{
	static ZWidget* create()
	{
		return new ZWindow{};
	}

	ZWindow();

	bool WindowResized(i32 w, i32 h) override;

	std::string Title           = "gluon";
	glm::vec4   BackgroundColor = ZColor::Chocolate;

private:
	void ParserPropertyInternal(Parser::ZNode::Ptr Node, u32 node_hash) override;
	void BuildRenderInfosInternal(beard::array<RectangleInfo>* Result) override;

	void PreEvaluate() override;
	void PostEvaluate() override;
};