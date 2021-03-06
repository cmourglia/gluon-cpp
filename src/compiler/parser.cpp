#include "parser.h"

#include "tokenizer.h"

#include <loguru.hpp>

#include <fstream>
#include <memory>
#include <functional>
#include <unordered_map>

// void from_json(const json& j, RectangleInfo& r)
// {
// 	r = {};

// 	const glm::vec2 p = glm::vec2(j.value("x", 0.0f), j.value("y", 0.0f));
// 	const glm::vec2 s = glm::vec2(j.value("w", 0.0f), j.value("h", 0.0f));

// 	r.position = p + (s * 0.5f);
// 	r.size     = s;

// 	r.radius = j.value("radius", 0.0f);

// 	if (const auto& fillColor = j["fillColor"]; fillColor.is_array())
// 	{
// 		switch (fillColor.size())
// 		{
// 			case 1:
// 			{
// 				r.fillColor = glm::vec4(fillColor[0].get<f32>(),
// 				                        fillColor[0].get<f32>(),
// 				                        fillColor[0].get<f32>(),
// 				                        1.0f);
// 			}
// 			break;

// 			case 3:
// 			{
// 				r.fillColor = glm::vec4(fillColor[0].get<f32>(),
// 				                        fillColor[1].get<f32>(),
// 				                        fillColor[2].get<f32>(),
// 				                        1.0f);
// 			}
// 			break;

// 			case 4:
// 			{
// 				r.fillColor = glm::vec4(fillColor[0].get<f32>(),
// 				                        fillColor[1].get<f32>(),
// 				                        fillColor[2].get<f32>(),
// 				                        fillColor[3].get<f32>());
// 			}
// 			break;

// 			default:
// 				// ignore
// 				break;
// 		}
// 	}
// }

enum NodeType
{
	Node_Unknown,

	Node_Structure,
	Node_Property,
	Node_Value,
};

struct Node
{
	explicit Node(NodeType t)
	    : type(t)
	{
	}

	NodeType    type;
	std::string name;

	std::vector<Token> associatedTokens;

	Node*                              parent;
	std::vector<std::shared_ptr<Node>> children;
};

struct StructureNode : public Node
{
	StructureNode()
	    : Node(Node_Structure)
	{
	}
};

struct PropertyNode : public Node
{
	PropertyNode()
	    : Node(Node_Property)
	{
	}
};

struct ValueNode : Node
{
	ValueNode()
	    : Node(Node_Value)
	{
	}
};

enum ParserState
{
	State_Initial,

	State_Structure,
	State_Property,
	State_Value,
};

struct Widget;

typedef std::shared_ptr<Widget> WidgetFactory(void);

static std::unordered_map<std::string, WidgetFactory*> widgetFactories;

struct Widget
{
	virtual void Deserialize(std::shared_ptr<Node> node) final
	{
		for (auto c : node->children)
		{
			if (c->type == Node_Structure)
			{
				auto child = (*widgetFactories[c->name])();
				children.push_back(child);

				child->Deserialize(c);
			}
			else if (c->type == Node_Property)
			{
				ParseProperty(c);
			}
		}
	}

	std::vector<std::shared_ptr<Widget>> children;
	Widget*                              parent = nullptr;

	glm::vec2 pos  = glm::vec2(0.0f);
	glm::vec2 size = glm::vec2(0.0f);

private:
	virtual void ParseProperty(std::shared_ptr<Node> node) final
	{
		// TODO/FIXME: This is absolutely terrible code.
		if (node->name == "x")
		{
			assert(node->children.size() == 1);
			pos.x = std::stof(node->children[0]->name);
		}
		else if (node->name == "y")
		{
			assert(node->children.size() == 1);
			pos.y = std::stof(node->children[0]->name);
		}
		else if (node->name == "pos" || node->name == "position")
		{
			assert(node->children.size() == 2);
			pos.x = std::stof(node->children[0]->name);
			pos.y = std::stof(node->children[1]->name);
		}
		else if (node->name == "w")
		{
			assert(node->children.size() == 1);
			size.x = std::stof(node->children[0]->name);
		}
		else if (node->name == "h")
		{
			assert(node->children.size() == 1);
			size.y = std::stof(node->children[0]->name);
		}
		else if (node->name == "size")
		{
			assert(node->children.size() == 2);
			size.x = std::stof(node->children[0]->name);
			size.y = std::stof(node->children[1]->name);
		}
		else if (node->name == "geometry")
		{
			assert(node->children.size() == 4);
			assert(node->children[0]->associatedTokens.size() == 1);

			pos.x  = std::stof(node->children[0]->name);
			pos.y  = std::stof(node->children[1]->name);
			size.x = std::stof(node->children[2]->name);
			size.y = std::stof(node->children[3]->name);
		}
		else
		{
			ParsePropertyVirtual(node);
		}
	}

	virtual void ParsePropertyVirtual(std::shared_ptr<Node> node) = 0;
};

struct Window : public Widget
{
	static std::shared_ptr<Widget> Create()
	{
		return std::make_shared<Window>();
	}

	std::string title           = "Gluon";
	glm::vec4   backgroundColor = Color::Chocolate;

private:
	virtual void ParsePropertyVirtual(std::shared_ptr<Node> node) override
	{
		LOG_F(INFO, "Hello window property %s", node->name.c_str());
	}
};

struct Rectangle : public Widget
{
	static std::shared_ptr<Widget> Create()
	{
		return std::make_shared<Rectangle>();
	}

private:
	virtual void ParsePropertyVirtual(std::shared_ptr<Node> node) override
	{
		LOG_F(INFO, "Hello rectangle property %s", node->name.c_str());
	}
};

// TODO: Output an intermediate scene graph instead of a flat RectangleInfo vector
std::vector<RectangleInfo> ParseGluonFile(const char* file)
{
	if (widgetFactories.empty())
	{
		// FIXME: How / when do we wanna build this map ?
		widgetFactories["Window"]    = &Window::Create;
		widgetFactories["Rectangle"] = &Rectangle::Create;
	}

	auto tokens = Tokenize(file);

	std::shared_ptr<Node> root;
	Node*                 currentNode;

	auto remove_it = std::remove_if(tokens.begin(), tokens.end(), [](const Token& token) {
		return token.type == Token_Spacing;
	});
	tokens.erase(remove_it, tokens.end());

	Token* token = tokens.data();

	ParserState state = State_Initial;

	bool done = false;
	while (!done)
	{
		if (state == State_Property)
		{
			std::vector<Token> valueTokens;

			std::string name;

			if (token->type == Token_Colon)
			{
				++token;
			}

			while (!(token->type == Token_EndOfLine || token->type == Token_Semicolon))
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
			state       = State_Structure;
		}
		else
		{
			switch (token->type)
			{
				case Token_EndOfStream:
					done = true;
					break;

				case Token_Identifier:
				{
					if (state == State_Initial)
					{

						// We MUST start with a structure node
						auto node    = std::make_shared<StructureNode>();
						node->name   = token->text;
						node->parent = nullptr;
						node->associatedTokens.push_back(*token);

						root        = node;
						currentNode = root.get();

						state = State_Structure;
					}
					else if (state == State_Structure)
					{
						// Either we have a property or a child structure
						if (token[1].type == Token_OpenBrace)
						{

							auto node    = std::make_shared<StructureNode>();
							node->name   = token->text;
							node->parent = currentNode;
							currentNode->children.push_back(node);
							currentNode = node.get();

							state = State_Structure;
						}
						else if (token[1].type == Token_Colon)
						{
							auto node    = std::make_shared<PropertyNode>();
							node->name   = token->text;
							node->parent = currentNode;
							currentNode->children.push_back(node);
							currentNode = node.get();

							state = State_Property;
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

				case Token_CloseBrace:
				{
					currentNode = currentNode->parent;
				}

				default:
					break;
			}
		}

		++token;
	}

	auto rootWidget = (*widgetFactories[root->name])();
	rootWidget->Deserialize(root);

	std::vector<RectangleInfo> result;

	return result;
}