#pragma once

#include <gluon/lang/token.h>
#include <gluon/lang/value.h>

#include <beard/containers/array.h>
#include <beard/core/macros.h>
#include <beard/misc/optional.h>

#include <memory>

class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class UnaryExpr;
class VariableExpr;
class AssignExpr;

class ExprVisitor
{
public:
    virtual Value VisitBinary(BinaryExpr& binary)       = 0;
    virtual Value VisitGrouping(GroupingExpr& grouping) = 0;
    virtual Value VisitLiteral(LiteralExpr& literal)    = 0;
    virtual Value VisitUnary(UnaryExpr& unary)          = 0;
    virtual Value VisitVariable(VariableExpr& variable) = 0;
    virtual Value VisitAssign(AssignExpr& assign)       = 0;
};

struct ExprType
{
    enum Enum
    {
        kBinary,
        kGrouping,
        kLiteral,
        kUnary,
        kVariable,
        kAssign
    };
};

class Expr
{
public:
    NONCOPYABLE(Expr);
    NONMOVEABLE(Expr);
    explicit Expr(ExprType::Enum expr_type)
        : m_type(expr_type)
    {
    }

    virtual ~Expr()                            = default;

    virtual Value Accept(ExprVisitor& visitor) = 0;

    [[nodiscard]] ExprType::Enum type() const { return m_type; }

private:
    ExprType::Enum m_type;
};

using ExprPtr = std::shared_ptr<Expr>;
using ExprArr = beard::array<ExprPtr>;

class BinaryExpr : public Expr
{
public:
    explicit BinaryExpr(ExprPtr left, Token op, ExprPtr right)
        : Expr{ExprType::kBinary}
        , m_left{std::move(left)}
        , m_op{std::move(op)}
        , m_right{std::move(right)}
    {
    }

    Value Accept(ExprVisitor& Visitor) override { return Visitor.VisitBinary(*this); }

    [[nodiscard]] Expr* left() const { return m_left.get(); }
    [[nodiscard]] Token op() const { return m_op; }
    [[nodiscard]] Expr* right() const { return m_right.get(); }

private:
    ExprPtr m_left;
    Token   m_op;
    ExprPtr m_right;
};

class GroupingExpr : public Expr
{
public:
    explicit GroupingExpr(ExprPtr expr)
        : Expr{ExprType::kGrouping}
        , m_expr{std::move(expr)}
    {
    }

    Value Accept(ExprVisitor& Visitor) override { return Visitor.VisitGrouping(*this); }

    [[nodiscard]] Expr* expr() const { return m_expr.get(); }

private:
    ExprPtr m_expr;
};

class LiteralExpr : public Expr
{
public:
    explicit LiteralExpr(Value value)
        : Expr{ExprType::kLiteral}
        , m_value{value}
    {
    }

    Value Accept(ExprVisitor& Visitor) override { return Visitor.VisitLiteral(*this); }

    [[nodiscard]] Value value() const { return m_value; }

private:
    Value m_value;
};

class UnaryExpr : public Expr
{
public:
    explicit UnaryExpr(Token op, ExprPtr right)
        : Expr{ExprType::kUnary}
        , m_op{std::move(op)}
        , m_right{std::move(right)}
    {
    }

    Value Accept(ExprVisitor& Visitor) override { return Visitor.VisitUnary(*this); }

    [[nodiscard]] Token op() const { return m_op; }
    [[nodiscard]] Expr* right() const { return m_right.get(); }

private:
    Token   m_op;
    ExprPtr m_right;
};

class VariableExpr : public Expr
{
public:
    explicit VariableExpr(Token name)
        : Expr{ExprType::kVariable}
        , m_name{std::move(name)}
    {
    }

    Value Accept(ExprVisitor& Visitor) override { return Visitor.VisitVariable(*this); }

    [[nodiscard]] Token name() const { return m_name; }

private:
    Token m_name;
};

class AssignExpr : public Expr
{
public:
    explicit AssignExpr(ExprPtr name, ExprPtr value)
        : Expr{ExprType::kAssign}
        , m_name{std::move(name)}
        , m_value{std::move(value)}
    {
    }

    Value Accept(ExprVisitor& Visitor) override { return Visitor.VisitAssign(*this); }

    [[nodiscard]] Expr* name() const { return m_name.get(); }
    [[nodiscard]] Expr* value() const { return m_value.get(); }

private:
    ExprPtr m_name;
    ExprPtr m_value;
};

class ExprStmt;
class PrintStmt;
class VarDeclStmt;
class BlockStmt;
class IfStmt;
class WhileStmt;

class StmtVisitor
{
public:
    virtual void VisitExpr(ExprStmt& expr)           = 0;
    virtual void VisitPrint(PrintStmt& print)        = 0;
    virtual void VisitVarDecl(VarDeclStmt& var_decl) = 0;
    virtual void VisitBlock(BlockStmt& block)        = 0;
    virtual void VisitIf(IfStmt& if_stmt)            = 0;
    virtual void VisitWhile(WhileStmt& while_stmt)   = 0;
};

struct StmtType
{
    enum Enum
    {
        kExpr,
        kPrint,
        kVarDecl,
        kBlock,
        kIf,
        kWhile,
    };
};

class Stmt
{
public:
    NONCOPYABLE(Stmt);
    NONMOVEABLE(Stmt);

    explicit Stmt(StmtType::Enum stmt_type)
        : m_type{stmt_type}
    {
    }

    virtual ~Stmt()                           = default;

    virtual void Accept(StmtVisitor& visitor) = 0;

    [[nodiscard]] StmtType::Enum type() const { return m_type; }

private:
    StmtType::Enum m_type;
};

using StmtPtr = std::shared_ptr<Stmt>;
using StmtArr = beard::array<StmtPtr>;

class ExprStmt : public Stmt
{
public:
    explicit ExprStmt(ExprPtr expr)
        : Stmt{StmtType::kExpr}
        , m_expr{std::move(expr)}
    {
    }

    void Accept(StmtVisitor& Visitor) override { Visitor.VisitExpr(*this); }

    [[nodiscard]] Expr* expr() const { return m_expr.get(); }

private:
    ExprPtr m_expr;
};

class PrintStmt : public Stmt
{
public:
    explicit PrintStmt(std::initializer_list<ExprPtr> exprs)
        : Stmt{StmtType::kPrint}
        , m_exprs{exprs}
    {
    }

    void Accept(StmtVisitor& Visitor) override { Visitor.VisitPrint(*this); }

    [[nodiscard]] const ExprArr& exprs() const { return m_exprs; }

private:
    beard::array<ExprPtr> m_exprs;
};

class VarDeclStmt : public Stmt
{
public:
    explicit VarDeclStmt(Token name, ExprPtr value)
        : Stmt{StmtType::kVarDecl}
        , m_name{std::move(name)}
        , m_value{std::move(value)}
    {
    }

    void Accept(StmtVisitor& Visitor) override { Visitor.VisitVarDecl(*this); }

    [[nodiscard]] Token name() const { return m_name; }
    [[nodiscard]] Expr* value() const { return m_value.get(); }

private:
    Token   m_name;
    ExprPtr m_value;
};

class BlockStmt : public Stmt
{
public:
    explicit BlockStmt(std::initializer_list<StmtPtr> stmts)
        : Stmt{StmtType::kBlock}
        , m_stmts{stmts}
    {
    }

    void Accept(StmtVisitor& Visitor) override { Visitor.VisitBlock(*this); }

    [[nodiscard]] const StmtArr& stmts() { return m_stmts; }

private:
    StmtArr m_stmts;
};

class IfStmt : public Stmt
{
public:
    explicit IfStmt(ExprPtr test, StmtPtr stmt, StmtPtr else_stmt)
        : Stmt{StmtType::kIf}
        , m_test{std::move(test)}
        , m_stmt{std::move(stmt)}
        , m_else_stmt{std::move(else_stmt)}
    {
    }

    void Accept(StmtVisitor& Visitor) override { Visitor.VisitIf(*this); }

    [[nodiscard]] Expr* test() const { return m_test.get(); }
    [[nodiscard]] Stmt* stmt() const { return m_stmt.get(); }
    [[nodiscard]] Stmt* else_stmt() const { return m_else_stmt.get(); }

private:
    ExprPtr m_test;
    StmtPtr m_stmt;
    StmtPtr m_else_stmt;
};

class WhileStmt : public Stmt
{
public:
    explicit WhileStmt(ExprPtr test, StmtPtr stmt)
        : Stmt{StmtType::kWhile}
        , m_test{std::move(test)}
        , m_stmt{std::move(stmt)}
    {
    }

    void Accept(StmtVisitor& Visitor) override { Visitor.VisitWhile(*this); }

    [[nodiscard]] Expr* test() const { return m_test.get(); }
    [[nodiscard]] Stmt* stmt() const { return m_stmt.get(); }

private:
    ExprPtr m_test;
    StmtPtr m_stmt;
};
