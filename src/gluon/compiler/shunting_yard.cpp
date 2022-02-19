#include <gluon/compiler/shunting_yard.h>

#include <gluon/widgets/widget.h>

#include <loguru.hpp>

#include <deque>
#include <stack>

namespace shunting_yard {

f32 Expression::Evaluate() const {
  beard::array<f32> value_stack;

  auto working_queue = evaluation_queue;

  while (!working_queue.is_empty()) {
    auto* node = working_queue.first().get();
    working_queue.pop_and_discard();

    switch (node->type) {
      case NodeType::kConstant: {
        value_stack.add(reinterpret_cast<ConstantNode*>(node)->constant);
      } break;

      case NodeType::kOperator: {
        // TODO: Unary operators

        f32 right = value_stack.pop();
        f32 left = value_stack.pop();

        value_stack.add(EvaluateOperator(reinterpret_cast<OperatorNode*>(node),
                                         left, right));
      } break;

      case NodeType::kFunction: {
        value_stack.add(
            EvaluateFunction(reinterpret_cast<FunctionNode*>(node)));
      } break;
    }
  }

  return value_stack.last();
}

f32 Expression::EvaluateOperator(OperatorNode* op, f32 left, f32 right) {
  switch (op->op) {
    case Operator::kAdd:
      return op->unary ? left : left + right;
    case Operator::kSubstract:
      return op->unary ? -left : left - right;
    case Operator::kMultiply:
      return left * right;
    case Operator::kDivide:
      return left / right;

    case Operator::kOpenParen:
    case Operator::kCloseParen:
      // We should never have those in the evaluation queue
      ASSERT_UNREACHABLE();
      break;
  }

  return 0.0f;
}

inline bool IsOperator(TokenType::Enum token) {
  switch (token) {
    case TokenType::kPlus:
    case TokenType::kMinus:
    case TokenType::kAsterisk:
    case TokenType::kSlash:
      return true;

    default:
      break;
  }

  return false;
}

inline Operator::Enum GetOperator(TokenType::Enum token) {
  // clang-format off
	switch (token)
	{
		case TokenType::kPlus: return Operator::kAdd;
		case TokenType::kMinus: return Operator::kSubstract;
		case TokenType::kAsterisk: return Operator::kMultiply;
		case TokenType::kSlash: return Operator::kDivide;
		default:
			ASSERT_UNREACHABLE();
			break;
	}
  // clang-format on

  return Operator::kOpenParen;
}

inline i32 GetPrecedence(Operator::Enum op) {
  // clang-format off
	switch (op)
	{
		case Operator::kAdd:       return 2;
		case Operator::kSubstract: return 2;
		case Operator::kMultiply:  return 3;
		case Operator::kDivide:    return 3;
		default:
			ASSERT_UNREACHABLE();
			break;
	}
  // clang-format on

  return 0;
}

Expression Expression::Build(const beard::array<Token>& tokens,
                             Widget* root_widget,
                             Widget* current_widget) {
  Expression result;

  std::stack<Operator::Enum> operator_stack;

  for (i32 i = 0; i < tokens.element_count(); ++i) {
    Token token = tokens[i];

    switch (token.token_type) {
      case TokenType::kNumber: {
        result.evaluation_queue.add(
            std::make_shared<ConstantNode>(token.number));
      } break;

      case TokenType::kOpenParen: {
        operator_stack.push(Operator::kOpenParen);
      } break;

      case TokenType::kCloseParen: {
        while (operator_stack.top() != Operator::kOpenParen) {
          result.evaluation_queue.add(
              std::make_shared<OperatorNode>(operator_stack.top(), false));
          operator_stack.pop();
        }

        operator_stack.pop();
      } break;

      case TokenType::kMinus:
      case TokenType::kPlus: {
        bool is_first_element = (i == 0);
        bool previous_is_open_paren =
            (i > 0) && tokens[i - 1].token_type == TokenType::kOpenParen;
        bool previous_is_operator =
            (i > 0) && IsOperator(tokens[i - 1].token_type);
        bool is_unary =
            is_first_element || previous_is_open_paren || previous_is_operator;

        bool is_not_last = (i + 1) < tokens.element_count();
        bool next_is_number =
            is_not_last && (tokens[i + 1].token_type == TokenType::kNumber);

        // Special case for bUnary plus / minus
        if (is_unary && next_is_number) {
          f32 mult = token.token_type == TokenType::kMinus ? -1.0f : 1.0f;
          result.evaluation_queue.add(
              std::make_shared<ConstantNode>(mult * tokens[++i].number));

          continue;
        }

        // We are looking at operators, just continue
      }

      case TokenType::kAsterisk:
      case TokenType::kSlash: {
        Operator::Enum op = GetOperator(token.token_type);

        if (!operator_stack.empty()) {
          Operator::Enum other = operator_stack.top();
          if (GetPrecedence(op) <= GetPrecedence(other)) {
            operator_stack.pop();
            result.evaluation_queue.add(
                std::make_shared<OperatorNode>(other, false));
          }
        }

        operator_stack.push(op);
      } break;

      case TokenType::kIdentifier: {
        bool hasTwoNextValues = (i + 2) < tokens.element_count();
        bool nextIsDot =
            hasTwoNextValues && (tokens[i + 1].token_type == TokenType::kDot);
        bool nextIsProperty = hasTwoNextValues && (tokens[i + 2].token_type ==
                                                   TokenType::kIdentifier);

        Widget* Widget = nullptr;

        std::string Binding;

        if (hasTwoNextValues && nextIsDot && nextIsProperty) {
          auto widgetId = token.text;
          if (widgetId == "parent") {
            Widget = current_widget->parent;
          } else {
            Widget = GetWidgetById(root_widget, widgetId);
          }

          Binding = tokens[i + 2].text;
          i += 2;
        } else {
          Widget = current_widget;
          Binding = token.text;
        }

        Function::Enum function;

        if (Binding == "width") {
          function = Function::kWidth;
        } else if (Binding == "height") {
          function = Function::kHeight;
        } else if (Binding == "x") {
          function = Function::kX;
        } else if (Binding == "y") {
          function = Function::kY;
        } else if (Binding == "bottom") {
          function = Function::kBottom;
        } else if (Binding == "top") {
          function = Function::kTop;
        } else if (Binding == "left") {
          function = Function::kLeft;
        } else if (Binding == "right") {
          function = Function::kRight;
        }

        result.evaluation_queue.add(
            std::make_shared<FunctionNode>(function, current_widget));
      } break;

      default:
        ASSERT_UNREACHABLE();
        break;
    }
  }

  while (!operator_stack.empty()) {
    if (operator_stack.top() == Operator::kOpenParen ||
        operator_stack.top() == Operator::kCloseParen) {
      LOG_F(ERROR, "Imbalanced expression");
    } else {
      result.evaluation_queue.add(
          std::make_shared<OperatorNode>(operator_stack.top(), false));
    }
    operator_stack.pop();
  }

  return result;
}

f32 Expression::EvaluateFunction(FunctionNode* function) {
  // clang-format off
	switch (function->function)
	{
		case Function::kX:      return function->widget->pos.x;
		case Function::kY:      return function->widget->pos.y;
		case Function::kWidth:  return function->widget->size.x;
		case Function::kHeight: return function->widget->size.y;

		case Function::kLeft:   return function->widget->pos.x;
		case Function::kRight:  return function->widget->pos.x + function->widget->size.x;
		case Function::kTop:    return function->widget->pos.y;
		case Function::kBottom: return function->widget->pos.y + function->widget->size.y;
	}
  // clang-format on

  return 0.0f;
}
}  // namespace shunting_yard