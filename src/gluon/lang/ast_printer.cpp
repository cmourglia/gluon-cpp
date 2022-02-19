#include <gluon/lang/ast_printer.h>

#include <fmt/format.h>

void AstPrinter::PrintAST(Expr& Root) {
  Root.accept(*this);

  int test = 2;
  test += 3;
  test -= 42;
  printf("%d\n", test);

  std::cout << "Produced AST:" << std::endl << m_string << std::endl;
}

Value AstPrinter::visit_binary(BinaryExpr& binary) {
  add_indentation();
  m_string += "Binary\n";
  {
    m_indentation += 1;
    add_indentation();
    m_string += "Left:\n";
    {
      m_indentation += 1;
      binary.left()->accept(*this);
      m_indentation -= 1;
    }

    add_indentation();
    m_string += "Right:\n";
    {
      m_indentation += 1;
      binary.right()->accept(*this);
      m_indentation -= 1;
    }

    add_indentation();
    m_string += fmt::format("op: {}\n", binary.op().lexeme);
    m_indentation -= 1;
  }

  return Value::kUndefined;
}

Value AstPrinter::visit_grouping(GroupingExpr& grouping) {
  add_indentation();
  m_string += "Group:\n";
  m_indentation += 1;
  grouping.expr()->accept(*this);
  m_indentation -= 1;

  return Value::kUndefined;
}

Value AstPrinter::visit_literal(LiteralExpr& literal) {
  Value value = literal.value();

  add_indentation();

  m_string += "Literal: " + value.to_string() + "\n";

  return Value::kUndefined;
}

Value AstPrinter::visit_unary(UnaryExpr& unary) {
  add_indentation();
  m_string += "Binary\n";
  {
    m_indentation += 1;
    add_indentation();
    m_string += "Right:\n";
    {
      m_indentation += 1;
      unary.right()->accept(*this);
      m_indentation -= 1;
    }

    add_indentation();
    m_string += fmt::format("op: {}\n", unary.op().lexeme);

    m_indentation -= 1;
  }

  return Value::kUndefined;
}

void AstPrinter::add_indentation() {
  constexpr i32 INDENT_SIZE = 2;
  for (int i = 0; i < m_indentation * INDENT_SIZE; ++i) {
    m_string += " ";
  }
}

Value AstPrinter::visit_variable(VariableExpr& expr) {
  UNUSED(expr);
  return Value::kUndefined;
}

Value AstPrinter::visit_assign(AssignExpr& expr) {
  UNUSED(expr);
  return Value::kUndefined;
}

void AstPrinter::visit_expr(ExprStmt& stmt) {
  UNUSED(stmt);
}

void AstPrinter::visit_print(PrintStmt& stmt) {
  UNUSED(stmt);
}

void AstPrinter::visit_var_decl(VarDeclStmt& stmt) {
  UNUSED(stmt);
}

void AstPrinter::visit_block(BlockStmt& stmt) {
  UNUSED(stmt);
}

void AstPrinter::visit_if(IfStmt& stmt) {
  UNUSED(stmt);
}

void AstPrinter::visit_while(WhileStmt& stmt) {
  UNUSED(stmt);
}
