#pragma once

#include <gluon/lang/value.h>

#include <beard/containers/array.h>
#include <beard/core/macros.h>

#include <memory>
#include <string>

template <typename T, typename... Args>
std::unique_ptr<T> Make(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

class ZInterpreter;

#if 0
// Generic ZASTNode. Basically does nothing.
struct ZASTNode
{
	DEFAULT_CTORS(ZASTNode);

	virtual ~ZASTNode() = default;

	virtual void Dump(i32 indent) const;

	virtual ZValue Execute(ZInterpreter* interpreter)
	{
		UNUSED(interpreter);
		return ZValue::Undefined;
	}

protected:
	ZASTNode() = default;

private:
};

using ZASTNodePtr = std::unique_ptr<ZASTNode>;

// ZNode with a body and other stuff at some point
struct ZScopeNode : public ZASTNode
{
	void Dump(i32 indent) const override;

	template <typename T, typename... Args>
	T* add(Args&&... args)
	{
		m_Body.Emplace(std::make_unique<T>(std::forward<Args>(args)...));
		return (T*)m_Body.Last().get();
	}

	const beard::Array<ZASTNodePtr>& Children() const
	{
		return m_Body;
	}

	ZValue Execute(ZInterpreter* interpreter) override;

protected:
	ZScopeNode() = default;

private:
	beard::Array<ZASTNodePtr> m_Body;
};

using ScopeNodePtr = std::unique_ptr<ZScopeNode>;

struct ZProgram : public ZScopeNode
{
};

struct ZBlockStatement : public ZScopeNode
{
};

using ZBlockStatementPtr = std::unique_ptr<ZBlockStatement>;

struct ZFunctionDeclaration : public ZASTNode
{
	explicit ZFunctionDeclaration(std::string Name)
	    : m_Name{std::move(Name)}
	    , m_Body{Make<ZBlockStatement>()}
	{
	}

	ZBlockStatement* Body()
	{
		return m_Body.get();
	}

	void Dump(i32 indent) const override;

	ZValue Execute(ZInterpreter* interpreter) override;

private:
	std::string        m_Name;
	ZBlockStatementPtr m_Body = nullptr;
};

struct ZExpression : public ZASTNode
{
private:
};

using ZExpressionPtr = std::unique_ptr<ZExpression>;

struct ExpressionStatement : public ZASTNode
{
	explicit ExpressionStatement(ZExpressionPtr expression)
	    : m_Expression{std::move(expression)}
	{
	}

	ZValue Execute(ZInterpreter* interpreter) override;

	void Dump(i32 indent) const override;

private:
	ZExpressionPtr m_Expression;
};

struct ZReturnStatement : public ZASTNode
{
	explicit ZReturnStatement(ZExpressionPtr&& argument)
	    : m_Argument(std::move(argument))
	{
	}

	ZValue Execute(ZInterpreter* interpreter) override;

	void Dump(i32 indent) const override;

private:
	ZExpressionPtr m_Argument = nullptr;
};

enum class EBinaryOp
{
	Addition,     // +
	Substraction, // -
#	if 0
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
#	endif
};

struct ZBinaryExpression : public ZExpression
{
	ZBinaryExpression(EBinaryOp Operator, ZExpressionPtr&& Left, ZExpressionPtr&& Right)
	    : m_Op(Operator)
	    , m_LHS(std::move(Left))
	    , m_RHS(std::move(Right))
	{
	}

	ZValue Execute(ZInterpreter* interpreter) override;

	void Dump(i32 indent) const override;

private:
	EBinaryOp      m_Op  = EBinaryOp::Addition;
	ZExpressionPtr m_LHS = nullptr;
	ZExpressionPtr m_RHS = nullptr;
};

struct ZCallExpression : public ZExpression
{
	explicit ZCallExpression(std::string callee)
	    : m_Callee(std::move(callee))
	{
	}

	ZValue Execute(ZInterpreter* interpreter) override;

	void Dump(i32 indent) const override;

private:
	std::string m_Callee;
};

struct ZIdentifier : public ZExpression
{
	explicit ZIdentifier(std::string Name)
	    : m_Name(std::move(Name))
	{
	}

	const std::string& Name() const
	{
		return m_Name;
	}

	ZValue Execute(ZInterpreter* interpreter) override;

	void Dump(i32 indent) const override;

private:
	std::string m_Name;
};
using ZIdentifierPtr = std::unique_ptr<ZIdentifier>;

struct ZLiteral : public ZExpression
{
	explicit ZLiteral(ZValue value)
	    : m_Value(value)
	{
	}

	void Dump(i32 indent) const override;

	ZValue Execute(ZInterpreter* interpreter) override;

private:
	ZValue m_Value;
};

enum class EAssignmentOperator
{
	Assign,
};

struct AssignmentExpression : public ZExpression
{
	AssignmentExpression(EAssignmentOperator Operator, ZIdentifierPtr Left, ZExpressionPtr Right)
	    : m_Op{Operator}
	    , m_LHS{std::move(Left)}
	    , m_RHS{std::move(Right)}
	{
	}

	void Dump(i32 indent) const override;

	ZValue Execute(ZInterpreter* interpreter) override;

private:
	EAssignmentOperator m_Op;
	ZIdentifierPtr      m_LHS;
	ZExpressionPtr      m_RHS;
};

struct ZVariableDeclaration : public ZASTNode
{
	explicit ZVariableDeclaration(ZIdentifierPtr identifier, ZExpressionPtr initializer = nullptr)
	    : m_Identifier{std::move(identifier)}
	    , m_Initializer{std::move(initializer)}
	{
	}

	void Dump(i32 indent) const override;

	ZValue Execute(ZInterpreter* intepreter) override;

private:
	ZIdentifierPtr m_Identifier;
	ZExpressionPtr m_Initializer = nullptr;
};
#endif