#pragma once

#include "compiler/tokenizer.h"

#include <queue>
#include <memory>

struct GluonWidget;

namespace ShuntingYard
{
enum class NodeType
{
	Constant,
	Operator,
	Function,
};

enum class Operator
{
	Add,
	Substract,
	Multiply,
	Divide,
	OpenParen,
	CloseParen,
};

enum class Function
{
	Width,
	Height,
	X,
	Y,
	Bottom,
	Top,
	Left,
	Right,
};

struct Node
{
	explicit Node(NodeType _type)
	    : type(_type)
	{
	}

	NodeType type;
};

struct ConstantNode : public Node
{
	explicit ConstantNode(f32 _constant = 0.0f)
	    : Node(NodeType::Constant)
	    , constant(_constant)
	{
	}

	f32 constant;
};

struct OperatorNode : public Node
{
	OperatorNode(Operator _op, bool _unary)
	    : Node(NodeType::Operator)
	    , op(_op)
	    , unary(_unary)
	{
	}

	Operator op;
	bool     unary = false;
};

struct FunctionNode : public Node
{
	FunctionNode(Function _fn, GluonWidget* _widget)
	    : Node(NodeType::Function)
	    , fn(_fn)
	    , widget(_widget)
	{
	}

	Function     fn;
	GluonWidget* widget;
};

struct Expression
{
	std::queue<std::shared_ptr<Node>> evaluationQueue;

	static Expression Build(const std::vector<Token>& tokens, GluonWidget* rootWidget, GluonWidget* currentWidget);

	static Expression Zero()
	{
		Expression result;
		result.evaluationQueue.push(std::make_shared<ConstantNode>(0.0f));
		return result;
	}

	f32 Evaluate();

private:
	f32 EvaluateOperator(OperatorNode* op, f32 left, f32 right);
	f32 EvaluateFunction(FunctionNode* fn);
};
}