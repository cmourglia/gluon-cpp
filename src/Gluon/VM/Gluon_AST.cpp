#include <Gluon/VM/Gluon_AST.h>

#include <Gluon/VM/Gluon_Function.h>
#include <Gluon/VM/Gluon_Interpreter.h>
#include <Gluon/VM/Gluon_Object.h>
#include <Gluon/VM/Gluon_Value.h>

#include <limits>
#include <stdio.h>

ZValue ZScopeNode::Execute(ZInterpreter* interpreter)
{
	return interpreter->Run(this);
}

ZValue ZFunctionDeclaration::Execute(ZInterpreter* interpreter)
{
	auto* function = interpreter->Heap()->Allocate<ZFunction>(m_Name, m_Body.get());
	interpreter->GlobalObject()->Add(m_Name, ZValue{function});
	return ZValue{function};
}

ZValue ExpressionStatement::Execute(ZInterpreter* interpreter)
{
	return m_Expression->Execute(interpreter);
}

ZValue ZReturnStatement::Execute(ZInterpreter* interpreter)
{
	// TODO: Is there specific stuff to do ?
	//       Should we tell the interpreter something returned ?
	return m_Argument->Execute(interpreter);
}

ZValue ZCallExpression::Execute(ZInterpreter* interpreter)
{
	ZValue functionValue = interpreter->GlobalObject()->Get(m_Callee);
	auto*  object        = functionValue.AsObject();

	ASSERT(object->IsFunction(), "Could this happen in real life ?");
	if (object->IsFunction())
	{
		auto* function = static_cast<ZFunction*>(object);

		return interpreter->Run(function->Body());
	}
	return ZValue::Undefined;
}

inline ZValue add(ZValue lhs, ZValue rhs)
{
	if ((lhs.IsNumber() && rhs.IsUndefined()) || (lhs.IsUndefined() && rhs.IsNumber()))
	{
		return ZValue{std::numeric_limits<f64>::quiet_NaN()};
	}

	if (lhs.IsNumber() && rhs.IsNull())
	{
		return ZValue{lhs.AsNumber()};
	}

	if (lhs.IsNull() && rhs.IsNumber())
	{
		return ZValue{rhs.AsNumber()};
	}

	if (lhs.IsNumber() && rhs.IsNumber())
	{
		return ZValue{lhs.AsNumber() + rhs.AsNumber()};
	}

	ASSERT_UNREACHABLE();
	return ZValue::Undefined;
}

inline ZValue Sub(ZValue lhs, ZValue rhs)
{
	if ((lhs.IsNumber() && rhs.IsUndefined()) || (lhs.IsUndefined() && rhs.IsNumber()))
	{
		return ZValue{std::numeric_limits<f64>::quiet_NaN()};
	}

	if (lhs.IsNumber() && rhs.IsNull())
	{
		return ZValue{lhs.AsNumber()};
	}

	if (lhs.IsNull() && rhs.IsNumber())
	{
		return ZValue{-rhs.AsNumber()};
	}

	if (lhs.IsNumber() && rhs.IsNumber())
	{
		return ZValue{lhs.AsNumber() - rhs.AsNumber()};
	}

	ASSERT_UNREACHABLE();
	return ZValue::Undefined;
}

ZValue ZBinaryExpression::Execute(ZInterpreter* interpreter)
{
	ZValue lhs = m_LHS->Execute(interpreter);
	ZValue rhs = m_RHS->Execute(interpreter);

	switch (m_Op)
	{
		case EBinaryOp::Addition:
			return add(lhs, rhs);

		case EBinaryOp::Substraction:
			return Sub(lhs, rhs);
	}

	ASSERT_UNREACHABLE();
	return ZValue::Undefined;
}

ZValue ZIdentifier::Execute(ZInterpreter* interpreter)
{
	return interpreter->GetVariable(m_Name.c_str());
}

ZValue ZLiteral::Execute(ZInterpreter* interpreter)
{
	UNUSED(interpreter);
	return m_Value;
}

ZValue AssignmentExpression::Execute(ZInterpreter* interpreter)
{
	switch (m_Op)
	{
		case EAssignmentOperator::Assign:
		{
			interpreter->SetVariable(m_LHS->Name().c_str(), m_RHS->Execute(interpreter));
		}
		break;

		default:
			ASSERT_UNREACHABLE();
			break;
	}
	return ZValue::Undefined;
}

ZValue ZVariableDeclaration::Execute(ZInterpreter* interpreter)
{
	interpreter->DeclareVariable(m_Identifier->Name().c_str());

	ZValue Result = ZValue::Null;

	if (m_Initializer != nullptr)
	{
		Result = m_Initializer->Execute(interpreter);
		interpreter->SetVariable(m_Identifier->Name().c_str(), Result);
	}

	return Result;
}

// ----------------------------------------------------------------------------
// DUMP
// ----------------------------------------------------------------------------

inline void PrintIndent(i32 Indent)
{
	for (i32 i = 0; i < Indent * 2; ++i)
	{
		printf(" ");
	}
}

void ZASTNode::Dump(i32 Indent) const
{
	UNUSED(Indent);
}

void ZScopeNode::Dump(i32 Indent) const
{
	PrintIndent(Indent);

	printf("%s\n", "(ZScopeNode)");

	if (!m_Body.IsEmpty())
	{
		Indent += 1;
		PrintIndent(Indent);

		printf("body:\n");

		for (auto&& elt : m_Body)
		{
			elt->Dump(Indent + 1);
		}
	}
}

void ZIdentifier::Dump(i32 Indent) const
{
	PrintIndent(Indent);

	printf("Identifier <%s>\n", m_Name.c_str());
}

void ZFunctionDeclaration::Dump(i32 Indent) const
{
	PrintIndent(Indent);

	printf("Function\n");
	Indent += 1;

	PrintIndent(Indent);
	printf("Name: %s\n", m_Name.c_str());
	PrintIndent(Indent);
	printf("body:\n");
	m_Body->Dump(Indent + 1);
}

void ExpressionStatement::Dump(i32 Indent) const
{
	PrintIndent(Indent);
	printf("ExpressionStatement\n");
	m_Expression->Dump(Indent + 1);
}

void ZReturnStatement::Dump(i32 Indent) const
{
	PrintIndent(Indent);
	printf("ReturnStatement\n");
	m_Argument->Dump(Indent + 1);
}

void ZCallExpression::Dump(i32 Indent) const
{
	PrintIndent(Indent);
	printf("CallExpression: %s()\n", m_Callee.c_str());
}

void ZBinaryExpression::Dump(i32 Indent) const
{
	PrintIndent(Indent);
	printf("BinaryExpression\n");

	Indent += 1;
	PrintIndent(Indent);
	printf("Operator: ");

	switch (m_Op)
	{
		case EBinaryOp::Addition:
			printf("+");
			break;

		case EBinaryOp::Substraction:
			printf("-");
			break;
	}
	printf("\n");

	PrintIndent(Indent);
	printf("Left:\n");
	m_LHS->Dump(Indent + 1);
	PrintIndent(Indent);
	printf("Right:\n");
	m_RHS->Dump(Indent + 1);
}

void ZLiteral::Dump(i32 Indent) const
{
	PrintIndent(Indent);

	printf("%s\n", m_Value.ToString().c_str());
}

void AssignmentExpression::Dump(i32 Indent) const
{
	PrintIndent(Indent);
	printf("operator: ");
	switch (m_Op)
	{
		case EAssignmentOperator::Assign:
			printf("=");
			break;
		default:
			ASSERT_UNREACHABLE();
			break;
	}
	printf("\n");

	PrintIndent(Indent);
	printf("Left:\n");
	m_LHS->Dump(Indent + 1);

	PrintIndent(Indent);
	printf("Right:\n");
	m_RHS->Dump(Indent + 1);
}

void ZVariableDeclaration::Dump(i32 Indent) const
{
	PrintIndent(Indent);
	Indent += 1;

	printf("Variable declaration\n");
	PrintIndent(Indent);
	printf("Name:\n");
	m_Identifier->Dump(Indent + 1);
	PrintIndent(Indent);
	printf("init:\n");
	if (m_Initializer.get() != nullptr)
	{
		m_Initializer->Dump(Indent + 1);
	}
	else
	{
		PrintIndent(Indent + 1);
		printf("<null>\n");
	}
}
