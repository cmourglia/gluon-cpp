#pragma once

#include <Gluon/Compiler/Gluon_Parser.h>
#include <Gluon/Compiler/Gluon_ShuntingYard.h>

#include <Beard/Array.h>
#include <Beard/HashMap.h>
#include <Beard/HashSet.h>

#include <memory>
#include <string>

struct ZWidget
{
	ZWidget() = default;
	virtual ~ZWidget();

	NONMOVEABLE(ZWidget);
	NONCOPYABLE(ZWidget);

	virtual void deserialize(Parser::ZNode::Ptr Node) final;
	virtual void BuildRenderInfos(Beard::Array<RectangleInfo>* Result) final;

	virtual bool WindowResized(i32 newWidth, i32 newHeight);
	virtual void touch() final;

	Beard::Array<ZWidget*> Children;
	ZWidget*               Parent = nullptr;

	std::string ID;

	glm::vec2 Pos  = glm::vec2(0.0f);
	glm::vec2 Size = glm::vec2(0.0f);

	Beard::StringHashSet                      DependencyIDs;
	Beard::HashMap<u32, Beard::Array<ZToken>> GeometryExpressions;

	Beard::Array<ZWidget*> Dependencies;
	Beard::Array<ZWidget*> Dependants;
	bool                   bDirty = true;

	Beard::Array<std::pair<f32*, ShuntingYard::ZExpression>> Evaluators;

	static void Evaluate();

protected:
	static Beard::Array<ZWidget*> s_WidgetMap;

	virtual void EvaluateInternal();

private:
	virtual void ParseProperty(Parser::ZNode::Ptr Node) final;
	virtual void ParserPropertyInternal(Parser::ZNode::Ptr Node, u32 node_hash) = 0;

	virtual void BuildRenderInfosInternal(Beard::Array<RectangleInfo>* Result) = 0;

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
