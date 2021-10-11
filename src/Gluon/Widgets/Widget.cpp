#include <Gluon/Widgets/Widget.h>

#include <Gluon/Widgets/Hashes.h>

#include <Gluon/Core/Utils.h>
#include <Gluon/Core/Containers/HashMap.h>

#include <loguru.hpp>

#include <filesystem>

namespace Utils
{
glm::vec4 extract_color(const std::vector<Token>& tokens)
{
	glm::vec4 color = {};

	if (!tokens.empty())
	{
		Token first_token = tokens.front();
		switch (first_token.type)
		{
			case TokenType::String:
			{
				ASSERT(tokens.size() == 1,
				       "A color as a string can only be defined by one token");
				first_token.text.erase(std::remove_if(first_token.text.begin(),
				                                      first_token.text.end(),
				                                      [](const char c)
				                                      { return c == '"'; }),
				                       first_token.text.end());

				color = GluonColor::from_string(first_token.text);
			}
			break;

			case TokenType::Identifier:
			{
				static constexpr auto RGB_HASH  = Utils::crc32("rgb");
				static constexpr auto RGBA_HASH = Utils::crc32("rgba");
				static constexpr auto HSL_HASH  = Utils::crc32("hsl");
				static constexpr auto HSLA_HASH = Utils::crc32("hsla");

				const auto hash = Utils::crc32(first_token.text);

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
							color = GluonColor::from_rgba((i32)values[0],
							                              (i32)values[1],
							                              (i32)values[2]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed rgb() call, should be "
							      "rgb(red, green, blue)",
							      first_token.line);
						}
					}
					break;

					case RGBA_HASH:
					{
						if (values.size() == 4)
						{
							color = GluonColor::from_rgba((i32)values[0],
							                              (i32)values[1],
							                              (i32)values[2],
							                              values[3]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed rgba() call, should be "
							      "rgba(red, green, blue, alpha)",
							      first_token.line);
						}
					}
					break;

					case HSL_HASH:
					{
						if (values.size() == 3)
						{
							color = GluonColor::from_hsla((i32)values[0],
							                              (i32)values[1],
							                              (i32)values[2]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed hsl() call, should be "
							      "hsl(hue, saturation, lightness)",
							      first_token.line);
						}
					}
					break;

					case HSLA_HASH:
					{
						if (values.size() == 4)
						{
							color = GluonColor::from_hsla((i32)values[0],
							                              (i32)values[1],
							                              (i32)values[2],
							                              values[3]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed hsla() call, should be "
							      "hsla(hue, saturation, lightness, alpha",
							      first_token.line);
						}
					}
					break;
					default:
					{
						if (first_token.text.compare("Color") == 0)
						{
							ASSERT(tokens.size() == 3 &&
							           tokens[2].type == TokenType::Identifier,
							       "We are looking for `Color.ColorName`");
							auto it = GluonColor::s_colors_by_name.find(
							    tokens[2].text);
							if (it != GluonColor::s_colors_by_name.end())
							{
								color = it->second;
							}
							else
							{
								LOG_F(ERROR,
								      "ling %d: Color '%s' is not a valid "
								      "color name. See "
								      "https://www.w3schools.com/colors/"
								      "colors_names.asp for a complete list.",
								      first_token.line,
								      tokens[2].text.c_str());
							}
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Could not parse color given '%s' "
							      "value",
							      first_token.line,
							      first_token.text.c_str());
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

std::vector<GluonWidget*> GluonWidget::s_widget_map = {};

extern StringHashMap<WidgetFactory*> s_widget_factories;

GluonWidget::~GluonWidget()
{
	s_widget_map.erase(
	    std::find(s_widget_map.begin(), s_widget_map.end(), this));

	for (auto&& c : children)
	{
		delete c;
	}
	children.clear();
}

void GluonWidget::deserialize(Parser::Node::Ptr node)
{
	s_widget_map.push_back(this);

	for (auto c : node->children)
	{
		if (c->type == Parser::Node_Structure)
		{
			auto child = (*s_widget_factories[c->name])();

			children.push_back(child);

			child->parent = this;

			child->deserialize(c);
		}
		else if (c->type == Parser::Node_Property)
		{
			parse_property(c);
		}
	}
}

void GluonWidget::build_render_infos(std::vector<RectangleInfo>* result)
{
	build_render_infos_internal(result);
	for (auto c : children)
	{
		c->build_render_infos(result);
	}
}

void GluonWidget::evaluate()
{
	for (auto&& w : s_widget_map)
	{
		w->pre_evaluate();
	}

	for (auto&& w : s_widget_map)
	{
		w->evaluate_internal();
	}

	for (auto&& w : s_widget_map)
	{
		w->post_evaluate();
	}
}

void GluonWidget::evaluate_internal()
{
	if (dirty)
	{
		for (auto d : dependencies)
		{
			d->evaluate_internal();
		}

		for (auto&& eval : evaluators)
		{
			*eval.first = eval.second.evaluate();
		}

		dirty = false;
	}
}

void GluonWidget::touch()
{
	dirty = true;
	for (auto d : dependants)
	{
		d->touch();
	}
}

bool GluonWidget::window_resized(i32 w, i32 h)
{
	bool needUpdate = false;

	for (auto c : children)
	{
		needUpdate |= c->window_resized(w, h);
	}

	return needUpdate;
}

void GluonWidget::parse_property(Parser::Node::Ptr node)
{
	const auto node_hash = Utils::crc32(node->name);
	switch (node_hash)
	{
		case NodeHash::ID:
		{
			ASSERT(node->children.size() == 1, "id: <id>");
			id = node->children[0]->name;
		}
		break;

		case NodeHash::X:
		case NodeHash::Y:
		case NodeHash::Width:
		case NodeHash::Height:
		{
			auto infos = node->children[0];

			geometry_expressions[node_hash] = infos->associated_tokens;

			for (usize i = 0; i < infos->associated_tokens.size() - 1; ++i)
			{
				if (infos->associated_tokens[i].type == TokenType::Identifier &&
				    infos->associated_tokens[i + 1].type == TokenType::Dot)
				{
					dependency_ids.insert(infos->associated_tokens[i].text);
				}
			}
		}
		break;

		case NodeHash::Anchors:
		{
			LOG_F(WARNING, "Anchors not handled yet");
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
			parse_property_internal(node, node_hash);
		}
		break;
	}
}

GluonWidget* get_widget_by_id(GluonWidget* root_widget, const std::string& name)
{
	if (root_widget->id == name)
	{
		return root_widget;
	}

	for (auto child : root_widget->children)
	{
		if (auto node = get_widget_by_id(child, name); node != nullptr)
		{
			return node;
		}
	}

	return nullptr;
}

void build_dependency_graph(GluonWidget* root_widget,
                            GluonWidget* current_widget)
{
	for (auto id : current_widget->dependency_ids)
	{
		GluonWidget* dep = id == "parent" ? current_widget->parent
		                                  : get_widget_by_id(root_widget, id);

		if (dep != nullptr)
		{
			current_widget->dependencies.push_back(dep);
			dep->dependants.push_back(current_widget);
		}
	}

	for (auto c : current_widget->children)
	{
		build_dependency_graph(root_widget, c);
	}
}

void build_expression_evaluators(GluonWidget* root_widget,
                                 GluonWidget* current_widget)
{
	auto get_hash_index = [](u32 hash)
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
				ASSERT_UNREACHABLE();
				return -1;
		}
	};

	auto get_property_ptr = [&current_widget](u32 hash) -> f32*
	{
		switch (hash)
		{
			case NodeHash::X:
				return &current_widget->pos.x;
			case NodeHash::Y:
				return &current_widget->pos.y;
			case NodeHash::Width:
				return &current_widget->size.x;
			case NodeHash::Height:
				return &current_widget->size.y;

			default:
				ASSERT_UNREACHABLE();
				return nullptr;
		}
	};

	std::unordered_set<u32> remaining_attributes = {NodeHash::X,
	                                                NodeHash::Y,
	                                                NodeHash::Width,
	                                                NodeHash::Height};
	for (auto expr : current_widget->geometry_expressions)
	{
		remaining_attributes.erase(expr.first);

		auto expression = ShuntingYard::Expression::build(expr.second,
		                                                  root_widget,
		                                                  current_widget);

		current_widget->evaluators.push_back(
		    std::make_pair(get_property_ptr(expr.first), expression));
	}

	for (auto c : current_widget->children)
	{
		build_expression_evaluators(root_widget, c);
	}
}
