#pragma once

#include "Gluon/Compiler/Tokenizer.h"

#include "Gluon/Core/Types.h"

#include <vector>
#include <optional>
#include <string>
#include <memory>

struct ParsedApplication
{
	std::optional<std::string> applicationName{};
};

struct GluonWidget;

GluonWidget* ParseGluonBuffer(std::string_view buffer);

namespace Parser
{
enum NodeType
{
	Node_Unknown,

	Node_Structure,
	Node_Property,
	Node_Value,
};

struct Node
{
	using Ptr = std::shared_ptr<Node>;

	explicit Node(NodeType t)
	    : type(t)
	{
	}

	NodeType    type;
	std::string name;

	std::vector<Token> associatedTokens;

	Node*                  parent;
	std::vector<Node::Ptr> children;
};

struct StructureNode : public Node
{
	StructureNode()
	    : Node(Node_Structure)
	{
	}
};

struct PropertyNode : public Node
{
	PropertyNode()
	    : Node(Node_Property)
	{
	}
};

struct ValueNode : Node
{
	ValueNode()
	    : Node(Node_Value)
	{
	}
};
}