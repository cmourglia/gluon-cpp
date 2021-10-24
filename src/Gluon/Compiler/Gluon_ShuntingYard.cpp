#include <Gluon/Compiler/Gluon_ShuntingYard.h>

#include <Gluon/Widgets/Gluon_Widget.h>

#include <loguru.hpp>

#include <deque>
#include <stack>

namespace ShuntingYard
{

f32 ZExpression::Evaluate() const
{
	Beard::Array<f32> ValueStack;

	auto WorkingQueue = EvaluationQueue;

	while (!WorkingQueue.IsEmpty())
	{
		auto* Node = WorkingQueue.First().get();
		WorkingQueue.PopAndDiscard();

		switch (Node->Type)
		{
			case ENodeType::Constant:
			{
				ValueStack.Add(reinterpret_cast<ConstantNode*>(Node)->Constant);
			}
			break;

			case ENodeType::Operator:
			{
				// TODO: Unary operators

				f32 Right = ValueStack.Pop();
				f32 Left  = ValueStack.Pop();

				ValueStack.Add(EvaluateOperator(reinterpret_cast<OperatorNode*>(Node), Left, Right));
			}
			break;

			case ENodeType::ZFunction:
			{
				ValueStack.Add(EvaluateFunction(reinterpret_cast<FunctionNode*>(Node)));
			}
			break;
		}
	}

	return ValueStack.Last();
}

f32 ZExpression::EvaluateOperator(OperatorNode* Operator, f32 Left, f32 Right)
{
	switch (Operator->Operator)
	{
		case EOperator::Add:
			return Operator->bUnary ? Left : Left + Right;
		case EOperator::Substract:
			return Operator->bUnary ? -Left : Left - Right;
		case EOperator::Multiply:
			return Left * Right;
		case EOperator::Divide:
			return Left / Right;

		case EOperator::OpenParen:
		case EOperator::CloseParen:
			// We should never have those in the evaluation queue
			ASSERT_UNREACHABLE();
			break;
	}

	return 0.0f;
}

inline bool IsOperator(ETokenType ZToken)
{
	switch (ZToken)
	{
		case ETokenType::Plus:
		case ETokenType::Minus:
		case ETokenType::Asterisk:
		case ETokenType::Slash:
			return true;

		default:
			break;
	}

	return false;
}

inline EOperator GetOperator(ETokenType ZToken)
{
	// clang-format off
	switch (ZToken)
	{
		case ETokenType::Plus: return EOperator::Add;
		case ETokenType::Minus: return EOperator::Substract;
		case ETokenType::Asterisk: return EOperator::Multiply;
		case ETokenType::Slash: return EOperator::Divide;
		default:
			ASSERT_UNREACHABLE();
			break;
	}
	// clang-format on

	return EOperator::OpenParen;
}

inline i32 GetPrecedence(EOperator Operator)
{
	// clang-format off
	switch (Operator)
	{
		case EOperator::Add:       return 2;
		case EOperator::Substract: return 2;
		case EOperator::Multiply:  return 3;
		case EOperator::Divide:    return 3;
		default:
			ASSERT_UNREACHABLE();
			break;
	}
	// clang-format on

	return 0;
}

ZExpression ZExpression::build(const Beard::Array<ZToken>& Tokens, ZWidget* RootWidget, ZWidget* CurrentWidget)
{
	ZExpression Result;

	std::stack<EOperator> OperatorStack;

	for (i32 i = 0; i < Tokens.ElementCount(); ++i)
	{
		ZToken ZToken = Tokens[i];

		switch (ZToken.Type)
		{
			case ETokenType::Number:
			{
				Result.EvaluationQueue.Add(std::make_shared<ConstantNode>(ZToken.Number));
			}
			break;

			case ETokenType::OpenParen:
			{
				OperatorStack.push(EOperator::OpenParen);
			}
			break;

			case ETokenType::CloseParen:
			{
				while (OperatorStack.top() != EOperator::OpenParen)
				{
					Result.EvaluationQueue.Add(std::make_shared<OperatorNode>(OperatorStack.top(), false));
					OperatorStack.pop();
				}

				OperatorStack.pop();
			}
			break;

			case ETokenType::Minus:
			case ETokenType::Plus:
			{
				bool bIsFirstElement      = (i == 0);
				bool bPreviousIsOpenParen = (i > 0) && Tokens[i - 1].Type == ETokenType::OpenParen;
				bool bPreviousIsOperator  = (i > 0) && IsOperator(Tokens[i - 1].Type);
				bool bIsUnary             = bIsFirstElement || bPreviousIsOpenParen || bPreviousIsOperator;

				bool bIsNotLast    = (i + 1) < Tokens.ElementCount();
				bool bNextIsNumber = bIsNotLast && (Tokens[i + 1].Type == ETokenType::Number);

				// Special case for bUnary plus / minus
				if (bIsUnary && bNextIsNumber)
				{
					f32 Mult = ZToken.Type == ETokenType::Minus ? -1.0f : 1.0f;
					Result.EvaluationQueue.Add(std::make_shared<ConstantNode>(Mult * Tokens[++i].Number));

					continue;
				}

				// We are looking at operators, just continue
			}

			case ETokenType::Asterisk:
			case ETokenType::Slash:
			{
				EOperator Operator = GetOperator(ZToken.Type);

				if (!OperatorStack.empty())
				{
					EOperator Other = OperatorStack.top();
					if (GetPrecedence(Operator) <= GetPrecedence(Other))
					{
						OperatorStack.pop();
						Result.EvaluationQueue.Add(std::make_shared<OperatorNode>(Other, false));
					}
				}

				OperatorStack.push(Operator);
			}
			break;

			case ETokenType::ZIdentifier:
			{
				bool hasTwoNextValues = (i + 2) < Tokens.ElementCount();
				bool nextIsDot        = hasTwoNextValues && (Tokens[i + 1].Type == ETokenType::Dot);
				bool nextIsProperty   = hasTwoNextValues && (Tokens[i + 2].Type == ETokenType::ZIdentifier);

				ZWidget* Widget = nullptr;

				std::string Binding;

				if (hasTwoNextValues && nextIsDot && nextIsProperty)
				{
					auto widgetId = ZToken.Text;
					if (widgetId == "parent")
					{
						Widget = CurrentWidget->Parent;
					}
					else
					{
						Widget = GetWidgetByID(RootWidget, widgetId);
					}

					Binding = Tokens[i + 2].Text;
					i += 2;
				}
				else
				{
					Widget  = CurrentWidget;
					Binding = ZToken.Text;
				}

				EFunction Function;

				if (Binding == "width")
				{
					Function = EFunction::Width;
				}
				else if (Binding == "height")
				{
					Function = EFunction::Height;
				}
				else if (Binding == "x")
				{
					Function = EFunction::X;
				}
				else if (Binding == "y")
				{
					Function = EFunction::Y;
				}
				else if (Binding == "bottom")
				{
					Function = EFunction::Bottom;
				}
				else if (Binding == "top")
				{
					Function = EFunction::Top;
				}
				else if (Binding == "left")
				{
					Function = EFunction::Left;
				}
				else if (Binding == "right")
				{
					Function = EFunction::Right;
				}

				Result.EvaluationQueue.Add(std::make_shared<FunctionNode>(Function, Widget));
			}
			break;

			default:
				ASSERT_UNREACHABLE();
				break;
		}
	}

	while (!OperatorStack.empty())
	{
		if (OperatorStack.top() == EOperator::OpenParen || OperatorStack.top() == EOperator::CloseParen)
		{
			LOG_F(ERROR, "Imbalanced expression");
		}
		else
		{
			Result.EvaluationQueue.Add(std::make_shared<OperatorNode>(OperatorStack.top(), false));
		}
		OperatorStack.pop();
	}

	return Result;
}

f32 ZExpression::EvaluateFunction(FunctionNode* Function)
{
	// clang-format off
	switch (Function->Function)
	{
		case EFunction::X:      return Function->Widget->Pos.x;
		case EFunction::Y:      return Function->Widget->Pos.y;
		case EFunction::Width:  return Function->Widget->Size.x;
		case EFunction::Height: return Function->Widget->Size.y;

		case EFunction::Left:   return Function->Widget->Pos.x;
		case EFunction::Right:  return Function->Widget->Pos.x + Function->Widget->Size.x;
		case EFunction::Top:    return Function->Widget->Pos.y;
		case EFunction::Bottom: return Function->Widget->Pos.y + Function->Widget->Size.y;
	}
	// clang-format on

	return 0.0f;
}
}