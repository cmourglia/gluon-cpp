#pragma once

#include "gluon/widgets/widget.h"

struct ZText : public Widget {
  static Widget* create() { return new ZText{}; }

  std::string content;
  std::string font;
  u32 pointSize = 16;
  glm::vec4 textColor = color::kBlack;

 private:
  void ParserPropertyInternal(parser::Node::Ptr Node, u32 node_hash) override;
  void BuildRenderInfosInternal(beard::array<RectangleInfo>* Result) override;
};