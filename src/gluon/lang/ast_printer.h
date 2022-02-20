#pragma once

#include "gluon/lang/grammar.h"

namespace gluon::lang {

class AstPrinter : public ExprVisitor, public StmtVisitor {
 public:
  void PrintAST(Expr& ASTRoot);

 private:
  Value visit_binary(BinaryExpr& binary) override;
  Value visit_grouping(GroupingExpr& grouping) override;
  Value visit_literal(LiteralExpr& literal) override;
  Value visit_unary(UnaryExpr& unary) override;
  Value visit_variable(VariableExpr& expr) override;
  Value visit_assign(AssignExpr& expr) override;

  void visit_expr(ExprStmt& expr) override;
  void visit_print(PrintStmt& expr) override;
  void visit_var_decl(VarDeclStmt& expr) override;
  void visit_block(BlockStmt& block) override;
  void visit_if(IfStmt& stmt) override;
  void visit_while(WhileStmt& stmt) override;

  void add_indentation();

  std::string m_string;
  i32 m_indentation = 0;
};

}  // namespace gluon::lang