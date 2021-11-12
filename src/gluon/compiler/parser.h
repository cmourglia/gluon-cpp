#pragma once

#include <gluon/compiler/tokenizer.h>

#include <gluon/core/types.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

struct ZParsedApplication
{
	std::optional<std::string> applicationName{};
};

struct ZWidget;

ZWidget* parse_gluon_buffer(const char* Buffer);

namespace Parser
{
enum class ENodeType
{
	Unknown,

	Structure,
	Property,
	ZValue,
};

struct ZNode
{
	using Ptr = std::shared_ptr<ZNode>;

	explicit ZNode(ENodeType t)
	    : Type(t)
	{
	}

	ENodeType   Type;
	std::string Name;

	beard::array<ZToken> AssociatedTokens;

	ZNode*                   Parent;
	beard::array<ZNode::Ptr> Children;
};

struct StructureNode : public ZNode
{
	StructureNode()
	    : ZNode(ENodeType::Structure)
	{
	}
};

struct PropertyNode : public ZNode
{
	PropertyNode()
	    : ZNode(ENodeType::Property)
	{
	}
};

struct ValueNode : ZNode
{
	ValueNode()
	    : ZNode(ENodeType::ZValue)
	{
	}
};
}