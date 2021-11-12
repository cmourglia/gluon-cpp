#pragma once

#include <Gluon/Compiler/parser.h>
#include <Gluon/Compiler/shunting_yard.h>

#include <beard/containers/array.h>
#include <beard/containers/hash_map.h>
#include <beard/containers/hash_set.h>

#include <memory>
#include <string>

struct ZWidget
{
	ZWidget() = default;
	virtual ~ZWidget();

	NONMOVEABLE(ZWidget);
	NONCOPYABLE(ZWidget);

	virtual void deserialize(Parser::ZNode::Ptr Node) final;
	virtual void BuildRenderInfos(beard::array<RectangleInfo>* Result) final;

	virtual bool WindowResized(i32 newWidth, i32 newHeight);
	virtual void touch() final;

	beard::array<ZWidget*> Children;
	ZWidget*               Parent = nullptr;

	std::string ID;

	glm::vec2 Pos  = glm::vec2(0.0f);
	glm::vec2 Size = glm::vec2(0.0f);

	beard::string_hash_set DependencyIDs;
	beard::hash_map<u32, beard::array<ZToken>> GeometryExpressions;

	beard::array<ZWidget*> Dependencies;
	beard::array<ZWidget*> Dependants;
	bool                   bDirty = true;

	beard::array<std::pair<f32*, ShuntingYard::ZExpression>> Evaluators;

	static void Evaluate();

protected:
	static beard::array<ZWidget*> s_WidgetMap;

	virtual void EvaluateInternal();

private:
	virtual void ParseProperty(Parser::ZNode::Ptr Node) final;
	virtual void ParserPropertyInternal(Parser::ZNode::Ptr Node, u32 node_hash) = 0;

	virtual void BuildRenderInfosInternal(beard::array<RectangleInfo>* Result) = 0;

	virtual void PreEvaluate()
	{
	}
	virtual void PostEvaluate()
	{
	}
};

using ZWidgetFactory = ZWidget*();

ZWidget* GetWidgetByID(ZWidget* RootWidget, const std::string& Name);
void     BuildDependencyGraph(ZWidget* RootWidget, ZWidget* current_widget);
void     BuildExpressionEvaluators(ZWidget* RootWidget, ZWidget* current_widget);
