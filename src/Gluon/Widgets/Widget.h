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

	virtual void deserialize(Parser::Node::Ptr node) final;
	virtual void build_render_infos(std::vector<RectangleInfo>* result) final;

	virtual bool window_resized(i32 newWidth, i32 newHeight);
	virtual void touch() final;

	std::vector<GluonWidget*> children;
	GluonWidget*              parent = nullptr;

	std::string id;

	glm::vec2 pos  = glm::vec2(0.0f);
	glm::vec2 size = glm::vec2(0.0f);

	std::unordered_set<std::string>             dependency_ids;
	std::unordered_map<u32, std::vector<Token>> geometry_expressions;

	std::vector<GluonWidget*> dependencies;
	std::vector<GluonWidget*> dependants;
	bool                      dirty = true;

	std::vector<std::pair<f32*, ShuntingYard::Expression>> evaluators;

	static void evaluate();

protected:
	static std::vector<GluonWidget*> s_widget_map;

	virtual void evaluate_internal();

private:
	virtual void parse_property(Parser::Node::Ptr node) final;
	virtual void parse_property_internal(Parser::Node::Ptr node,
	                                     u32               node_hash) = 0;

	virtual void build_render_infos_internal(
	    std::vector<RectangleInfo>* result) = 0;

	virtual void pre_evaluate() { }
	virtual void post_evaluate() { }
};

using WidgetFactory = GluonWidget*();

GluonWidget* get_widget_by_id(GluonWidget*       root_widget,
                              const std::string& name);
void         build_dependency_graph(GluonWidget* root_widget,
                                    GluonWidget* current_widget);
void         build_expression_evaluators(GluonWidget* root_widget,
                                         GluonWidget* current_widget);
