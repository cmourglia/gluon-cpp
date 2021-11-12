#pragma once

#include <gluon/compiler/tokenizer.h>

#include <beard/containers/array.h>

#include <memory>

struct Widget;

namespace shunting_yard
{
struct NodeType
{
    enum Enum
    {
        kConstant,
        kOperator,
        kFunction,
    };
};

struct Operator
{
    enum Enum
    {
        kAdd,
        kSubstract,
        kMultiply,
        kDivide,
        kOpenParen,
        kCloseParen,
    };
};

struct Function
{
    enum Enum
    {
        kWidth,
        kHeight,
        kX,
        kY,
        kBottom,
        kTop,
        kLeft,
        kRight,
    };
};

struct Node
{
    explicit Node(NodeType::Enum type)
        : type{type}
    {
    }

    NodeType::Enum type;
};

struct ConstantNode : public Node
{
    explicit ConstantNode(f32 value = 0.0f)
        : Node{NodeType::kConstant}
        , constant{value}
    {
    }

    f32 constant;
};

struct OperatorNode : public Node
{
    OperatorNode(Operator::Enum op, bool unary)
        : Node(NodeType::kOperator)
        , op{op}
        , unary{unary}
    {
    }

    Operator::Enum op;
    bool           unary = false;
};

struct FunctionNode : public Node
{
    FunctionNode(Function::Enum function, Widget* widget)
        : Node{NodeType::kFunction}
        , function{function}
        , widget{widget}
    {
    }

    Function::Enum function;
    Widget*        widget;
};

struct Expression
{
    beard::array<std::shared_ptr<Node>> evaluation_queue;

    static Expression Build(const beard::array<Token>& tokens, Widget* root_widget, Widget* current_widget);

    static Expression Zero()
    {
        Expression result;
        result.evaluation_queue.add(std::make_shared<ConstantNode>(0.0f));
        return result;
    }

    f32 Evaluate() const;

private:
    static f32 EvaluateOperator(OperatorNode* op, f32 left, f32 right);
    static f32 EvaluateFunction(FunctionNode* function);
};
}