#include "widgets/window.h"

#include "widgets/hashes.h"

#include "gluon_app.h"

namespace Utils
{
glm::vec4 ExtractColor(const std::vector<Token>& tokens);
}

GluonWindow::GluonWindow()
    : GluonWidget()
{
	size = GluonApp::Get()->GetWindowSize();
}

void GluonWindow::ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash)
{
	switch (nodeHash)
	{
		case NodeHash::Title:
		{
			GluonApp::Get()->SetTitle(node->children[0]->name.c_str());
		}
		break;

		case NodeHash::WindowWidth:
		{
			size.x = node->children[0]->associatedTokens[0].number;
		}
		break;

		case NodeHash::WindowHeight:
		{
			size.y = node->children[0]->associatedTokens[0].number;
		}
		break;

		case NodeHash::Color:
		{
			glm::vec4 color = Utils::ExtractColor(node->children[0]->associatedTokens);
			GluonApp::Get()->SetBackgroundColor(color);
		}
	}
}

void GluonWindow::PreEvaluate()
{
	geometryExpressions.clear();
	evaluators.clear();
}

void GluonWindow::PostEvaluate()
{
	GluonApp::Get()->SetWindowSize((i32)size.x, (i32)size.y);
	// GluonApp::Get()->SetWindowPos((i32)pos.x, (i32)pos.y);
}

bool GluonWindow::WindowResized(i32 w, i32 h)
{
	bool resized = false;
	if ((i32)size.x != w || (i32)size.y != h)
	{
		size.x = w;
		size.y = h;
		Touch();
		resized = true;
	}

	resized |= GluonWidget::WindowResized(w, h);

	return resized;
}

void GluonWindow::BuildRenderInfosInternal(std::vector<RectangleInfo>* result)
{
	// Passthrough
}
