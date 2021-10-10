#include <Gluon/VM/AST.h>

#include <Gluon/VM/Interpreter.h>
#include <Gluon/VM/Value.h>
#include <Gluon/VM/Object.h>
#include <Gluon/VM/Function.h>

#include <stdio.h>

namespace VM
{

Value ScopeNode::Execute(Interpreter* interpreter)
{
	return interpreter->Run(this);
}

Value FunctionDeclaration::Execute(Interpreter* interpreter)
{
	auto* function = interpreter->GetHeap()->Allocate<Function>(m_name,
	                                                            m_body.get());
	interpreter->GlobalObject()->Add(m_name, Value{function});
	return Value{function};
}

Value ExpressionStatement::Execute(Interpreter* interpreter)
{
	return m_expression->Execute(interpreter);
}

Value ReturnStatement::Execute(Interpreter* interpreter)
{
	// TODO: Is there specific stuff to do ?
	//       Should we tell the interpreter something returned ?
	return m_argument->Execute(interpreter);
}

Value CallExpression::Execute(Interpreter* interpreter)
{
	Value functionValue = interpreter->GlobalObject()->Get(m_callee);
	auto* object        = functionValue.AsObject();

	Assert(object->IsFunction(), "Could this happen in real life ?");
	if (object->IsFunction())
	{
		auto* function = static_cast<Function*>(object);

		return interpreter->Run(function->GetBody());
	}
	return Value::Undefined;
}

inline Value Add(Value lhs, Value rhs)
{
	Assert(lhs.IsNumber(), "TODO");
	Assert(rhs.IsNumber(), "TODO");

	return Value{lhs.AsNumber() + rhs.AsNumber()};
}

inline Value Sub(Value lhs, Value rhs)
{
	Assert(lhs.IsNumber(), "TODO");
	Assert(rhs.IsNumber(), "TODO");

	return Value{lhs.AsNumber() - rhs.AsNumber()};
}

Value BinaryExpression::Execute(Interpreter* interpreter)
{
	Value lhs = m_left->Execute(interpreter);
	Value rhs = m_right->Execute(interpreter);

	switch (m_op)
	{
		case BinaryOp::Addition:
			return Add(lhs, rhs);

		case BinaryOp::Substraction:
			return Sub(lhs, rhs);
	}

	AssertUnreachable();
	return Value::Undefined;
}

Value Literal::Execute(Interpreter* interpreter)
{
	UNUSED(interpreter);
	return m_value;
}

// ----------------------------------------------------------------------------
// DUMP
// ----------------------------------------------------------------------------
void ASTNode::Dump(i32 indent) const
{
	for (i32 i = 0; i < indent * 2; ++i)
	{
		printf(" ");
	}
}

void ScopeNode::Dump(i32 indent) const
{
	ASTNode::Dump(indent);

	printf("%s\n", TypeName());

	if (!m_body.IsEmpty())
	{
		indent += 1;
		ASTNode::Dump(indent);

		printf("body:\n");

		for (auto&& bodyElt : m_body)
		{
			bodyElt->Dump(indent + 1);
		}
	}
}

void Identifier::Dump(i32 indent) const
{
	ASTNode::Dump(indent);

	printf("Identifier <%s>\n", m_name.c_str());
}

void FunctionDeclaration::Dump(i32 indent) const
{
	ASTNode::Dump(indent);

	printf("Function\n");
	indent += 1;

	ASTNode::Dump(indent);
	printf("name: %s\n", m_name.c_str());
	ASTNode::Dump(indent);
	printf("body:\n");
	m_body->Dump(indent + 1);
}

void ExpressionStatement::Dump(i32 indent) const
{
	ASTNode::Dump(indent);
	printf("ExpressionStatement\n");
	m_expression->Dump(indent + 1);
}

void ReturnStatement::Dump(i32 indent) const
{
	ASTNode::Dump(indent);
	printf("ReturnStatement\n");
	m_argument->Dump(indent + 1);
}

void CallExpression::Dump(i32 indent) const
{
	ASTNode::Dump(indent);
	printf("CallExpression: %s()\n", m_callee.c_str());
}

void BinaryExpression::Dump(i32 indent) const
{
	ASTNode::Dump(indent);
	printf("BinaryExpression\n");

	indent += 1;
	ASTNode::Dump(indent);
	printf("Operator: ");

	switch (m_op)
	{
		case BinaryOp::Addition:
			printf("+");
			break;

		case BinaryOp::Substraction:
			printf("-");
			break;
	}
	printf("\n");

	ASTNode::Dump(indent);
	printf("Left:\n");
	m_left->Dump(indent + 1);
	ASTNode::Dump(indent);
	printf("Right:\n");
	m_right->Dump(indent + 1);
}

void Literal::Dump(i32 indent) const
{
	ASTNode::Dump(indent);

	m_value.Dump();

	printf("\n");
}

}
