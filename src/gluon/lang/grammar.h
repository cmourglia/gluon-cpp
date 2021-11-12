#pragma once

#include <gluon/lang/token.h>
#include <gluon/lang/value.h>

#include <beard/core/macros.h>

#include <memory>

class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class UnaryExpr;

class ExprVisitor
{
public:
    virtual Value VisitBinary(BinaryExpr& Binary)       = 0;
    virtual Value VisitGrouping(GroupingExpr& Grouping) = 0;
    virtual Value VisitLiteral(LiteralExpr& Literal)    = 0;
    virtual Value VisitUnary(UnaryExpr& Unary)          = 0;
};

class Expr
{
public:
    NONCOPYABLE(Expr);
    NONMOVEABLE(Expr);
    Expr()          = default;
    virtual ~Expr() = default;

    virtual Value Accept(ExprVisitor& visitor) = 0;
};

class BinaryExpr : public Expr
{
public:
    explicit BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : m_left{std::move(left)}
        , m_op{std::move(op)}
        , m_right{std::move(right)}
    {
    }

    Value Accept(ExprVisitor& Visitor) override
    {
        return Visitor.VisitBinary(*this);
    }

    Expr* left() const
    {
        return m_left.get();
    }

    Token op() const
    {
        return m_op;
    }

    Expr* right() const
    {
        return m_right.get();
    }

private:
    std::unique_ptr<Expr> m_left;
    Token                 m_op;
    std::unique_ptr<Expr> m_right;
};

class GroupingExpr : public Expr
{
public:
    explicit GroupingExpr(std::unique_ptr<Expr> expr)
        : m_expr{std::move(expr)}
    {
    }

    Value Accept(ExprVisitor& Visitor) override
    {
        return Visitor.VisitGrouping(*this);
    }

    Expr* expr() const
    {
        return m_expr.get();
    }

private:
    std::unique_ptr<Expr> m_expr;
};

class LiteralExpr : public Expr
{
public:
    explicit LiteralExpr(Value value)
        : m_value{value}
    {
    }

    Value Accept(ExprVisitor& Visitor) override
    {
        return Visitor.VisitLiteral(*this);
    }

    Value value() const
    {
        return m_value;
    }

private:
    Value m_value;
};

class UnaryExpr : public Expr
{
public:
    explicit UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : m_op{std::move(op)}
        , m_right{std::move(right)}
    {
    }

    Value Accept(ExprVisitor& Visitor) override
    {
        return Visitor.VisitUnary(*this);
    }

    Token op() const
    {
        return m_op;
    }

    Expr* right() const
    {
        return m_right.get();
    }

private:
    Token                 m_op;
    std::unique_ptr<Expr> m_right;
};

class StmtVisitor
{
public:
};

class Stmt
{
public:
    NONCOPYABLE(Stmt);
    NONMOVEABLE(Stmt);
    Stmt()          = default;
    virtual ~Stmt() = default;

    virtual Value Accept(StmtVisitor& visitor) = 0;
};
