#pragma once

#include <Gluon/Compiler/Gluon_Tokenizer.h>

#include <Gluon/Core/Gluon_Types.h>

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

	Beard::Array<ZToken> AssociatedTokens;

	ZNode*                   Parent;
	Beard::Array<ZNode::Ptr> Children;
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