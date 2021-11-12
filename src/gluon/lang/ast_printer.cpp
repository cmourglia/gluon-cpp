#include <gluon/lang/ast_printer.h>

void AstPrinter::PrintAST(Expr& Root)
{
    Root.Accept(*this);

    int test = 2;
    test += 3;
    test -= 42;
    printf("%d\n", test);

    printf("Produced AST:\n%s\n", m_string.c_str());
}

Value AstPrinter::VisitBinary(BinaryExpr& binary)
{
    AddIndentation();
    m_string += "Binary\n";
    {
        m_indentation += 1;
        AddIndentation();
        m_string += "Left:\n";
        {
            m_indentation += 1;
            binary.left()->Accept(*this);
            m_indentation -= 1;
        }

        AddIndentation();
        m_string += ("Right:\n");
        {
            m_indentation += 1;
            binary.right()->Accept(*this);
            m_indentation -= 1;
        }

        AddIndentation();
        m_string += "op: " + binary.op().text + "\n";
        m_indentation -= 1;
    }

    return Value::Undefined;
}

Value AstPrinter::VisitGrouping(GroupingExpr& grouping)
{
    AddIndentation();
    m_string += ("Group:\n");
    m_indentation += 1;
    grouping.expr()->Accept(*this);
    m_indentation -= 1;

    return Value::Undefined;
}

Value AstPrinter::VisitLiteral(LiteralExpr& literal)
{
    Value Value = literal.value();

    AddIndentation();
    m_string += "Literal: " + Value.ToString() + "\n";

    return Value::Undefined;
}

Value AstPrinter::VisitUnary(UnaryExpr& unary)
{
    AddIndentation();
    m_string += ("Binary\n");
    {
        m_indentation += 1;
        AddIndentation();
        m_string += ("Right:\n");
        {
            m_indentation += 1;
            unary.right()->Accept(*this);
            m_indentation -= 1;
        }

        AddIndentation();
        m_string += "op: " + unary.op().text + "\n";

        m_indentation -= 1;
    }

    return Value::Undefined;
}

void AstPrinter::AddIndentation()
{
    constexpr i32 INDENT_SIZE = 2;
    for (int i = 0; i < m_indentation * INDENT_SIZE; ++i)
    {
        m_string += " ";
    }
}
