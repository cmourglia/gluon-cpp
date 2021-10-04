#include "widget.h"

#include "parser.h"
#include "utils.h"

#include <loguru.hpp>

namespace NodeHash
{
// Generic node names
static constexpr uint32_t ID      = Utils::Crc32("id");
static constexpr uint32_t X       = Utils::Crc32("x");
static constexpr uint32_t Y       = Utils::Crc32("y");
static constexpr uint32_t Width   = Utils::Crc32("width");
static constexpr uint32_t Height  = Utils::Crc32("height");
static constexpr uint32_t Anchors = Utils::Crc32("anchors");
static constexpr uint32_t Padding = Utils::Crc32("padding");
static constexpr uint32_t Margins = Utils::Crc32("margins");

// Window
static constexpr uint32_t Title = Utils::Crc32("title");

// Rectangle
static constexpr uint32_t Color = Utils::Crc32("color");
}

namespace
{

inline glm::vec4 ExtractColor(const std::vector<Token>& tokens)
{
	glm::vec4 color = {};

	if (!tokens.empty())
	{
		Token firstToken = tokens.front();
		switch (firstToken.type)
		{
			case TokenType::String:
			{
				assert(tokens.size() == 1);
				firstToken.text.erase(std::remove_if(firstToken.text.begin(),
				                                     firstToken.text.end(),
				                                     [](const char c) { return c == '"'; }),
				                      firstToken.text.end());

				color = MuColor::FromString(firstToken.text);
			}
			break;

			case TokenType::Identifier:
			{
				static constexpr auto RGB_HASH  = Utils::Crc32("rgb");
				static constexpr auto RGBA_HASH = Utils::Crc32("rgba");
				static constexpr auto HSL_HASH  = Utils::Crc32("hsl");
				static constexpr auto HSLA_HASH = Utils::Crc32("hsla");

				const auto hash = Utils::Crc32(firstToken.text);

				std::vector<f32> values;
				for (const auto& t : tokens)
				{
					if (t.type == TokenType::Number)
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
							color = MuColor::FromRgba(values[0], values[1], values[2]);
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
							color = MuColor::FromRgba(values[0], values[1], values[2], values[3]);
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
							color = MuColor::FromHsla(values[0], values[1], values[2]);
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
							color = MuColor::FromHsla(values[0], values[1], values[2], values[3]);
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
							assert(tokens.size() == 3 && tokens[2].type == TokenType::Identifier);
							auto it = MuColor::ColorsByName.find(tokens[2].text);
							if (it != MuColor::ColorsByName.end())
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

std::vector<Widget*> Widget::widgetMap = {};

extern std::unordered_map<std::string, WidgetFactory*> widgetFactories;

void Widget::Deserialize(Parser::Node::Ptr node)
{
	widgetMap.push_back(this);

	for (auto c : node->children)
	{
		if (c->type == Parser::Node_Structure)
		{
			auto child = (*widgetFactories[c->name])();
			children.push_back(child);

			child->parent = this;

			child->Deserialize(c);
		}
		else if (c->type == Parser::Node_Property)
		{
			ParseProperty(c);
		}
	}
}

void Widget::BuildRenderInfos(std::vector<RectangleInfo>* result)
{
	BuildRenderInfosInternal(result);
	for (auto c : children)
	{
		c->BuildRenderInfos(result);
	}
}

void Widget::Evaluate()
{
	if (dirty)
	{
		for (auto d : dependencies)
		{
			d->Evaluate();
		}

		for (auto&& eval : evaluators)
		{
			*eval.first = eval.second.Evaluate();
		}

		dirty = false;
	}
}

void Widget::Touch()
{
	dirty = true;
	for (auto d : dependants)
	{
		d->Touch();
	}
}

void Widget::ParseProperty(Parser::Node::Ptr node)
{
	const auto nodeHash = Utils::Crc32(node->name);
	switch (nodeHash)
	{
		case NodeHash::ID:
		{
			assert(node->children.size() == 1);
			id = node->children[0]->name;
		}
		break;

		case NodeHash::X:
		case NodeHash::Y:
		case NodeHash::Width:
		case NodeHash::Height:
		{
			auto infos = node->children[0];

			geometryExpressions[nodeHash] = infos->associatedTokens;

			for (usize i = 0; i < infos->associatedTokens.size() - 1; ++i)
			{
				if (infos->associatedTokens[i].type == TokenType::Identifier &&
				    infos->associatedTokens[i + 1].type == TokenType::Dot)
				{
					dependencyIds.insert(infos->associatedTokens[i].text);
				}
			}
		}
		break;

		case NodeHash::Anchors:
		{
			LOG_F(INFO, "I am an anchor");
		}
		break;

		case NodeHash::Padding:
		{
			LOG_F(WARNING, "Padding not handled yet");
		}
		break;

		case NodeHash::Margins:
		{
			LOG_F(WARNING, "Margins not handled yet");
		}
		break;

		default:
		{
			ParsePropertyInternal(node, nodeHash);
		}
		break;
	}
}

void Window::ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash)
{
	pos  = {0.0f, 0.0f};
	size = {1024.0f, 768.0f};
}

void Window::BuildRenderInfosInternal(std::vector<RectangleInfo>* result)
{
	// Passthrough
}

void Rectangle::ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash)
{
	if (nodeHash == NodeHash::Color)
	{
		assert(!node->children.empty());
		fillColor = ExtractColor(node->children[0]->associatedTokens);
	}
}

void Rectangle::BuildRenderInfosInternal(std::vector<RectangleInfo>* result)
{
	RectangleInfo rect = {
	    .position  = pos,
	    .size      = size,
	    .fillColor = fillColor,
	};

	result->push_back(rect);
}

Widget* GetWidgetById(Widget* rootWidget, const std::string& name)
{
	if (rootWidget->id == name)
	{
		return rootWidget;
	}

	for (auto child : rootWidget->children)
	{
		if (auto node = GetWidgetById(child.get(), name); node != nullptr)
		{
			return node;
		}
	}

	return nullptr;
}

void BuildDependencyGraph(Widget* rootWidget, Widget* currentWidget)
{
	for (auto id : currentWidget->dependencyIds)
	{
		Widget* dep = id == "parent" ? currentWidget->parent : GetWidgetById(rootWidget, id);

		if (dep != nullptr)
		{
			currentWidget->dependencies.push_back(dep);
			dep->dependants.push_back(currentWidget);
		}
	}

	for (auto c : currentWidget->children)
	{
		BuildDependencyGraph(rootWidget, c.get());
	}
}

void BuildExpressionEvaluators(Widget* rootWidget, Widget* currentWidget)
{
	auto GetHashIndex = [](u32 hash)
	{
		switch (hash)
		{
			case NodeHash::X:
				return 0;
			case NodeHash::Y:
				return 1;
			case NodeHash::Width:
				return 2;
			case NodeHash::Height:
				return 3;

			default:
				assert(false);
				return -1;
		}
	};

	auto GetPropertyPtr = [&currentWidget](u32 hash) -> f32*
	{
		switch (hash)
		{
			case NodeHash::X:
				return &currentWidget->pos.x;
			case NodeHash::Y:
				return &currentWidget->pos.y;
			case NodeHash::Width:
				return &currentWidget->size.x;
			case NodeHash::Height:
				return &currentWidget->size.y;

			default:
				assert(false);
				return nullptr;
		}
	};

	std::unordered_set<u32> remainingAttributes = {NodeHash::X, NodeHash::Y, NodeHash::Width, NodeHash::Height};
	for (auto expr : currentWidget->geometryExpressions)
	{
		remainingAttributes.erase(expr.first);

		auto expression = ShuntingYard::Expression::Build(expr.second, rootWidget, currentWidget);

		currentWidget->evaluators.push_back(std::make_pair(GetPropertyPtr(expr.first), expression));
	}

	for (auto c : currentWidget->children)
	{
		BuildExpressionEvaluators(rootWidget, c.get());
	}
}
