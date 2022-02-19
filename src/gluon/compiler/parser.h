#pragma once

#include <gluon/compiler/tokenizer.h>

#include <gluon/core/types.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

struct ParsedApplication {
  std::optional<std::string> application_name{};
};

struct Widget;

Widget* ParseGluonBuffer(const char* buffer);

namespace parser {
struct NodeType {
  enum Enum {
    kUnknown,

    kStructure,
    kProperty,
    kValue,
  };
};

struct Node {
  using Ptr = std::shared_ptr<Node>;

  explicit Node(NodeType::Enum t) : node_type(t) {}

  NodeType::Enum node_type;
  std::string name;

  beard::array<Token> associated_tokens;

  Node* parent;
  beard::array<Node::Ptr> children;
};

struct StructureNode : public Node {
  StructureNode() : Node(NodeType::kStructure) {}
};

struct PropertyNode : public Node {
  PropertyNode() : Node(NodeType::kProperty) {}
};

struct ValueNode : Node {
  ValueNode() : Node(NodeType::kValue) {}
};
}  // namespace parser