#include <Gluon/Compiler/ShuntingYard.h>

#include <Gluon/Widgets/Widget.h>

#include <loguru.hpp>

#include <deque>
#include <stack>

namespace ShuntingYard
{

f32 Expression::evaluate() const
{
	Beard::Array<f32> value_stack;

	auto working_queue = evaluation_queue;

	while (!working_queue.IsEmpty())
	{
		auto* node = working_queue.First().get();
		working_queue.PopAndDiscard();

		switch (node->type)
		{
			case NodeType::Constant:
			{
				value_stack.Add(reinterpret_cast<ConstantNode*>(node)->constant);
			}
			break;

			case NodeType::Operator:
			{
				// TODO: Unary operators

				f32 right = value_stack.Pop();
				f32 left  = value_stack.Pop();

				value_stack.Add(evaluate_operator(reinterpret_cast<OperatorNode*>(node), left, right));
			}
			break;

			case NodeType::Function:
			{
				value_stack.Add(evaluate_function(reinterpret_cast<FunctionNode*>(node)));
			}
			break;
		}
	}

	return value_stack.Last();
}

f32 Expression::evaluate_operator(OperatorNode* op, f32 left, f32 right)
{
	switch (op->op)
	{
		case Operator::Add: return op->unary ? left : left + right;
		case Operator::Substract: return op->unary ? -left : left - right;
		case Operator::Multiply: return left * right;
		case Operator::Divide: return left / right;

		case Operator::OpenParen:
		case Operator::CloseParen:
			// We should never have those in the evaluation queue
			ASSERT_UNREACHABLE();
			break;
	}

	return 0.0f;
}

inline bool IsOperator(TokenType token)
{
	switch (token)
	{
		case TokenType::Plus:
		case TokenType::Minus:
		case TokenType::Asterisk:
		case TokenType::Slash: return true;

		default: break;
	}

	return false;
}

inline Operator GetOperator(TokenType token)
{
	// clang-format off
	switch (token)
	{
		case TokenType::Plus: return Operator::Add;
		case TokenType::Minus: return Operator::Substract;
		case TokenType::Asterisk: return Operator::Multiply;
		case TokenType::Slash: return Operator::Divide;
		default:
			ASSERT_UNREACHABLE();
			break;
	}
	// clang-format on

	return Operator::OpenParen;
}

inline i32 GetPrecedence(Operator op)
{
	// clang-format off
	switch (op)
	{
		case Operator::Add:       return 2;
		case Operator::Substract: return 2;
		case Operator::Multiply:  return 3;
		case Operator::Divide:    return 3;
		default:
			ASSERT_UNREACHABLE();
			break;
	}
	// clang-format on

	return 0;
}

Expression Expression::build(const std::vector<Token>& tokens, GluonWidget* rootWidget, GluonWidget* currentWidget)
{
	Expression result;

	std::stack<Operator> operatorStack;

	for (usize i = 0; i < tokens.size(); ++i)
	{
		Token token = tokens[i];

		switch (token.type)
		{
			case TokenType::Number:
			{
				result.evaluation_queue.Add(std::make_shared<ConstantNode>(token.number));
			}
			break;

			case TokenType::OpenParen:
			{
				operatorStack.push(Operator::OpenParen);
			}
			break;

			case TokenType::CloseParen:
			{
				while (operatorStack.top() != Operator::OpenParen)
				{
					result.evaluation_queue.Add(std::make_shared<OperatorNode>(operatorStack.top(), false));
					operatorStack.pop();
				}

				operatorStack.pop();
			}
			break;

			case TokenType::Minus:
			case TokenType::Plus:
			{
				bool isFirstElement      = (i == 0);
				bool previousIsOpenParen = (i > 0) && tokens[i - 1].type == TokenType::OpenParen;
				bool previousIsOperator  = (i > 0) && IsOperator(tokens[i - 1].type);
				bool isUnary             = isFirstElement || previousIsOpenParen || previousIsOperator;

				bool isNotLast    = (i + 1) < tokens.size();
				bool nextIsNumber = isNotLast && (tokens[i + 1].type == TokenType::Number);

				// Special case for unary plus / minus
				if (isUnary && nextIsNumber)
				{
					f32 mult = token.type == TokenType::Minus ? -1.0f : 1.0f;
					result.evaluation_queue.Add(std::make_shared<ConstantNode>(mult * tokens[++i].number));

					continue;
				}

				// We are looking at operators, just continue
			}

			case TokenType::Asterisk:
			case TokenType::Slash:
			{
				Operator op = GetOperator(token.type);

				if (!operatorStack.empty())
				{
					Operator other = operatorStack.top();
					if (GetPrecedence(op) <= GetPrecedence(other))
					{
						operatorStack.pop();
						result.evaluation_queue.Add(std::make_shared<OperatorNode>(other, false));
					}
				}

				operatorStack.push(op);
			}
			break;

			case TokenType::Identifier:
			{
				bool hasTwoNextValues = (i + 2) < tokens.size();
				bool nextIsDot        = hasTwoNextValues && (tokens[i + 1].type == TokenType::Dot);
				bool nextIsProperty   = hasTwoNextValues && (tokens[i + 2].type == TokenType::Identifier);

				GluonWidget* widget = nullptr;

				std::string binding;

				if (hasTwoNextValues && nextIsDot && nextIsProperty)
				{
					auto widgetId = token.text;
					if (widgetId == "parent") { widget = currentWidget->parent; }
					else
					{
						widget = get_widget_by_id(rootWidget, widgetId);
					}

					binding = tokens[i + 2].text;
					i += 2;
				}
				else
				{
					widget  = currentWidget;
					binding = token.text;
				}

				Function fn;

				if (binding == "width") { fn = Function::Width; }
				else if (binding == "height")
				{
					fn = Function::Height;
				}
				else if (binding == "x")
				{
					fn = Function::X;
				}
				else if (binding == "y")
				{
					fn = Function::Y;
				}
				else if (binding == "bottom")
				{
					fn = Function::Bottom;
				}
				else if (binding == "top")
				{
					fn = Function::Top;
				}
				else if (binding == "left")
				{
					fn = Function::Left;
				}
				else if (binding == "right")
				{
					fn = Function::Right;
				}

				result.evaluation_queue.Add(std::make_shared<FunctionNode>(fn, widget));
			}
			break;

			default: ASSERT_UNREACHABLE(); break;
		}
	}

	while (!operatorStack.empty())
	{
		if (operatorStack.top() == Operator::OpenParen || operatorStack.top() == Operator::CloseParen)
		{
			LOG_F(ERROR, "Imbalanced expression");
		}
		else
		{
			result.evaluation_queue.Add(std::make_shared<OperatorNode>(operatorStack.top(), false));
		}
		operatorStack.pop();
	}

	return result;
}

f32 Expression::evaluate_function(FunctionNode* fn)
{
	// clang-format off
	switch (fn->fn)
	{
		case Function::X:      return fn->widget->pos.x;
		case Function::Y:      return fn->widget->pos.y;
		case Function::Width:  return fn->widget->size.x;
		case Function::Height: return fn->widget->size.y;

		case Function::Left:   return fn->widget->pos.x;
		case Function::Right:  return fn->widget->pos.x + fn->widget->size.x;
		case Function::Top:    return fn->widget->pos.y;
		case Function::Bottom: return fn->widget->pos.y + fn->widget->size.y;
	}
	// clang-format on

	return 0.0f;
}
}