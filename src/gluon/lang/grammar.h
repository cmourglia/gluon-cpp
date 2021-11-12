#pragma once

#include <gluon/lang/token.h>
#include <gluon/lang/value.h>

#include <beard/core/macros.h>

#include <memory>

class ZBinary;
class ZGrouping;
class ZLiteral;
class ZUnary;

class IExpressionVisitor
{
public:
    virtual ZValue VisitBinary(ZBinary& Binary)       = 0;
    virtual ZValue VisitGrouping(ZGrouping& Grouping) = 0;
    virtual ZValue VisitLiteral(ZLiteral& Literal)    = 0;
    virtual ZValue VisitUnary(ZUnary& Unary)          = 0;
};

class ZExpression
{
public:
    NONCOPYABLE(ZExpression);
    NONMOVEABLE(ZExpression);
    ZExpression()          = default;
    virtual ~ZExpression() = default;

    virtual ZValue Accept(IExpressionVisitor& visitor) = 0;
};

class ZBinary : public ZExpression
{
public:
    explicit ZBinary(std::unique_ptr<ZExpression> Left, ZToken Operator, std::unique_ptr<ZExpression> Right)
        : m_Left{std::move(Left)}
        , m_Operator{std::move(Operator)}
        , m_Right{std::move(Right)}
    {
    }

    ZValue Accept(IExpressionVisitor& Visitor) override
    {
        return Visitor.VisitBinary(*this);
    }

    ZExpression* Left() const
    {
        return m_Left.get();
    }

    ZToken Operator() const
    {
        return m_Operator;
    }

    ZExpression* Right() const
    {
        return m_Right.get();
    }

private:
    std::unique_ptr<ZExpression> m_Left;
    ZToken                       m_Operator;
    std::unique_ptr<ZExpression> m_Right;
};

class ZGrouping : public ZExpression
{
public:
    explicit ZGrouping(std::unique_ptr<ZExpression> Expression)
        : m_Expression{std::move(Expression)}
    {
    }

    ZValue Accept(IExpressionVisitor& Visitor) override
    {
        return Visitor.VisitGrouping(*this);
    }

    ZExpression* Expression() const
    {
        return m_Expression.get();
    }

private:
    std::unique_ptr<ZExpression> m_Expression;
};

class ZLiteral : public ZExpression
{
public:
    explicit ZLiteral(ZValue Value)
        : m_Value{Value}
    {
    }

    ZValue Accept(IExpressionVisitor& Visitor) override
    {
        return Visitor.VisitLiteral(*this);
    }

    ZValue Value() const
    {
        return m_Value;
    }

private:
    ZValue m_Value;
};

class ZUnary : public ZExpression
{
public:
    explicit ZUnary(ZToken Operator, std::unique_ptr<ZExpression> Right)
        : m_Operator{std::move(Operator)}
        , m_Right{std::move(Right)}
    {
    }

    ZValue Accept(IExpressionVisitor& Visitor) override
    {
        return Visitor.VisitUnary(*this);
    }

    ZToken Operator() const
    {
        return m_Operator;
    }

    ZExpression* Right() const
    {
        return m_Right.get();
    }

private:
    ZToken                       m_Operator;
    std::unique_ptr<ZExpression> m_Right;
};

class IStatementVisitor
{
public:
};

class ZStatement
{
public:
    NONCOPYABLE(ZStatement);
    NONMOVEABLE(ZStatement);
    ZStatement()          = default;
    virtual ~ZStatement() = default;

    virtual ZValue Accept(IStatementVisitor& visitor) = 0;
};
