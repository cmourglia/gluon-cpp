#include <Gluon/Compiler/parser.h>

#include <Gluon/Compiler/tokenizer.h>

#include <Gluon/Widgets/image.h>
#include <Gluon/Widgets/rectangle.h>
#include <Gluon/Widgets/text.h>
#include <Gluon/Widgets/widget.h>
#include <Gluon/Widgets/window.h>

#include <beard/containers/hash_map.h>

#include <loguru.hpp>

#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <stack>
#include <unordered_set>

using namespace parser;

enum class ParserState {
  kInitial,

  kStructure,
  kProperty,
  kValue,
};

enum class GeometryPolicy {
  kAbsolute,
  kRelative,
  kAnchor,
};

beard::string_hash_map<WidgetFactory*> s_widget_factories;

Widget* ParseGluonBuffer(const char* buffer) {
  if (s_widget_factories.is_empty()) {
    // FIXME: How / when do we wanna build this map ?
    s_widget_factories["Window"] = &Window::create;
    s_widget_factories["Rectangle"] = &Rectangle::create;
    s_widget_factories["Image"] = &Image::create;
    s_widget_factories["Text"] = &ZText::create;
  }

  auto Tokens = Tokenize(buffer);

  Node::Ptr root;
  Node* current_node = nullptr;

  auto remove_it =
      std::remove_if(Tokens.begin(), Tokens.end(), [](const Token& token) {
        return token.token_type == TokenType::kSpacing;
      });
  Tokens.remove_range(remove_it, Tokens.end());

  Token* token = Tokens.data();

  ParserState state = ParserState::kInitial;

  bool done = false;
  while (!done) {
    if (state == ParserState::kProperty) {
      beard::array<Token> value_tokens;

      std::string name;

      if (token->token_type == TokenType::kColon) {
        ++token;
      }

      while (!(token->token_type == TokenType::kEOL ||
               token->token_type == TokenType::kSemicolon)) {
        value_tokens.add(*token);
        name += token->text;

        ++token;
      }

      auto node = std::make_shared<ValueNode>();
      node->name = name;
      node->parent = current_node;
      node->associated_tokens = value_tokens;
      current_node->children.add(node);

      current_node = current_node->parent;
      state = ParserState::kStructure;
    } else {
      switch (token->token_type) {
        case TokenType::kEOF:
          done = true;
          break;

        case TokenType::kIdentifier: {
          if (state == ParserState::kInitial) {
            // We MUST start with a structure Node
            auto node = std::make_shared<StructureNode>();
            node->name = token->text;
            node->parent = nullptr;
            node->associated_tokens.add(*token);

            root = node;
            current_node = root.get();

            state = ParserState::kStructure;
          } else if (state == ParserState::kStructure) {
            // Either we have a property or a child structure
            if (token[1].token_type == TokenType::kOpenBrace) {
              auto node = std::make_shared<StructureNode>();
              node->name = token->text;
              node->parent = current_node;
              current_node->children.add(node);
              current_node = node.get();

              state = ParserState::kStructure;
            } else if (token[1].token_type == TokenType::kColon) {
              auto node = std::make_shared<PropertyNode>();
              node->name = token->text;
              node->parent = current_node;
              current_node->children.add(node);
              current_node = node.get();

              state = ParserState::kProperty;
            }
            // FIXME(Charly): Maybe we should try to find a way to
            // avoid code duplication ?
            else if (token[1].token_type == TokenType::kDot) {
              auto node = std::make_shared<PropertyNode>();
              node->name = token->text;
              node->parent = current_node;

              // FIXME(Charly): This loop is not safe
              for (int i = 2; token[i].token_type != TokenType::kColon; ++i) {
                if (token[i].token_type == TokenType::kIdentifier) {
                  node->associated_tokens.add(token[i]);
                }
              }

              current_node->children.add(node);
              current_node = node.get();

              state = ParserState::kProperty;
            } else {
              LOG_F(ERROR, "Invalid identifier token %s (%d %d)",
                    token->text.c_str(), token->line, token->column);
            }
          }
        } break;

        case TokenType::kCloseBrace: {
          current_node = current_node->parent;
        }

        default:
          break;
      }
    }

    ++token;
  }

  beard::array<RectangleInfo> result;

  Widget* root_widget = nullptr;

  if (root) {
    root_widget = (*s_widget_factories[root->name])();
    root_widget->deserialize(root);

    BuildDependencyGraph(root_widget, root_widget);
    BuildExpressionEvaluators(root_widget, root_widget);
  } else {
    LOG_F(ERROR, "Something wrong happened");
  }

  return root_widget;
}
