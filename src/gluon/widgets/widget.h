#pragma once

#include <Gluon/Compiler/parser.h>
#include <Gluon/Compiler/shunting_yard.h>

#include <beard/containers/array.h>
#include <beard/containers/hash_map.h>
#include <beard/containers/hash_set.h>

#include <memory>
#include <string>

struct Widget {
  Widget() = default;
  virtual ~Widget();

  NONMOVEABLE(Widget);
  NONCOPYABLE(Widget);

  virtual void deserialize(parser::Node::Ptr node) final;
  virtual void BuildRenderInfos(beard::array<RectangleInfo>* result) final;

  virtual bool WindowResized(i32 new_width, i32 new_height);
  virtual void touch() final;

  beard::array<Widget*> children;
  Widget* parent = nullptr;

  std::string id;

  glm::vec2 pos = glm::vec2(0.0f);
  glm::vec2 size = glm::vec2(0.0f);

  beard::string_hash_set dependency_ids;
  beard::hash_map<u32, beard::array<Token>> geometry_expressions;

  beard::array<Widget*> dependencies;
  beard::array<Widget*> dependees;
  bool is_dirty = true;

  beard::array<std::pair<f32*, shunting_yard::Expression>> evaluators;

  static void Evaluate();

 protected:
  static beard::array<Widget*> s_widget_map;

  virtual void EvaluateInternal();

 private:
  virtual void ParseProperty(parser::Node::Ptr node) final;
  virtual void ParserPropertyInternal(parser::Node::Ptr node,
                                      u32 node_hash) = 0;

  virtual void BuildRenderInfosInternal(
      beard::array<RectangleInfo>* result) = 0;

  virtual void PreEvaluate() {}
  virtual void PostEvaluate() {}
};

using WidgetFactory = Widget*();

Widget* GetWidgetById(Widget* root_widget, const std::string& name);
void BuildDependencyGraph(Widget* root_widget, Widget* current_widget);
void BuildExpressionEvaluators(Widget* root_widget, Widget* current_widget);
