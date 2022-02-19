#include <gluon/lang/ast_printer.h>

#include <beard/io/io.h>

void AstPrinter::PrintAST(Expr& Root) {
  Root.Accept(*this);

  int test = 2;
  test += 3;
  test -= 42;
  printf("%d\n", test);

  std::cout << "Produced AST:" << std::endl
            << beard::io::from_utf8(m_string) << std::endl;
}

Value AstPrinter::VisitBinary(BinaryExpr& binary) {
  AddIndentation();
  m_string += U"Binary\n";
  {
    m_indentation += 1;
    AddIndentation();
    m_string += U"Left:\n";
    {
      m_indentation += 1;
      binary.left()->Accept(*this);
      m_indentation -= 1;
    }

    AddIndentation();
    m_string += U"Right:\n";
    {
      m_indentation += 1;
      binary.right()->Accept(*this);
      m_indentation -= 1;
    }

    AddIndentation();
    m_string += U"op: " + binary.op().lexeme + U"\n";
    m_indentation -= 1;
  }

  return Value::Undefined;
}

Value AstPrinter::VisitGrouping(GroupingExpr& grouping) {
  AddIndentation();
  m_string += U"Group:\n";
  m_indentation += 1;
  grouping.expr()->Accept(*this);
  m_indentation -= 1;

  return Value::Undefined;
}

Value AstPrinter::VisitLiteral(LiteralExpr& literal) {
  Value value = literal.value();

  AddIndentation();

  m_string += U"Literal: " + beard::io::to_utf8(value.ToString()) + U"\n";

  return Value::Undefined;
}

Value AstPrinter::VisitUnary(UnaryExpr& unary) {
  AddIndentation();
  m_string += U"Binary\n";
  {
    m_indentation += 1;
    AddIndentation();
    m_string += U"Right:\n";
    {
      m_indentation += 1;
      unary.right()->Accept(*this);
      m_indentation -= 1;
    }

    AddIndentation();
    m_string += U"op: " + unary.op().lexeme + U"\n";

    m_indentation -= 1;
  }

  return Value::Undefined;
}

void AstPrinter::AddIndentation() {
  constexpr i32 INDENT_SIZE = 2;
  for (int i = 0; i < m_indentation * INDENT_SIZE; ++i) {
    m_string += U" ";
  }
}

Value AstPrinter::VisitVariable(VariableExpr& expr) {
  UNUSED(expr);
  return Value::Undefined;
}

Value AstPrinter::VisitAssign(AssignExpr& expr) {
  UNUSED(expr);
  return Value::Undefined;
}

void AstPrinter::VisitExpr(ExprStmt& stmt) {
  UNUSED(stmt);
}

void AstPrinter::VisitPrint(PrintStmt& stmt) {
  UNUSED(stmt);
}

void AstPrinter::VisitVarDecl(VarDeclStmt& stmt) {
  UNUSED(stmt);
}

void AstPrinter::VisitBlock(BlockStmt& stmt) {
  UNUSED(stmt);
}

void AstPrinter::VisitIf(IfStmt& stmt) {
  UNUSED(stmt);
}

void AstPrinter::VisitWhile(WhileStmt& stmt) {
  UNUSED(stmt);
}
