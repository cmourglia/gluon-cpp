#include "parser.h"

#include "tokenizer.h"
#include "widget.h"

#include <loguru.hpp>

#include <fstream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <queue>

using namespace Parser;

enum class ParserState
{
	Initial,

	Structure,
	Property,
	Value,
};

enum class GeometryPolicy
{
	Absolute,
	Relative,
	Anchor,
};

std::unordered_map<std::string, WidgetFactory*> widgetFactories;

void EvaluateWidgets(MuWidget* rootWidget) { }

// TODO: Output an intermediate scene graph instead of a flat RectangleInfo vector
std::vector<RectangleInfo> ParseGluonBuffer(std::string_view buffer)
{
	if (widgetFactories.empty())
	{
		// FIXME: How / when do we wanna build this map ?
		widgetFactories["Window"]    = &MuWindow::Create;
		widgetFactories["Rectangle"] = &MuRectangle::Create;
		widgetFactories["Image"]     = &MuImage::Create;
	}

	auto tokens = Tokenize(buffer);

	Node::Ptr root;
	Node*     currentNode;

	auto remove_it = std::remove_if(tokens.begin(),
	                                tokens.end(),
	                                [](const Token& token) { return token.type == TokenType::Spacing; });
	tokens.erase(remove_it, tokens.end());

	Token* token = tokens.data();

	ParserState state = ParserState::Initial;

	bool done = false;
	while (!done)
	{
		if (state == ParserState::Property)
		{
			std::vector<Token> valueTokens;

			std::string name;

			if (token->type == TokenType::Colon)
			{
				++token;
			}

			while (!(token->type == TokenType::EndOfLine || token->type == TokenType::Semicolon))
			{
				valueTokens.push_back(*token);
				name += token->text;

				++token;
			}

			auto node              = std::make_shared<ValueNode>();
			node->name             = name;
			node->parent           = currentNode;
			node->associatedTokens = valueTokens;
			currentNode->children.push_back(node);

			currentNode = currentNode->parent;
			state       = ParserState::Structure;
		}
		else
		{
			switch (token->type)
			{
				case TokenType::EndOfStream:
					done = true;
					break;

				case TokenType::Identifier:
				{
					if (state == ParserState::Initial)
					{

						// We MUST start with a structure node
						auto node    = std::make_shared<StructureNode>();
						node->name   = token->text;
						node->parent = nullptr;
						node->associatedTokens.push_back(*token);

						root        = node;
						currentNode = root.get();

						state = ParserState::Structure;
					}
					else if (state == ParserState::Structure)
					{
						// Either we have a property or a child structure
						if (token[1].type == TokenType::OpenBrace)
						{

							auto node    = std::make_shared<StructureNode>();
							node->name   = token->text;
							node->parent = currentNode;
							currentNode->children.push_back(node);
							currentNode = node.get();

							state = ParserState::Structure;
						}
						else if (token[1].type == TokenType::Colon)
						{
							auto node    = std::make_shared<PropertyNode>();
							node->name   = token->text;
							node->parent = currentNode;
							currentNode->children.push_back(node);
							currentNode = node.get();

							state = ParserState::Property;
						}
						// FIXME(Charly): Maybe we should try to find a way to avoid code duplication ?
						else if (token[1].type == TokenType::Dot)
						{
							auto node    = std::make_shared<PropertyNode>();
							node->name   = token->text;
							node->parent = currentNode;

							// FIXME(Charly): This loop is not safe
							for (int i = 2; token[i].type != TokenType::Colon; ++i)
							{
								if (token[i].type == TokenType::Identifier)
								{
									node->associatedTokens.push_back(token[i]);
								}
							}

							currentNode->children.push_back(node);
							currentNode = node.get();

							state = ParserState::Property;
						}
						else
						{
							LOG_F(ERROR,
							      "Invalid identifier token %s (%d %d)",
							      token->text.c_str(),
							      token->line,
							      token->column);
						}
					}
				}
				break;

				case TokenType::CloseBrace:
				{
					currentNode = currentNode->parent;
				}

				default:
					break;
			}
		}

		++token;
	}

	std::vector<RectangleInfo> result;

	if (root)
	{
		MuWidget::widgetMap.clear();

		auto rootWidget = (*widgetFactories[root->name])();
		MuWidget::widgetMap.push_back(rootWidget);
		rootWidget->Deserialize(root);

		BuildDependencyGraph(rootWidget.get(), rootWidget.get());
		BuildExpressionEvaluators(rootWidget.get(), rootWidget.get());

		for (auto&& widget : MuWidget::widgetMap)
		{
			widget->Evaluate();
		}

		for (auto&& widget : MuWidget::widgetMap)
		{
			widget->PostEvaluate();
		}

		rootWidget->BuildRenderInfos(&result);
	}
	else
	{
		LOG_F(ERROR, "Something wrong happened");
	}

	return result;
}