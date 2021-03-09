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

namespace
{
static constexpr uint32_t CRC_TABLE[256] =
    {0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
     0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
     0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
     0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
     0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
     0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
     0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
     0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
     0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
     0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
     0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
     0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
     0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
     0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
     0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
     0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
     0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
     0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
     0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
     0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
     0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
     0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
     0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
     0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
     0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
     0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
     0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
     0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
     0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
     0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
     0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
     0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL};

inline constexpr uint32_t Crc32(std::string_view str)
{
	uint32_t crc = 0xffffffff;
	for (auto c : str)
	{
		crc = (crc >> 8) ^ CRC_TABLE[(crc ^ c) & 0xff];
	}
	return crc ^ 0xffffffff;
}

inline glm::vec4 ExtractColor(const std::vector<Token>& tokens)
{
	glm::vec4 color = {};

	if (!tokens.empty())
	{
		Token firstToken = tokens.front();
		switch (firstToken.type)
		{
			case Token_String:
			{
				assert(tokens.size() == 1);
				firstToken.text.erase(std::remove_if(firstToken.text.begin(),
				                                     firstToken.text.end(),
				                                     [](const char c) { return c == '"'; }),
				                      firstToken.text.end());

				color = Color::FromString(firstToken.text);
			}
			break;

			case Token_Identifier:
			{
				static constexpr auto RGB_HASH  = Crc32("rgb");
				static constexpr auto RGBA_HASH = Crc32("rgba");
				static constexpr auto HSL_HASH  = Crc32("hsl");
				static constexpr auto HSLA_HASH = Crc32("hsla");

				const auto hash = Crc32(firstToken.text);

				std::vector<f32> values;
				for (const auto& t : tokens)
				{
					if (t.type == Token_Number)
					{
						values.push_back(t.number);
					}
				}

				switch (hash)
				{
					case RGB_HASH:
					{
						if (values.size() == 3)
						{
							color = Color::FromRgba(values[0], values[1], values[2]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed rgb() call, should be rgb(red, green, blue)",
							      firstToken.line);
						}
					}
					break;

					case RGBA_HASH:
					{
						if (values.size() == 4)
						{
							color = Color::FromRgba(values[0], values[1], values[2], values[3]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed rgba() call, should be rgba(red, green, blue, alpha)",
							      firstToken.line);
						}
					}
					break;

					case HSL_HASH:
					{
						if (values.size() == 3)
						{
							color = Color::FromHsla(values[0], values[1], values[2]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed hsl() call, should be hsl(hue, saturation, lightness)",
							      firstToken.line);
						}
					}
					break;

					case HSLA_HASH:
					{
						if (values.size() == 4)
						{
							color = Color::FromHsla(values[0], values[1], values[2], values[3]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed hsla() call, should be hsla(hue, saturation, lightness, alpha",
							      firstToken.line);
						}
					}
					break;
					default:
					{
						if (firstToken.text.compare("Color") == 0)
						{
							assert(tokens.size() == 3 && tokens[2].type == Token_Identifier);
							auto it = Color::ColorsByName.find(tokens[2].text);
							if (it != Color::ColorsByName.end())
							{
								color = it->second;
							}
							else
							{
								LOG_F(ERROR,
								      "ling %d: Color '%s' is not a valid color name. See "
								      "https://www.w3schools.com/colors/colors_names.asp for a complete list.",
								      firstToken.line,
								      tokens[2].text.c_str());
							}
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Could not parse color given '%s' value",
							      firstToken.line,
							      firstToken.text.c_str());
						}
					}
					break;
				}
			}
		}
	}

	return color;
}
}

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

	virtual void BuildRenderInfos(std::vector<RectangleInfo>* result) final
	{
		BuildRenderInfosInternal(result);
		for (auto c : children)
		{
			c->BuildRenderInfos(result);
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
			ParsePropertyInternal(node);
		}
	}

	virtual void ParsePropertyInternal(std::shared_ptr<Node> node) = 0;

	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) = 0;
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
	virtual void ParsePropertyInternal(std::shared_ptr<Node> node) override
	{
		LOG_F(INFO, "Hello window property %s", node->name.c_str());
	}

	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result)
	{
		// Passthrough
	}
};

struct Rectangle : public Widget
{
	static std::shared_ptr<Widget> Create()
	{
		return std::make_shared<Rectangle>();
	}

	glm::vec4 fillColor;
	glm::vec4 borderColor;

private:
	virtual void ParsePropertyInternal(std::shared_ptr<Node> node) override
	{
		LOG_F(INFO, "Hello rectangle property %s", node->name.c_str());
		if (node->name.compare("color") == 0)
		{
			assert(!node->children.empty());
			fillColor = ExtractColor(node->children[0]->associatedTokens);
		}
	}

	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result)
	{
		RectangleInfo rect = {
		    .position  = pos,
		    .size      = size,
		    .fillColor = fillColor,
		};

		result->push_back(rect);
	}
};

// TODO: Output an intermediate scene graph instead of a flat RectangleInfo vector
std::vector<RectangleInfo> ParseGluonBuffer(std::string_view buffer)
{
	if (widgetFactories.empty())
	{
		// FIXME: How / when do we wanna build this map ?
		widgetFactories["Window"]    = &Window::Create;
		widgetFactories["Rectangle"] = &Rectangle::Create;
	}

	auto tokens = Tokenize(buffer);

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

	std::vector<RectangleInfo> result;

	if (root)
	{
		auto rootWidget = (*widgetFactories[root->name])();
		rootWidget->Deserialize(root);

		rootWidget->BuildRenderInfos(&result);
	}
	else
	{
		LOG_F(ERROR, "Something wrong happened");
	}

	return result;
}