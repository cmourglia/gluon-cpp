#pragma once

#include <Gluon/Compiler/Gluon_Tokenizer.h>

#include <Beard/Array.h>

#include <memory>

struct ZWidget;

namespace ShuntingYard
{
enum class ENodeType
{
	Constant,
	Operator,
	ZFunction,
};

enum class EOperator
{
	Add,
	Substract,
	Multiply,
	Divide,
	OpenParen,
	CloseParen,
};

enum class EFunction
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

struct ZNode
{
	explicit ZNode(ENodeType Type)
	    : Type{Type}
	{
	}

	ENodeType Type;
};

struct ConstantNode : public ZNode
{
	explicit ConstantNode(f32 Value = 0.0f)
	    : ZNode{ENodeType::Constant}
	    , Constant{Value}
	{
	}

	f32 Constant;
};

struct OperatorNode : public ZNode
{
	OperatorNode(EOperator Operator, bool bUnary)
	    : ZNode(ENodeType::Operator)
	    , Operator{Operator}
	    , bUnary{bUnary}
	{
	}

	EOperator Operator;
	bool      bUnary = false;
};

struct FunctionNode : public ZNode
{
	FunctionNode(EFunction Function, ZWidget* Widget)
	    : ZNode{ENodeType::ZFunction}
	    , Function{Function}
	    , Widget{Widget}
	{
	}

	EFunction Function;
	ZWidget*  Widget;
};

struct ZExpression
{
	Beard::Array<std::shared_ptr<ZNode>> EvaluationQueue;

	static ZExpression build(const Beard::Array<ZToken>& Tokens, ZWidget* RootWidget, ZWidget* CurrentWidget);

	static ZExpression Zero()
	{
		ZExpression Result;
		Result.EvaluationQueue.Add(std::make_shared<ConstantNode>(0.0f));
		return Result;
	}

	f32 Evaluate() const;

private:
	static f32 EvaluateOperator(OperatorNode* Operator, f32 Left, f32 Right);
	static f32 EvaluateFunction(FunctionNode* Function);
};
}