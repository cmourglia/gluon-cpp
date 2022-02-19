#pragma once

#include <Gluon/Widgets/widget.h>

struct Window : public Widget {
  static Widget* create() { return new Window{}; }

  Window();

  bool WindowResized(i32 w, i32 h) override;

  std::string title = "gluon";
  glm::vec4 background_color = color::kChocolate;

 private:
  void ParserPropertyInternal(parser::Node::Ptr node, u32 node_hash) override;
  void BuildRenderInfosInternal(beard::array<RectangleInfo>* result) override;

  void PreEvaluate() override;
  void PostEvaluate() override;
};