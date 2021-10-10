#pragma once

#include <Gluon/Compiler/Parser.h>
#include <Gluon/Compiler/ShuntingYard.h>

#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <string>

struct GluonWidget
{
	GluonWidget() = default;
	virtual ~GluonWidget();

	NONMOVEABLE(GluonWidget);
	NONCOPYABLE(GluonWidget);

	virtual void Deserialize(Parser::Node::Ptr node) final;
	virtual void BuildRenderInfos(std::vector<RectangleInfo>* result) final;

	virtual bool WindowResized(i32 newWidth, i32 newHeight);
	virtual void Touch() final;

	std::vector<GluonWidget*> children;
	GluonWidget*              parent = nullptr;

	std::string id;

	glm::vec2 pos  = glm::vec2(0.0f);
	glm::vec2 size = glm::vec2(0.0f);

	std::unordered_set<std::string>             dependencyIds;
	std::unordered_map<u32, std::vector<Token>> geometryExpressions;

	std::vector<GluonWidget*> dependencies;
	std::vector<GluonWidget*> dependants;
	bool                      dirty = true;

	std::vector<std::pair<f32*, ShuntingYard::Expression>> evaluators;

	static void Evaluate();

protected:
	static std::vector<GluonWidget*> widgetMap;

	virtual void EvaluateInternal();

private:
	virtual void ParseProperty(Parser::Node::Ptr node) final;
	virtual void ParsePropertyInternal(Parser::Node::Ptr node,
	                                   u32               nodeHash) = 0;

	virtual void BuildRenderInfosInternal(
	    std::vector<RectangleInfo>* result) = 0;

	virtual void PreEvaluate() { }
	virtual void PostEvaluate() { }
};

using WidgetFactory = GluonWidget*();

GluonWidget* GetWidgetById(GluonWidget* rootWidget, const std::string& name);
void BuildDependencyGraph(GluonWidget* rootWidget, GluonWidget* currentWidget);
void BuildExpressionEvaluators(GluonWidget* rootWidget,
                               GluonWidget* currentWidget);
