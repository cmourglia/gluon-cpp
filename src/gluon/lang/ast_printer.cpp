#include <gluon/lang/ast_printer.h>

void ZASTPrinter::PrintAST(ZExpression& Root)
{
    Root.Accept(*this);

    int test = 2;
    test += 3;
    test -= 42;
    printf("%d\n", test);

    printf("Produced AST:\n%s\n", m_String.c_str());
}

ZValue ZASTPrinter::VisitBinary(ZBinary& Binary)
{
    AddIndentation();
    m_String += "Binary\n";
    {
        m_Indentation += 1;
        AddIndentation();
        m_String += "Left:\n";
        {
            m_Indentation += 1;
            Binary.Left()->Accept(*this);
            m_Indentation -= 1;
        }

        AddIndentation();
        m_String += ("Right:\n");
        {
            m_Indentation += 1;
            Binary.Right()->Accept(*this);
            m_Indentation -= 1;
        }

        AddIndentation();
        m_String += "Operator: " + Binary.Operator().Text + "\n";
        m_Indentation -= 1;
    }

    return ZValue::Undefined;
}

ZValue ZASTPrinter::VisitGrouping(ZGrouping& Grouping)
{
    AddIndentation();
    m_String += ("Group:\n");
    m_Indentation += 1;
    Grouping.Expression()->Accept(*this);
    m_Indentation -= 1;

    return ZValue::Undefined;
}

ZValue ZASTPrinter::VisitLiteral(ZLiteral& Literal)
{
    ZValue Value = Literal.Value();

    AddIndentation();
    m_String += "Literal: " + Value.ToString() + "\n";

    return ZValue::Undefined;
}

ZValue ZASTPrinter::VisitUnary(ZUnary& Unary)
{
    AddIndentation();
    m_String += ("Binary\n");
    {
        m_Indentation += 1;
        AddIndentation();
        m_String += ("Right:\n");
        {
            m_Indentation += 1;
            Unary.Right()->Accept(*this);
            m_Indentation -= 1;
        }

        AddIndentation();
        m_String += "Operator: " + Unary.Operator().Text + "\n";

        m_Indentation -= 1;
    }

    return ZValue::Undefined;
}

void ZASTPrinter::AddIndentation()
{
    constexpr i32 INDENT_SIZE = 2;
    for (int i = 0; i < m_Indentation * INDENT_SIZE; ++i)
    {
        m_String += " ";
    }
}
