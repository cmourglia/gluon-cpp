#include <Gluon/Compiler/Gluon_Parser.h>

#include <Gluon/Compiler/Gluon_Tokenizer.h>

#include <Gluon/Widgets/Gluon_Image.h>
#include <Gluon/Widgets/Gluon_Rectangle.h>
#include <Gluon/Widgets/Gluon_Text.h>
#include <Gluon/Widgets/Gluon_Widget.h>
#include <Gluon/Widgets/Gluon_Window.h>

#include <Beard/HashMap.h>

#include <loguru.hpp>

#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <stack>
#include <unordered_set>

using namespace Parser;

enum class ParserState
{
	Initial,

	Structure,
	Property,
	ZValue,
};

enum class GeometryPolicy
{
	Absolute,
	Relative,
	Anchor,
};

Beard::StringHashMap<ZWidgetFactory*> s_WidgetFactories;

ZWidget* parse_gluon_buffer(const char* Buffer)
{
	if (s_WidgetFactories.IsEmpty())
	{
		// FIXME: How / when do we wanna build this map ?
		s_WidgetFactories["Window"]    = &ZWindow::create;
		s_WidgetFactories["Rectangle"] = &ZRectangle::create;
		s_WidgetFactories["Image"]     = &ZImage::create;
		s_WidgetFactories["Text"]      = &ZText::create;
	}

	auto Tokens = Tokenize(Buffer);

	ZNode::Ptr root;
	ZNode*     currentNode = nullptr;

	auto remove_it = std::remove_if(Tokens.begin(),
	                                Tokens.end(),
	                                [](const ZToken& token) { return token.Type == ETokenType::Spacing; });
	Tokens.RemoveRange(remove_it, Tokens.end());

	ZToken* token = Tokens.Data();

	ParserState state = ParserState::Initial;

	bool done = false;
	while (!done)
	{
		if (state == ParserState::Property)
		{
			Beard::Array<ZToken> valueTokens;

			std::string Name;

			if (token->Type == ETokenType::Colon)
			{
				++token;
			}

			while (!(token->Type == ETokenType::EndOfLine || token->Type == ETokenType::Semicolon))
			{
				valueTokens.Add(*token);
				Name += token->Text;

				++token;
			}

			auto Node              = std::make_shared<ValueNode>();
			Node->Name             = Name;
			Node->Parent           = currentNode;
			Node->AssociatedTokens = valueTokens;
			currentNode->Children.Add(Node);

			currentNode = currentNode->Parent;
			state       = ParserState::Structure;
		}
		else
		{
			switch (token->Type)
			{
				case ETokenType::EndOfStream:
					done = true;
					break;

				case ETokenType::ZIdentifier:
				{
					if (state == ParserState::Initial)
					{

						// We MUST start with a structure Node
						auto Node    = std::make_shared<StructureNode>();
						Node->Name   = token->Text;
						Node->Parent = nullptr;
						Node->AssociatedTokens.Add(*token);

						root        = Node;
						currentNode = root.get();

						state = ParserState::Structure;
					}
					else if (state == ParserState::Structure)
					{
						// Either we have a property or a child structure
						if (token[1].Type == ETokenType::OpenBrace)
						{

							auto Node    = std::make_shared<StructureNode>();
							Node->Name   = token->Text;
							Node->Parent = currentNode;
							currentNode->Children.Add(Node);
							currentNode = Node.get();

							state = ParserState::Structure;
						}
						else if (token[1].Type == ETokenType::Colon)
						{
							auto Node    = std::make_shared<PropertyNode>();
							Node->Name   = token->Text;
							Node->Parent = currentNode;
							currentNode->Children.Add(Node);
							currentNode = Node.get();

							state = ParserState::Property;
						}
						// FIXME(Charly): Maybe we should try to find a way to
						// avoid code duplication ?
						else if (token[1].Type == ETokenType::Dot)
						{
							auto Node    = std::make_shared<PropertyNode>();
							Node->Name   = token->Text;
							Node->Parent = currentNode;

							// FIXME(Charly): This loop is not safe
							for (int i = 2; token[i].Type != ETokenType::Colon; ++i)
							{
								if (token[i].Type == ETokenType::ZIdentifier)
								{
									Node->AssociatedTokens.Add(token[i]);
								}
							}

							currentNode->Children.Add(Node);
							currentNode = Node.get();

							state = ParserState::Property;
						}
						else
						{
							LOG_F(ERROR,
							      "Invalid identifier token %s (%d %d)",
							      token->Text.c_str(),
							      token->Line,
							      token->Column);
						}
					}
				}
				break;

				case ETokenType::CloseBrace:
				{
					currentNode = currentNode->Parent;
				}

				default:
					break;
			}
		}

		++token;
	}

	Beard::Array<RectangleInfo> Result;

	ZWidget* RootWidget = nullptr;

	if (root)
	{
		RootWidget = (*s_WidgetFactories[root->Name])();
		RootWidget->deserialize(root);

		BuildDependencyGraph(RootWidget, RootWidget);
		BuildExpressionEvaluators(RootWidget, RootWidget);
	}
	else
	{
		LOG_F(ERROR, "Something wrong happened");
	}

	return RootWidget;
}
