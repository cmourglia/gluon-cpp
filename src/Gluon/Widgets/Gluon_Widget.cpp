#include <Gluon/Widgets/Gluon_Widget.h>

#include <Gluon/Widgets/Gluon_Hashes.h>

#include <Beard/Hash.h>
#include <Beard/HashMap.h>

#include <loguru.hpp>

#include <filesystem>

namespace Utils
{
glm::vec4 ExtractColor(const Beard::Array<ZToken>& Tokens)
{
	glm::vec4 Color = {};

	if (!Tokens.IsEmpty())
	{
		ZToken first_token = Tokens.First();
		switch (first_token.Type)
		{
			case ETokenType::String:
			{
				ASSERT(Tokens.ElementCount() == 1, "A Color as a string can only be defined by one token");
				first_token.Text.erase(std::remove_if(first_token.Text.begin(),
				                                      first_token.Text.end(),
				                                      [](const char c) { return c == '"'; }),
				                       first_token.Text.end());

				Color = ZColor::FromString(first_token.Text);
			}
			break;

			case ETokenType::ZIdentifier:
			{
				static constexpr auto RGB_HASH  = Beard::Crc32::Hash("rgb");
				static constexpr auto RGBA_HASH = Beard::Crc32::Hash("rgba");
				static constexpr auto HSL_HASH  = Beard::Crc32::Hash("hsl");
				static constexpr auto HSLA_HASH = Beard::Crc32::Hash("hsla");

				const auto hash = Beard::Crc32::Hash(first_token.Text);

				Beard::Array<f32> values;
				for (const auto& t : Tokens)
				{
					if (t.Type == ETokenType::Number)
					{
						values.Add(t.Number);
					}
				}

				switch (hash)
				{
					case RGB_HASH:
					{
						if (values.ElementCount() == 3)
						{
							Color = ZColor::FromRGBA((i32)values[0], (i32)values[1], (i32)values[2]);
						}
						else
						{
							LOG_F(ERROR,
							      "Line %d: Malformed rgb() call, should be "
							      "rgb(red, green, blue)",
							      first_token.Line);
						}
					}
					break;

					case RGBA_HASH:
					{
						if (values.ElementCount() == 4)
						{
							Color = ZColor::FromRGBA((i32)values[0], (i32)values[1], (i32)values[2], values[3]);
						}
						else
						{
							LOG_F(ERROR,
							      "Line %d: Malformed rgba() call, should be "
							      "rgba(red, green, blue, alpha)",
							      first_token.Line);
						}
					}
					break;

					case HSL_HASH:
					{
						if (values.ElementCount() == 3)
						{
							Color = ZColor::FromHSLA((i32)values[0], (i32)values[1], (i32)values[2]);
						}
						else
						{
							LOG_F(ERROR,
							      "Line %d: Malformed hsl() call, should be "
							      "hsl(hue, saturation, lightness)",
							      first_token.Line);
						}
					}
					break;

					case HSLA_HASH:
					{
						if (values.ElementCount() == 4)
						{
							Color = ZColor::FromHSLA((i32)values[0], (i32)values[1], (i32)values[2], values[3]);
						}
						else
						{
							LOG_F(ERROR,
							      "Line %d: Malformed hsla() call, should be "
							      "hsla(hue, saturation, lightness, alpha",
							      first_token.Line);
						}
					}
					break;
					default:
					{
						if (first_token.Text.compare("Color") == 0)
						{
							ASSERT(Tokens.ElementCount() == 3 && Tokens[2].Type == ETokenType::ZIdentifier,
							       "We are looking for `Color.ColorName`");
							if (auto it = ZColor::s_ColorsByName.Find(Tokens[2].Text);
							    it != ZColor::s_ColorsByName.end())
							{
								Color = it->second;
							}
							else
							{
								LOG_F(ERROR,
								      "ling %d: Color '%s' is not a valid "
								      "Color Name. See "
								      "https://www.w3schools.com/colors/"
								      "colors_names.asp for a complete list.",
								      first_token.Line,
								      Tokens[2].Text.c_str());
							}
						}
						else
						{
							LOG_F(ERROR,
							      "Line %d: Could not parse Color given '%s' "
							      "value",
							      first_token.Line,
							      first_token.Text.c_str());
						}
					}
					break;
				}
			}
		}
	}

	return Color;
}

}

Beard::Array<ZWidget*> ZWidget::s_WidgetMap = {};

extern Beard::StringHashMap<ZWidgetFactory*> s_WidgetFactories;

ZWidget::~ZWidget()
{
	s_WidgetMap.Remove(std::find(s_WidgetMap.begin(), s_WidgetMap.end(), this));

	for (auto&& c : Children)
	{
		delete c;
	}
	Children.Clear();
}

void ZWidget::deserialize(Parser::ZNode::Ptr Node)
{
	s_WidgetMap.Add(this);

	for (auto c : Node->Children)
	{
		if (c->Type == Parser::ENodeType::Structure)
		{
			auto child = (*s_WidgetFactories[c->Name])();

			Children.Add(child);

			child->Parent = this;

			child->deserialize(c);
		}
		else if (c->Type == Parser::ENodeType::Property)
		{
			ParseProperty(c);
		}
	}
}

void ZWidget::BuildRenderInfos(Beard::Array<RectangleInfo>* Result)
{
	BuildRenderInfosInternal(Result);
	for (auto c : Children)
	{
		c->BuildRenderInfos(Result);
	}
}

void ZWidget::Evaluate()
{
	for (auto&& w : s_WidgetMap)
	{
		w->PreEvaluate();
	}

	for (auto&& w : s_WidgetMap)
	{
		w->EvaluateInternal();
	}

	for (auto&& w : s_WidgetMap)
	{
		w->PostEvaluate();
	}
}

void ZWidget::EvaluateInternal()
{
	if (bDirty)
	{
		for (auto d : Dependencies)
		{
			d->EvaluateInternal();
		}

		for (auto&& eval : Evaluators)
		{
			*eval.first = eval.second.Evaluate();
		}

		bDirty = false;
	}
}

void ZWidget::touch()
{
	bDirty = true;
	for (auto d : Dependants)
	{
		d->touch();
	}
}

bool ZWidget::WindowResized(i32 w, i32 h)
{
	bool needUpdate = false;

	for (auto c : Children)
	{
		needUpdate |= c->WindowResized(w, h);
	}

	return needUpdate;
}

void ZWidget::ParseProperty(Parser::ZNode::Ptr Node)
{
	const auto node_hash = Beard::Crc32::Hash(Node->Name);
	switch (node_hash)
	{
		case static_cast<u32>(ENodeHash::ID):
		{
			ASSERT(Node->Children.ElementCount() == 1, "ID: <ID>");
			ID = Node->Children[0]->Name;
		}
		break;

		case static_cast<u32>(ENodeHash::X):
		case static_cast<u32>(ENodeHash::Y):
		case static_cast<u32>(ENodeHash::Width):
		case static_cast<u32>(ENodeHash::Height):
		{
			auto infos = Node->Children[0];

			GeometryExpressions[node_hash] = infos->AssociatedTokens;

			for (i32 i = 0; i < infos->AssociatedTokens.ElementCount() - 1; ++i)
			{
				if (infos->AssociatedTokens[i].Type == ETokenType::ZIdentifier &&
				    infos->AssociatedTokens[i + 1].Type == ETokenType::Dot)
				{
					DependencyIDs.Add(infos->AssociatedTokens[i].Text);
				}
			}
		}
		break;

		case static_cast<u32>(ENodeHash::Anchors):
		{
			LOG_F(WARNING, "Anchors not handled yet");
		}
		break;

		case static_cast<u32>(ENodeHash::Padding):
		{
			LOG_F(WARNING, "Padding not handled yet");
		}
		break;

		case static_cast<u32>(ENodeHash::Margins):
		{
			LOG_F(WARNING, "Margins not handled yet");
		}
		break;

		default:
		{
			ParserPropertyInternal(Node, node_hash);
		}
		break;
	}
}

ZWidget* GetWidgetByID(ZWidget* RootWidget, const std::string& Name)
{
	if (RootWidget->ID == Name)
	{
		return RootWidget;
	}

	for (auto child : RootWidget->Children)
	{
		if (auto Node = GetWidgetByID(child, Name); Node != nullptr)
		{
			return Node;
		}
	}

	return nullptr;
}

void BuildDependencyGraph(ZWidget* RootWidget, ZWidget* current_widget)
{
	for (auto ID : current_widget->DependencyIDs)
	{
		ZWidget* dep = ID == "Parent" ? current_widget->Parent : GetWidgetByID(RootWidget, ID);

		if (dep != nullptr)
		{
			current_widget->Dependencies.Add(dep);
			dep->Dependants.Add(current_widget);
		}
	}

	for (auto c : current_widget->Children)
	{
		BuildDependencyGraph(RootWidget, c);
	}
}

void BuildExpressionEvaluators(ZWidget* RootWidget, ZWidget* current_widget)
{
	auto get_hash_index = [](u32 hash)
	{
		switch (hash)
		{
			case static_cast<u32>(ENodeHash::X):
				return 0;
			case static_cast<u32>(ENodeHash::Y):
				return 1;
			case static_cast<u32>(ENodeHash::Width):
				return 2;
			case static_cast<u32>(ENodeHash::Height):
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
			case static_cast<u32>(ENodeHash::X):
				return &current_widget->Pos.x;
			case static_cast<u32>(ENodeHash::Y):
				return &current_widget->Pos.y;
			case static_cast<u32>(ENodeHash::Width):
				return &current_widget->Size.x;
			case static_cast<u32>(ENodeHash::Height):
				return &current_widget->Size.y;

			default:
				ASSERT_UNREACHABLE();
				return nullptr;
		}
	};

	Beard::HashSet<u32> remaining_attributes = {
	    static_cast<u32>(ENodeHash::X),
	    static_cast<u32>(ENodeHash::Y),
	    static_cast<u32>(ENodeHash::Width),
	    static_cast<u32>(ENodeHash::Height),
	};
	for (auto expr : current_widget->GeometryExpressions)
	{
		remaining_attributes.Remove((u32)expr.first);

		auto expression = ShuntingYard::ZExpression::build(expr.second, RootWidget, current_widget);

		current_widget->Evaluators.Add(std::make_pair(get_property_ptr(expr.first), expression));
	}

	for (auto c : current_widget->Children)
	{
		BuildExpressionEvaluators(RootWidget, c);
	}
}
