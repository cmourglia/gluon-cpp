#include <Gluon/Widgets/window.h>

#include <Gluon/Widgets/hashes.h>

#include <Gluon/App/app.h>

namespace utils {
glm::vec4 ExtractColor(const beard::array<Token>& tokens);
}

Window::Window() {
  size = GluonApp::instance()->GetWindowSize();
}

void Window::ParserPropertyInternal(parser::Node::Ptr node, u32 node_hash) {
  UNUSED(node);

  switch (node_hash) {
    case static_cast<u32>(NodeHash::kTitle): {
      GluonApp::instance()->SetTitle(node->children[0]->name.c_str());
    } break;

    case static_cast<u32>(NodeHash::kWindowWidth): {
      size.x = node->children[0]->associated_tokens[0].number;
    } break;

    case static_cast<u32>(NodeHash::kWindowHeight): {
      size.y = node->children[0]->associated_tokens[0].number;
    } break;

    case static_cast<u32>(NodeHash::kColor): {
      glm::vec4 Color =
          utils::ExtractColor(node->children[0]->associated_tokens);
      GluonApp::instance()->SetBackgroundColor(Color);
    } break;

    default:
      ASSERT_UNREACHABLE();
      break;
  }
}

void Window::PreEvaluate() {
  geometry_expressions.clear();
  evaluators.clear();
}

void Window::PostEvaluate() {
  GluonApp::instance()->SetWindowSize(static_cast<i32>(size.x),
                                      static_cast<i32>(size.y));
  // GluonApp::Get()->SetWindowPos((i32)Pos.x, (i32)Pos.y);
}

bool Window::WindowResized(i32 w, i32 h) {
  bool resized = false;
  if (static_cast<i32>(size.x) != w || static_cast<i32>(size.y) != h) {
    size.x = static_cast<f32>(w);
    size.y = static_cast<f32>(h);
    touch();
    resized = true;
  }

  resized |= Widget::WindowResized(w, h);

  return resized;
}

void Window::BuildRenderInfosInternal(beard::array<RectangleInfo>* result) {
  UNUSED(result);
  // Passthrough
}
