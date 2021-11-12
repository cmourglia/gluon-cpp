#include <Gluon/Widgets/window.h>

#include <Gluon/Widgets/hashes.h>

#include <Gluon/App/app.h>

namespace Utils
{
glm::vec4 ExtractColor(const beard::array<ZToken>& Tokens);
}

ZWindow::ZWindow()
{
	Size = GluonApp::Get()->GetWindowSize();
}

void ZWindow::ParserPropertyInternal(Parser::ZNode::Ptr Node, u32 node_hash)
{
	UNUSED(Node);

	switch (node_hash)
	{
		case static_cast<u32>(ENodeHash::Title):
		{
			GluonApp::Get()->SetTitle(Node->Children[0]->Name.c_str());
		}
		break;

		case static_cast<u32>(ENodeHash::WindowWidth):
		{
			Size.x = Node->Children[0]->AssociatedTokens[0].Number;
		}
		break;

		case static_cast<u32>(ENodeHash::WindowHeight):
		{
			Size.y = Node->Children[0]->AssociatedTokens[0].Number;
		}
		break;

		case static_cast<u32>(ENodeHash::Color):
		{
			glm::vec4 Color = Utils::ExtractColor(Node->Children[0]->AssociatedTokens);
			GluonApp::Get()->SetBackgroundColor(Color);
		}
		break;

		default:
			ASSERT_UNREACHABLE();
			break;
	}
}

void ZWindow::PreEvaluate()
{
    GeometryExpressions.clear();
    Evaluators.clear();
}

void ZWindow::PostEvaluate()
{
	GluonApp::Get()->SetWindowSize(static_cast<i32>(Size.x), static_cast<i32>(Size.y));
	// GluonApp::Get()->SetWindowPos((i32)Pos.x, (i32)Pos.y);
}

bool ZWindow::WindowResized(i32 w, i32 h)
{
	bool resized = false;
	if (static_cast<i32>(Size.x) != w || static_cast<i32>(Size.y) != h)
	{
		Size.x = static_cast<f32>(w);
		Size.y = static_cast<f32>(h);
		touch();
		resized = true;
	}

	resized |= ZWidget::WindowResized(w, h);

	return resized;
}

void ZWindow::BuildRenderInfosInternal(beard::array<RectangleInfo>* Result)
{
	UNUSED(Result);
	// Passthrough
}
