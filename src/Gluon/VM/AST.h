#pragma once

#include <Gluon/VM/Value.h>

#include <Gluon/Core/Defines.h>
#include <Gluon/Core/Containers/DynArray.h>

#include <string>
#include <memory>

template <typename T, typename... Args>
std::shared_ptr<T> Make(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

namespace VM
{

class Interpreter;

// Generic ASTNode. Basically does nothing.
struct ASTNode
{
	DEFAULT_CTORS(ASTNode);

	virtual ~ASTNode() = default;

	virtual void Dump(i32 indent) const;

	virtual Value Execute(Interpreter* interpreter)
	{
		UNUSED(interpreter);
		return Value::Undefined;
	}

protected:
	ASTNode() = default;

private:
	virtual const char* TypeName() const = 0;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

// Node with a body and other stuff at some point
struct ScopeNode : public ASTNode
{
	void Dump(i32 indent) const override;

	template <typename T, typename... Args>
	T* Push(Args&&... args)
	{
		m_body.Emplace(std::make_unique<T>(std::forward<Args>(args)...));
		return (T*)m_body.Last().get();
	}

	const DynArray<ASTNodePtr>& GetChildren() const { return m_body; }

	Value Execute(Interpreter* interpreter) override;

protected:
	ScopeNode() = default;

private:
	const char* TypeName() const override = 0;

	DynArray<ASTNodePtr> m_body;
};

using ScopeNodePtr = std::shared_ptr<ScopeNode>;

struct Program : public ScopeNode
{
private:
	const char* TypeName() const override { return "Program"; }
};

struct FunctionDeclaration : public ASTNode
{
	explicit FunctionDeclaration(std::string name)
	    : m_name(std::move(name))
	{
	}

	void Dump(i32 indent) const override;

	template <typename T, typename... Args>
	T* MakeBody(Args&&... args)
	{
		m_body = std::make_unique<T>(std::forward<Args>(args)...);
		return (T*)m_body.get();
	}

	Value Execute(Interpreter* interpreter) override;

private:
	const char* TypeName() const override { return "FunctionDeclaration"; }

	std::string  m_name;
	ScopeNodePtr m_body = nullptr;
};

struct BlockStatement : public ScopeNode
{
private:
	const char* TypeName() const override { return "BlockStatement"; }
};

struct Expression : public ASTNode
{
private:
	const char* TypeName() const override = 0;
};

using ExpressionPtr = std::shared_ptr<Expression>;

struct ExpressionStatement : public ASTNode
{
	explicit ExpressionStatement(ExpressionPtr expression)
	    : m_expression(std::move(expression))
	{
	}

	Value Execute(Interpreter* interpreter) override;

	void Dump(i32 indent) const override;

private:
	const char* TypeName() const override { return "ExpressionStatement"; }

	ExpressionPtr m_expression;
};

struct ReturnStatement : public ASTNode
{
	explicit ReturnStatement(ExpressionPtr&& argument)
	    : m_argument(std::move(argument))
	{
	}

	Value Execute(Interpreter* interpreter) override;

	void Dump(i32 indent) const override;

private:
	const char* TypeName() const override { return "ReturnStatement"; }

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

	Value Execute(Interpreter* interpreter);

	void Dump(i32 indent) const override;

private:
	const char* TypeName() const override { return "BinaryExpression"; }

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

	Value Execute(Interpreter* interpreter) override;

	void Dump(i32 indent) const override;

private:
	const char* TypeName() const override { return "CallExpression\n"; }

	std::string m_callee;
};

struct Identifier : public Expression
{
	explicit Identifier(std::string name)
	    : m_name(std::move(name))
	{
	}

	const std::string& GetName() const { return m_name; }

	void Dump(i32 indent) const override;

private:
	const char* TypeName() const override { return "Identifier"; }

	std::string m_name;
};

struct Literal : public Expression
{
	explicit Literal(Value value)
	    : m_value(value)
	{
	}

	void Dump(i32 indent) const override;

	Value Execute(Interpreter* Interpreter) override;

private:
	const char* TypeName() const override { return "Literal"; }

	Value m_value;
};
}