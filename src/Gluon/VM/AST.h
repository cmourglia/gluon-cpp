#pragma once

#include <Gluon/VM/Value.h>

#include <Beard/Macros.h>
#include <Beard/Array.h>

#include <string>
#include <memory>

template <typename T, typename... Args>
std::unique_ptr<T> make(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

class Interpreter;

// Generic ASTNode. Basically does nothing.
struct ASTNode
{
	DEFAULT_CTORS(ASTNode);

	virtual ~ASTNode() = default;

	virtual void dump(i32 indent) const;

	virtual Value execute(Interpreter* interpreter)
	{
		UNUSED(interpreter);
		return Value::Undefined;
	}

protected:
	ASTNode() = default;

private:
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

// Node with a body and other stuff at some point
struct ScopeNode : public ASTNode
{
	void dump(i32 indent) const override;

	template <typename T, typename... Args>
	T* add(Args&&... args)
	{
		m_body.Emplace(std::make_unique<T>(std::forward<Args>(args)...));
		return (T*)m_body.Last().get();
	}

	const Beard::Array<ASTNodePtr>& children() const { return m_body; }

	Value execute(Interpreter* interpreter) override;

protected:
	ScopeNode() = default;

private:
	Beard::Array<ASTNodePtr> m_body;
};

using ScopeNodePtr = std::unique_ptr<ScopeNode>;

struct Program : public ScopeNode
{
};

struct BlockStatement : public ScopeNode
{
};

using BlockStatementPtr = std::unique_ptr<BlockStatement>;

struct FunctionDeclaration : public ASTNode
{
	explicit FunctionDeclaration(std::string name)
	    : m_name{std::move(name)}
	    , m_body{make<BlockStatement>()}
	{
	}

	BlockStatement* body() { return m_body.get(); }

	void dump(i32 indent) const override;

	Value execute(Interpreter* interpreter) override;

private:
	std::string       m_name;
	BlockStatementPtr m_body = nullptr;
};

struct Expression : public ASTNode
{
private:
};

using ExpressionPtr = std::unique_ptr<Expression>;

struct ExpressionStatement : public ASTNode
{
	explicit ExpressionStatement(ExpressionPtr expression)
	    : m_expression{std::move(expression)}
	{
	}

	Value execute(Interpreter* interpreter) override;

	void dump(i32 indent) const override;

private:
	ExpressionPtr m_expression;
};

struct ReturnStatement : public ASTNode
{
	explicit ReturnStatement(ExpressionPtr&& argument)
	    : m_argument(std::move(argument))
	{
	}

	Value execute(Interpreter* interpreter) override;

	void dump(i32 indent) const override;

private:
	ExpressionPtr m_argument = nullptr;
};

enum class BinaryOp
{
	Addition,     // +
	Substraction, // -
#if 0
	Multiplication,   // *
	Division,         // /
	Modulo,           // %
	Exponentiation,   // ^
	Equals,           // ==
	Inequals,         // !=
	LessThan,         // <
	LessEqualThan,    // <=
	GreaterThan,      // >
	GreaterEqualThan, // <=
#endif
};

struct BinaryExpression : public Expression
{
	BinaryExpression(BinaryOp op, ExpressionPtr&& left, ExpressionPtr&& right)
	    : m_op(op)
	    , m_left(std::move(left))
	    , m_right(std::move(right))
	{
	}

	Value execute(Interpreter* interpreter) override;

	void dump(i32 indent) const override;

private:
	BinaryOp      m_op    = BinaryOp::Addition;
	ExpressionPtr m_left  = nullptr;
	ExpressionPtr m_right = nullptr;
};

struct CallExpression : public Expression
{
	explicit CallExpression(std::string callee)
	    : m_callee(std::move(callee))
	{
	}

	Value execute(Interpreter* interpreter) override;

	void dump(i32 indent) const override;

private:
	std::string m_callee;
};

struct Identifier : public Expression
{
	explicit Identifier(std::string name)
	    : m_name(std::move(name))
	{
	}

	const std::string& name() const { return m_name; }

	Value execute(Interpreter* interpreter) override;

	void dump(i32 indent) const override;

private:
	std::string m_name;
};
using IdentifierPtr = std::unique_ptr<Identifier>;

struct Literal : public Expression
{
	explicit Literal(Value value)
	    : m_value(value)
	{
	}

	void dump(i32 indent) const override;

	Value execute(Interpreter* interpreter) override;

private:
	Value m_value;
};

enum class AssignmentOperator
{
	Assign,
};

struct AssignmentExpression : public Expression
{
	AssignmentExpression(AssignmentOperator op, IdentifierPtr left, ExpressionPtr right)
	    : m_op{op}
	    , m_left{std::move(left)}
	    , m_right{std::move(right)}
	{
	}

	void dump(i32 indent) const override;

	Value execute(Interpreter* interpreter) override;

private:
	AssignmentOperator m_op;
	IdentifierPtr      m_left;
	ExpressionPtr      m_right;
};

struct VariableDeclaration : public ASTNode
{
	explicit VariableDeclaration(IdentifierPtr identifier, ExpressionPtr initializer = nullptr)
	    : m_identifier{std::move(identifier)}
	    , m_initializer{std::move(initializer)}
	{
	}

	void dump(i32 indent) const override;

	Value execute(Interpreter* intepreter) override;

private:
	IdentifierPtr m_identifier;
	ExpressionPtr m_initializer = nullptr;
};
