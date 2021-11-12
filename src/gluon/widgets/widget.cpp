#include <Gluon/Widgets/widget.h>

#include <Gluon/Widgets/hashes.h>

#include <beard/containers/hash_map.h>
#include <beard/misc/hash.h>

#include <loguru.hpp>

#include <filesystem>

namespace utils
{
glm::vec4 ExtractColor(const beard::array<Token>& tokens)
{
    glm::vec4 color = {};

    if (!tokens.is_empty())
    {
        Token first_token = tokens.first();
        switch (first_token.token_type)
        {
            case TokenType::kString:
            {
                ASSERT(tokens.element_count() == 1, "A Color as a string can only be defined by one token");
                first_token.text.erase(std::remove_if(first_token.text.begin(),
                                                      first_token.text.end(),
                                                      [](const char c) { return c == '"'; }),
                                       first_token.text.end());

                color = color::FromString(first_token.text);
            }
            break;

            case TokenType::kIdentifier:
            {
                static constexpr auto kRgbHash  = beard::crc32::hash("rgb");
                static constexpr auto kRgbaHash = beard::crc32::hash("rgba");
                static constexpr auto kHslHash  = beard::crc32::hash("hsl");
                static constexpr auto kHslaHash = beard::crc32::hash("hsla");

                const auto hash = beard::crc32::hash(first_token.text);

                beard::array<f32> values;
                for (const auto& t : tokens)
                {
                    if (t.token_type == TokenType::kNumber)
                    {
                        values.add(t.number);
                    }
                }

                switch (hash)
                {
                    case kRgbHash:
                    {
                        if (values.element_count() == 3)
                        {
                            color = color::FromRGBA((i32)values[0], (i32)values[1], (i32)values[2]);
                        }
                        else
                        {
                            LOG_F(ERROR,
                                  "Line %d: Malformed rgb() call, should be "
                                  "rgb(red, green, blue)",
                                  first_token.line);
                        }
                    }
                    break;

                    case kRgbaHash:
                    {
                        if (values.element_count() == 4)
                        {
                            color = color::FromRGBA((i32)values[0], (i32)values[1], (i32)values[2], values[3]);
                        }
                        else
                        {
                            LOG_F(ERROR,
                                  "Line %d: Malformed rgba() call, should be "
                                  "rgba(red, green, blue, alpha)",
                                  first_token.line);
                        }
                    }
                    break;

                    case kHslHash:
                    {
                        if (values.element_count() == 3)
                        {
                            color = color::FromHSLA((i32)values[0], (i32)values[1], (i32)values[2]);
                        }
                        else
                        {
                            LOG_F(ERROR,
                                  "Line %d: Malformed hsl() call, should be "
                                  "hsl(hue, saturation, lightness)",
                                  first_token.line);
                        }
                    }
                    break;

                    case kHslaHash:
                    {
                        if (values.element_count() == 4)
                        {
                            color = color::FromHSLA((i32)values[0], (i32)values[1], (i32)values[2], values[3]);
                        }
                        else
                        {
                            LOG_F(ERROR,
                                  "Line %d: Malformed hsla() call, should be "
                                  "hsla(hue, saturation, lightness, alpha",
                                  first_token.line);
                        }
                    }
                    break;
                    default:
                    {
                        if (first_token.text.compare("Color") == 0)
                        {
                            ASSERT(tokens.element_count() == 3 && tokens[2].token_type == TokenType::kIdentifier,
                                   "We are looking for `Color.ColorName`");
                            if (auto it = color::kColorsByName.find(tokens[2].text); it != color::kColorsByName.end())
                            {
                                color = it->second;
                            }
                            else
                            {
                                LOG_F(ERROR,
                                      "ling %d: Color '%s' is not a valid "
                                      "Color Name. See "
                                      "https://www.w3schools.com/colors/"
                                      "colors_names.asp for a complete list.",
                                      first_token.line,
                                      tokens[2].text.c_str());
                            }
                        }
                        else
                        {
                            LOG_F(ERROR,
                                  "Line %d: Could not parse Color given '%s' "
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

beard::array<Widget*> Widget::s_widget_map = {};

extern beard::string_hash_map<WidgetFactory*> s_widget_factories;

Widget::~Widget()
{
    s_widget_map.remove(std::find(s_widget_map.begin(), s_widget_map.end(), this));

    for (auto&& c : children)
    {
        delete c;
    }
    children.clear();
}

void Widget::deserialize(parser::Node::Ptr node)
{
    s_widget_map.add(this);

    for (auto c : node->children)
    {
        if (c->node_type == parser::NodeType::kStructure)
        {
            auto child = (*s_widget_factories[c->name])();

            children.add(child);

            child->parent = this;

            child->deserialize(c);
        }
        else if (c->node_type == parser::NodeType::kProperty)
        {
            ParseProperty(c);
        }
    }
}

void Widget::BuildRenderInfos(beard::array<RectangleInfo>* result)
{
    BuildRenderInfosInternal(result);
    for (auto c : children)
    {
        c->BuildRenderInfos(result);
    }
}

void Widget::Evaluate()
{
    for (auto&& w : s_widget_map)
    {
        w->PreEvaluate();
    }

    for (auto&& w : s_widget_map)
    {
        w->EvaluateInternal();
    }

    for (auto&& w : s_widget_map)
    {
        w->PostEvaluate();
    }
}

void Widget::EvaluateInternal()
{
    if (is_dirty)
    {
        for (auto d : dependencies)
        {
            d->EvaluateInternal();
        }

        for (auto&& eval : evaluators)
        {
            *eval.first = eval.second.Evaluate();
        }

        is_dirty = false;
    }
}

void Widget::touch()
{
    is_dirty = true;
    for (auto d : dependees)
    {
        d->touch();
    }
}

bool Widget::WindowResized(i32 new_width, i32 new_height)
{
    bool needUpdate = false;

    for (auto c : children)
    {
        needUpdate |= c->WindowResized(new_width, new_height);
    }

    return needUpdate;
}

void Widget::ParseProperty(parser::Node::Ptr node)
{
    const auto node_hash = beard::crc32::hash(node->name);
    switch (node_hash)
    {
        case static_cast<u32>(NodeHash::kId):
        {
            ASSERT(node->children.element_count() == 1, "ID: <ID>");
            id = node->children[0]->name;
        }
        break;

        case static_cast<u32>(NodeHash::kX):
        case static_cast<u32>(NodeHash::kY):
        case static_cast<u32>(NodeHash::kWidth):
        case static_cast<u32>(NodeHash::kHeight):
        {
            auto infos = node->children[0];

            geometry_expressions[node_hash] = infos->associated_tokens;

            for (i32 i = 0; i < infos->associated_tokens.element_count() - 1; ++i)
            {
                if (infos->associated_tokens[i].token_type == TokenType::kIdentifier &&
                    infos->associated_tokens[i + 1].token_type == TokenType::kDot)
                {
                    dependency_ids.add(infos->associated_tokens[i].text);
                }
            }
        }
        break;

        case static_cast<u32>(NodeHash::kAnchors):
        {
            LOG_F(WARNING, "Anchors not handled yet");
        }
        break;

        case static_cast<u32>(NodeHash::kPadding):
        {
            LOG_F(WARNING, "Padding not handled yet");
        }
        break;

        case static_cast<u32>(NodeHash::kMargins):
        {
            LOG_F(WARNING, "Margins not handled yet");
        }
        break;

        default:
        {
            ParserPropertyInternal(node, node_hash);
        }
        break;
    }
}

Widget* GetWidgetById(Widget* root_widget, const std::string& name)
{
    if (root_widget->id == name)
    {
        return root_widget;
    }

    for (auto child : root_widget->children)
    {
        if (auto Node = GetWidgetById(child, name); Node != nullptr)
        {
            return Node;
        }
    }

    return nullptr;
}

void BuildDependencyGraph(Widget* root_widget, Widget* current_widget)
{
    for (auto ID : current_widget->dependency_ids)
    {
        Widget* dep = ID == "Parent" ? current_widget->parent : GetWidgetById(root_widget, ID);

        if (dep != nullptr)
        {
            current_widget->dependencies.add(dep);
            dep->dependees.add(current_widget);
        }
    }

    for (auto c : current_widget->children)
    {
        BuildDependencyGraph(root_widget, c);
    }
}

void BuildExpressionEvaluators(Widget* root_widget, Widget* current_widget)
{
    auto get_hash_index = [](u32 hash)
    {
        switch (hash)
        {
            case static_cast<u32>(NodeHash::kX):
                return 0;
            case static_cast<u32>(NodeHash::kY):
                return 1;
            case static_cast<u32>(NodeHash::kWidth):
                return 2;
            case static_cast<u32>(NodeHash::kHeight):
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
            case static_cast<u32>(NodeHash::kX):
                return &current_widget->pos.x;
            case static_cast<u32>(NodeHash::kY):
                return &current_widget->pos.y;
            case static_cast<u32>(NodeHash::kWidth):
                return &current_widget->size.x;
            case static_cast<u32>(NodeHash::kHeight):
                return &current_widget->size.y;

            default:
                ASSERT_UNREACHABLE();
                return nullptr;
        }
    };

    beard::hash_set<u32> remaining_attributes = {
        static_cast<u32>(NodeHash::kX),
        static_cast<u32>(NodeHash::kY),
        static_cast<u32>(NodeHash::kWidth),
        static_cast<u32>(NodeHash::kHeight),
    };
    for (auto expr : current_widget->geometry_expressions)
    {
        remaining_attributes.remove((u32)expr.first);

        auto expression = shunting_yard::Expression::Build(expr.second, root_widget, current_widget);

        current_widget->evaluators.add(std::make_pair(get_property_ptr(expr.first), expression));
    }

    for (auto c : current_widget->children)
    {
        BuildExpressionEvaluators(root_widget, c);
    }
}
