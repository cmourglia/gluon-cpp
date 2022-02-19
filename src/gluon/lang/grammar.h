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

class ExprVisitor {
 public:
  virtual Value visit_binary(BinaryExpr& binary) = 0;
  virtual Value visit_grouping(GroupingExpr& grouping) = 0;
  virtual Value visit_literal(LiteralExpr& literal) = 0;
  virtual Value visit_unary(UnaryExpr& unary) = 0;
  virtual Value visit_variable(VariableExpr& variable) = 0;
  virtual Value visit_assign(AssignExpr& assign) = 0;
};

enum class ExprType { Binary, Grouping, Literal, Unary, Variable, Assign };

class Expr {
 public:
  NONCOPYABLE(Expr);
  NONMOVEABLE(Expr);
  explicit Expr(ExprType expr_type) : m_type(expr_type) {}

  virtual ~Expr() = default;

  virtual Value accept(ExprVisitor& visitor) = 0;

  [[nodiscard]] ExprType type() const { return m_type; }

 private:
  ExprType m_type;
};

using ExprPtr = std::shared_ptr<Expr>;
using ExprArr = beard::array<ExprPtr>;

class BinaryExpr : public Expr {
 public:
  explicit BinaryExpr(ExprPtr left, Token op, ExprPtr right)
      : Expr{ExprType::Binary},
        m_left{std::move(left)},
        m_op{std::move(op)},
        m_right{std::move(right)} {}

  Value accept(ExprVisitor& Visitor) override {
    return Visitor.visit_binary(*this);
  }

  [[nodiscard]] Expr* left() const { return m_left.get(); }
  [[nodiscard]] Token op() const { return m_op; }
  [[nodiscard]] Expr* right() const { return m_right.get(); }

 private:
  ExprPtr m_left;
  Token m_op;
  ExprPtr m_right;
};

class GroupingExpr : public Expr {
 public:
  explicit GroupingExpr(ExprPtr expr)
      : Expr{ExprType::Grouping}, m_expr{std::move(expr)} {}

  Value accept(ExprVisitor& Visitor) override {
    return Visitor.visit_grouping(*this);
  }

  [[nodiscard]] Expr* expr() const { return m_expr.get(); }

 private:
  ExprPtr m_expr;
};

class LiteralExpr : public Expr {
 public:
  explicit LiteralExpr(Value value) : Expr{ExprType::Literal}, m_value{value} {}

  Value accept(ExprVisitor& Visitor) override {
    return Visitor.visit_literal(*this);
  }

  [[nodiscard]] Value value() const { return m_value; }

 private:
  Value m_value;
};

class UnaryExpr : public Expr {
 public:
  explicit UnaryExpr(Token op, ExprPtr right)
      : Expr{ExprType::Unary}, m_op{std::move(op)}, m_right{std::move(right)} {}

  Value accept(ExprVisitor& Visitor) override {
    return Visitor.visit_unary(*this);
  }

  [[nodiscard]] Token op() const { return m_op; }
  [[nodiscard]] Expr* right() const { return m_right.get(); }

 private:
  Token m_op;
  ExprPtr m_right;
};

class VariableExpr : public Expr {
 public:
  explicit VariableExpr(Token name)
      : Expr{ExprType::Variable}, m_name{std::move(name)} {}

  Value accept(ExprVisitor& Visitor) override {
    return Visitor.visit_variable(*this);
  }

  [[nodiscard]] Token name() const { return m_name; }

 private:
  Token m_name;
};

class AssignExpr : public Expr {
 public:
  explicit AssignExpr(ExprPtr name, ExprPtr value)
      : Expr{ExprType::Assign},
        m_name{std::move(name)},
        m_value{std::move(value)} {}

  Value accept(ExprVisitor& Visitor) override {
    return Visitor.visit_assign(*this);
  }

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

class StmtVisitor {
 public:
  virtual void visit_expr(ExprStmt& expr) = 0;
  virtual void visit_print(PrintStmt& print) = 0;
  virtual void visit_var_decl(VarDeclStmt& var_decl) = 0;
  virtual void visit_block(BlockStmt& block) = 0;
  virtual void visit_if(IfStmt& if_stmt) = 0;
  virtual void visit_while(WhileStmt& while_stmt) = 0;
};

enum class StmtType {
  Expr,
  Print,
  VarDecl,
  Block,
  If,
  While,
};

class Stmt {
 public:
  NONCOPYABLE(Stmt);
  NONMOVEABLE(Stmt);

  explicit Stmt(StmtType stmt_type) : m_type{stmt_type} {}

  virtual ~Stmt() = default;

  virtual void Accept(StmtVisitor& visitor) = 0;

  [[nodiscard]] StmtType type() const { return m_type; }

 private:
  StmtType m_type;
};

using StmtPtr = std::shared_ptr<Stmt>;
using StmtArr = beard::array<StmtPtr>;

class ExprStmt : public Stmt {
 public:
  explicit ExprStmt(ExprPtr expr)
      : Stmt{StmtType::Expr}, m_expr{std::move(expr)} {}

  void Accept(StmtVisitor& Visitor) override { Visitor.visit_expr(*this); }

  [[nodiscard]] Expr* expr() const { return m_expr.get(); }

 private:
  ExprPtr m_expr;
};

class PrintStmt : public Stmt {
 public:
  explicit PrintStmt(std::initializer_list<ExprPtr> exprs)
      : Stmt{StmtType::Print}, m_exprs{exprs} {}

  void Accept(StmtVisitor& Visitor) override { Visitor.visit_print(*this); }

  [[nodiscard]] const ExprArr& exprs() const { return m_exprs; }

 private:
  beard::array<ExprPtr> m_exprs;
};

class VarDeclStmt : public Stmt {
 public:
  explicit VarDeclStmt(Token name, ExprPtr value)
      : Stmt{StmtType::VarDecl},
        m_name{std::move(name)},
        m_value{std::move(value)} {}

  void Accept(StmtVisitor& Visitor) override { Visitor.visit_var_decl(*this); }

  [[nodiscard]] Token name() const { return m_name; }
  [[nodiscard]] Expr* value() const { return m_value.get(); }

 private:
  Token m_name;
  ExprPtr m_value;
};

class BlockStmt : public Stmt {
 public:
  explicit BlockStmt(std::initializer_list<StmtPtr> stmts)
      : Stmt{StmtType::Block}, m_stmts{stmts} {}

  void Accept(StmtVisitor& Visitor) override { Visitor.visit_block(*this); }

  [[nodiscard]] const StmtArr& stmts() { return m_stmts; }

 private:
  StmtArr m_stmts;
};

class IfStmt : public Stmt {
 public:
  explicit IfStmt(ExprPtr test, StmtPtr stmt, StmtPtr else_stmt)
      : Stmt{StmtType::If},
        m_test{std::move(test)},
        m_stmt{std::move(stmt)},
        m_else_stmt{std::move(else_stmt)} {}

  void Accept(StmtVisitor& Visitor) override { Visitor.visit_if(*this); }

  [[nodiscard]] Expr* test() const { return m_test.get(); }
  [[nodiscard]] Stmt* stmt() const { return m_stmt.get(); }
  [[nodiscard]] Stmt* else_stmt() const { return m_else_stmt.get(); }

 private:
  ExprPtr m_test;
  StmtPtr m_stmt;
  StmtPtr m_else_stmt;
};

class WhileStmt : public Stmt {
 public:
  explicit WhileStmt(ExprPtr test, StmtPtr stmt)
      : Stmt{StmtType::While},
        m_test{std::move(test)},
        m_stmt{std::move(stmt)} {}

  void Accept(StmtVisitor& Visitor) override { Visitor.visit_while(*this); }

  [[nodiscard]] Expr* test() const { return m_test.get(); }
  [[nodiscard]] Stmt* stmt() const { return m_stmt.get(); }

 private:
  ExprPtr m_test;
  StmtPtr m_stmt;
};
