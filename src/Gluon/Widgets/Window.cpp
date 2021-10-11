#include <Gluon/Widgets/Window.h>

#include <Gluon/Widgets/Hashes.h>

#include <Gluon/App/GluonApp.h>

namespace Utils
{
glm::vec4 extract_color(const std::vector<Token>& tokens);
}

GluonWindow::GluonWindow() { size = GluonApp::Get()->get_window_size(); }

void GluonWindow::parse_property_internal(Parser::Node::Ptr node, u32 node_hash)
{
	UNUSED(node);

	switch (node_hash)
	{
		case NodeHash::Title:
		{
			GluonApp::Get()->set_title(node->children[0]->name.c_str());
		}
		break;

		case NodeHash::WindowWidth:
		{
			size.x = node->children[0]->associated_tokens[0].number;
		}
		break;

		case NodeHash::WindowHeight:
		{
			size.y = node->children[0]->associated_tokens[0].number;
		}
		break;

		case NodeHash::Color:
		{
			glm::vec4 color = Utils::extract_color(
			    node->children[0]->associated_tokens);
			GluonApp::Get()->set_background_color(color);
		}
		break;

		default:
			ASSERT_UNREACHABLE();
			break;
	}
}

void GluonWindow::pre_evaluate()
{
	geometry_expressions.clear();
	evaluators.clear();
}

void GluonWindow::post_evaluate()
{
	GluonApp::Get()->set_window_size(static_cast<i32>(size.x),
	                                 static_cast<i32>(size.y));
	// GluonApp::Get()->SetWindowPos((i32)pos.x, (i32)pos.y);
}

bool GluonWindow::window_resized(i32 w, i32 h)
{
	bool resized = false;
	if (static_cast<i32>(size.x) != w || static_cast<i32>(size.y) != h)
	{
		size.x = static_cast<f32>(w);
		size.y = static_cast<f32>(h);
		touch();
		resized = true;
	}

	resized |= GluonWidget::window_resized(w, h);

	return resized;
}

void GluonWindow::build_render_infos_internal(
    std::vector<RectangleInfo>* result)
{
	UNUSED(result);
	// Passthrough
}
