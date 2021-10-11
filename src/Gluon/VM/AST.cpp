#include <Gluon/VM/AST.h>

#include <Gluon/VM/Interpreter.h>
#include <Gluon/VM/Value.h>
#include <Gluon/VM/Object.h>
#include <Gluon/VM/Function.h>

#include <stdio.h>

namespace VM
{

Value ScopeNode::execute(Interpreter* interpreter)
{
	return interpreter->run(this);
}

Value FunctionDeclaration::execute(Interpreter* interpreter)
{
	auto* function = interpreter->heap()->allocate<Function>(m_name,
	                                                         m_body.get());
	interpreter->global_object()->add(m_name, Value{function});
	return Value{function};
}

Value ExpressionStatement::execute(Interpreter* interpreter)
{
	return m_expression->execute(interpreter);
}

Value ReturnStatement::execute(Interpreter* interpreter)
{
	// TODO: Is there specific stuff to do ?
	//       Should we tell the interpreter something returned ?
	return m_argument->execute(interpreter);
}

Value CallExpression::execute(Interpreter* interpreter)
{
	Value functionValue = interpreter->global_object()->get(m_callee);
	auto* object        = functionValue.as_object();

	ASSERT(object->is_function(), "Could this happen in real life ?");
	if (object->is_function())
	{
		auto* function = static_cast<Function*>(object);

		return interpreter->run(function->body());
	}
	return Value::Undefined;
}

inline Value add(Value lhs, Value rhs)
{
	ASSERT(lhs.is_number(), "TODO");
	ASSERT(rhs.is_number(), "TODO");

	return Value{lhs.as_number() + rhs.as_number()};
}

inline Value Sub(Value lhs, Value rhs)
{
	ASSERT(lhs.is_number(), "TODO");
	ASSERT(rhs.is_number(), "TODO");

	return Value{lhs.as_number() - rhs.as_number()};
}

Value BinaryExpression::execute(Interpreter* interpreter)
{
	Value lhs = m_left->execute(interpreter);
	Value rhs = m_right->execute(interpreter);

	switch (m_op)
	{
		case BinaryOp::Addition:
			return add(lhs, rhs);

		case BinaryOp::Substraction:
			return Sub(lhs, rhs);
	}

	ASSERT_UNREACHABLE();
	return Value::Undefined;
}

Value Literal::execute(Interpreter* interpreter)
{
	UNUSED(interpreter);
	return m_value;
}

// ----------------------------------------------------------------------------
// DUMP
// ----------------------------------------------------------------------------
void ASTNode::dump(i32 indent) const
{
	for (i32 i = 0; i < indent * 2; ++i)
	{
		printf(" ");
	}
}

void ScopeNode::dump(i32 indent) const
{
	ASTNode::dump(indent);

	printf("%s\n", "(ScopeNode)");

	if (!m_body.is_empty())
	{
		indent += 1;
		ASTNode::dump(indent);

		printf("body:\n");

		for (auto&& elt : m_body)
		{
			elt->dump(indent + 1);
		}
	}
}

void Identifier::dump(i32 indent) const
{
	ASTNode::dump(indent);

	printf("Identifier <%s>\n", m_name.c_str());
}

void FunctionDeclaration::dump(i32 indent) const
{
	ASTNode::dump(indent);

	printf("Function\n");
	indent += 1;

	ASTNode::dump(indent);
	printf("name: %s\n", m_name.c_str());
	ASTNode::dump(indent);
	printf("body:\n");
	m_body->dump(indent + 1);
}

void ExpressionStatement::dump(i32 indent) const
{
	ASTNode::dump(indent);
	printf("ExpressionStatement\n");
	m_expression->dump(indent + 1);
}

void ReturnStatement::dump(i32 indent) const
{
	ASTNode::dump(indent);
	printf("ReturnStatement\n");
	m_argument->dump(indent + 1);
}

void CallExpression::dump(i32 indent) const
{
	ASTNode::dump(indent);
	printf("CallExpression: %s()\n", m_callee.c_str());
}

void BinaryExpression::dump(i32 indent) const
{
	ASTNode::dump(indent);
	printf("BinaryExpression\n");

	indent += 1;
	ASTNode::dump(indent);
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

	ASTNode::dump(indent);
	printf("Left:\n");
	m_left->dump(indent + 1);
	ASTNode::dump(indent);
	printf("Right:\n");
	m_right->dump(indent + 1);
}

void Literal::dump(i32 indent) const
{
	ASTNode::dump(indent);

	printf("%s\n", m_value.to_string().c_str());
}

}
