#include <Gluon/VM/AST.h>

#include <Gluon/VM/Interpreter.h>
#include <Gluon/VM/Value.h>
#include <Gluon/VM/Object.h>
#include <Gluon/VM/Function.h>

#include <stdio.h>
#include <limits>

Value ScopeNode::execute(Interpreter* interpreter) { return interpreter->run(this); }

Value FunctionDeclaration::execute(Interpreter* interpreter)
{
	auto* function = interpreter->heap()->allocate<Function>(m_name, m_body.get());
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
	if ((lhs.is_number() && rhs.is_undefined()) || (lhs.is_undefined() && rhs.is_number()))
	{
		return Value{std::numeric_limits<f64>::quiet_NaN()};
	}

	if (lhs.is_number() && rhs.is_null())
	{
		return Value{lhs.as_number()};
	}

	if (lhs.is_null() && rhs.is_number())
	{
		return Value{rhs.as_number()};
	}

	if (lhs.is_number() && rhs.is_number())
	{
		return Value{lhs.as_number() + rhs.as_number()};
	}

	ASSERT_UNREACHABLE();
	return Value::Undefined;
}

inline Value Sub(Value lhs, Value rhs)
{
	if ((lhs.is_number() && rhs.is_undefined()) || (lhs.is_undefined() && rhs.is_number()))
	{
		return Value{std::numeric_limits<f64>::quiet_NaN()};
	}

	if (lhs.is_number() && rhs.is_null())
	{
		return Value{lhs.as_number()};
	}

	if (lhs.is_null() && rhs.is_number())
	{
		return Value{-rhs.as_number()};
	}

	if (lhs.is_number() && rhs.is_number())
	{
		return Value{lhs.as_number() - rhs.as_number()};
	}

	ASSERT_UNREACHABLE();
	return Value::Undefined;
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

Value Identifier::execute(Interpreter* interpreter)
{
	return interpreter->get_variable(m_name.c_str());
}

Value Literal::execute(Interpreter* interpreter)
{
	UNUSED(interpreter);
	return m_value;
}

Value AssignmentExpression::execute(Interpreter* interpreter)
{
	switch (m_op)
	{
		case AssignmentOperator::Assign:
		{
			interpreter->set_variable(m_left->name().c_str(), m_right->execute(interpreter));
		}
		break;

		default:
			ASSERT_UNREACHABLE();
			break;
	}
	return Value::Undefined;
}

Value VariableDeclaration::execute(Interpreter* interpreter)
{
	interpreter->declare_variable(m_identifier->name().c_str());

	Value result = Value::Null;

	if (m_initializer != nullptr)
	{
		result = m_initializer->execute(interpreter);
		interpreter->set_variable(m_identifier->name().c_str(), result);
	}

	return result;
}

// ----------------------------------------------------------------------------
// DUMP
// ----------------------------------------------------------------------------

inline void print_indent(i32 indent)
{
	for (i32 i = 0; i < indent * 2; ++i)
	{
		printf(" ");
	}
}

void ASTNode::dump(i32 indent) const { UNUSED(indent); }

void ScopeNode::dump(i32 indent) const
{
	print_indent(indent);

	printf("%s\n", "(ScopeNode)");

	if (!m_body.IsEmpty())
	{
		indent += 1;
		print_indent(indent);

		printf("body:\n");

		for (auto&& elt : m_body)
		{
			elt->dump(indent + 1);
		}
	}
}

void Identifier::dump(i32 indent) const
{
	print_indent(indent);

	printf("Identifier <%s>\n", m_name.c_str());
}

void FunctionDeclaration::dump(i32 indent) const
{
	print_indent(indent);

	printf("Function\n");
	indent += 1;

	print_indent(indent);
	printf("name: %s\n", m_name.c_str());
	print_indent(indent);
	printf("body:\n");
	m_body->dump(indent + 1);
}

void ExpressionStatement::dump(i32 indent) const
{
	print_indent(indent);
	printf("ExpressionStatement\n");
	m_expression->dump(indent + 1);
}

void ReturnStatement::dump(i32 indent) const
{
	print_indent(indent);
	printf("ReturnStatement\n");
	m_argument->dump(indent + 1);
}

void CallExpression::dump(i32 indent) const
{
	print_indent(indent);
	printf("CallExpression: %s()\n", m_callee.c_str());
}

void BinaryExpression::dump(i32 indent) const
{
	print_indent(indent);
	printf("BinaryExpression\n");

	indent += 1;
	print_indent(indent);
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

	print_indent(indent);
	printf("Left:\n");
	m_left->dump(indent + 1);
	print_indent(indent);
	printf("Right:\n");
	m_right->dump(indent + 1);
}

void Literal::dump(i32 indent) const
{
	print_indent(indent);

	printf("%s\n", m_value.to_string().c_str());
}

void AssignmentExpression::dump(i32 indent) const
{
	print_indent(indent);
	printf("operator: ");
	switch (m_op)
	{
		case AssignmentOperator::Assign:
			printf("=");
			break;
		default:
			ASSERT_UNREACHABLE();
			break;
	}
	printf("\n");

	print_indent(indent);
	printf("left:\n");
	m_left->dump(indent + 1);

	print_indent(indent);
	printf("right:\n");
	m_right->dump(indent + 1);
}

void VariableDeclaration::dump(i32 indent) const
{
	print_indent(indent);
	printf("Variable declaration\n");
	print_indent(indent + 1);
	printf("name:\n");
	m_identifier->dump(indent + 2);
	printf("init:\n");
	if (m_initializer.get() != nullptr)
	{
		m_initializer->dump(indent + 2);
	}
	else
	{
		print_indent(indent + 2);
		printf("<null>");
	}
}
