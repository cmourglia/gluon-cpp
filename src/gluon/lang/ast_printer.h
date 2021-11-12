#pragma once

#include <gluon/lang/grammar.h>

class ZASTPrinter
    : public IExpressionVisitor
    , public IStatementVisitor
{
public:
    void PrintAST(ZExpression& ASTRoot);

private:
    ZValue VisitBinary(ZBinary& Binary) override;
    ZValue VisitGrouping(ZGrouping& Grouping) override;
    ZValue VisitLiteral(ZLiteral& Literal) override;
    ZValue VisitUnary(ZUnary& Unary) override;

    void AddIndentation();

    std::string m_String;
    i32         m_Indentation = 0;
};
