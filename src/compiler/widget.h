#pragma once

#include "parser.h"
#include "shuntingyard.h"

#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <string>

struct Widget
{
	using Ptr = std::shared_ptr<Widget>;

	virtual void Deserialize(Parser::Node::Ptr node) final;
	virtual void BuildRenderInfos(std::vector<RectangleInfo>* result) final;

	virtual void Evaluate() final;
	virtual void Touch() final;

	std::vector<Widget::Ptr> children;
	Widget*                  parent = nullptr;

	std::string id = "";

	glm::vec2 pos  = glm::vec2(0.0f);
	glm::vec2 size = glm::vec2(0.0f);

	std::unordered_set<std::string>             dependencyIds;
	std::unordered_map<u32, std::vector<Token>> geometryExpressions;

	std::vector<Widget*> dependencies;
	std::vector<Widget*> dependants;
	bool                 dirty = true;

	std::vector<std::pair<f32*, ShuntingYard::Expression>> evaluators;

	static std::vector<Widget*> widgetMap;

private:
	virtual void ParseProperty(Parser::Node::Ptr node) final;
	virtual void ParsePropertyInternal(Parser::Node::Ptr node, const uint32_t nodeHash) = 0;

	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) = 0;
};

typedef std::shared_ptr<Widget> WidgetFactory(void);

struct Window : public Widget
{
	static Widget::Ptr Create()
	{
		return std::make_shared<Window>();
	}

	std::string title           = "Gluon";
	glm::vec4   backgroundColor = MuColor::Chocolate;

private:
	virtual void ParsePropertyInternal(Parser::Node::Ptr node, const uint32_t nodeHash) override;
	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) override;
};

struct Rectangle : public Widget
{
	static Widget::Ptr Create()
	{
		return std::make_shared<Rectangle>();
	}

	glm::vec4 fillColor;
	glm::vec4 borderColor;

private:
	virtual void ParsePropertyInternal(Parser::Node::Ptr node, const uint32_t nodeHash) override;
	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) override;
};

Widget* GetWidgetById(Widget* rootWidget, const std::string& name);
void    BuildDependencyGraph(Widget* rootWidget, Widget* currentWidget);
void    BuildExpressionEvaluators(Widget* rootWidget, Widget* currentWidget);
