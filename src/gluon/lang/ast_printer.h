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
    Value VisitVariable(VariableExpr& expr) override;
    Value VisitAssign(AssignExpr& expr) override;

    void VisitExpr(ExprStmt& expr) override;
    void VisitPrint(PrintStmt& expr) override;
    void VisitVarDecl(VarDeclStmt& expr) override;
    void VisitBlock(BlockStmt& block) override;
    void VisitIf(IfStmt& stmt) override;
    void VisitWhile(WhileStmt& stmt) override;

    void AddIndentation();

    std::u32string m_string;
    i32            m_indentation = 0;
};
