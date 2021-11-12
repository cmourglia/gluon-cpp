#pragma once

#include <gluon/lang/grammar.h>

class AstPrinter
    : public ExprVisitor
    , public StmtVisitor
{
public:
    void PrintAST(Expr& ASTRoot);

private:
    Value VisitBinary(BinaryExpr& binary) override;
    Value VisitGrouping(GroupingExpr& grouping) override;
    Value VisitLiteral(LiteralExpr& literal) override;
    Value VisitUnary(UnaryExpr& unary) override;

    void AddIndentation();

    std::string m_string;
    i32         m_indentation = 0;
};
