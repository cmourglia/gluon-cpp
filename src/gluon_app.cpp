#include "gluon_app.h"

#include "utils.h"
#include "compiler/parser.h"
#include "widgets/widget.h"

#include <raylib.h>
#include <raymath.h>
#include <nanosvg.h>

#include <vector>

#include <assert.h>

GluonApp* GluonApp::s_instance = nullptr;

GluonApp* GluonApp::Get()
{
	return s_instance;
}

GluonApp::GluonApp(int argc, char** argv)
    : backgroundColor(0.8f, 0.8f, 0.8f, 1.0f)
{
	// TODO: Do stuff with argc, argv;
	(void)argc;
	(void)argv;

	assert(s_instance == nullptr);
	s_instance = this;

	// TODO: Extract window infos if set
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(1024, 768, "Gluon Muon Whatever");

	SetTargetFPS(-1);
}

GluonApp::~GluonApp()
{
	CloseWindow();
}

int GluonApp::Run()
{
	i64 lastWriteTime = 0;

	auto GetColor = [](const glm::vec4& color) -> Color {
		return Color{(u8)(color.r * 255), (u8)(color.g * 255), (u8)(color.b * 255), (u8)(color.a * 255)};
	};

	SetTargetFPS(120);

	GluonWidget* rootWidget = nullptr;

	while (!WindowShouldClose())
	{
		// Check if gluon file update is needed
		i64 writeTime;

		std::string fileContent;

		bool drawNeedsUpdate = false;

		if (FileUtils::ReadFileIfNewer("test.gluon", lastWriteTime, &writeTime, &fileContent))
		{
			delete rootWidget;
			rootWidget      = ParseGluonBuffer(fileContent);
			drawNeedsUpdate = true;

			lastWriteTime = writeTime;
		}

		if (rootWidget != nullptr)
		{
			if (IsWindowResized())
			{
				i32 w = GetScreenWidth();
				i32 h = GetScreenHeight();
				drawNeedsUpdate |= rootWidget->WindowResized(w, h);
			}

			Vector2 mouseDelta = GetMouseDelta();
			if (Vector2LengthSqr(mouseDelta) > 0.0f)
			{
				Vector2 mousePos = GetMousePosition();

				drawNeedsUpdate |= rootWidget->MouseMoved({mousePos.x, mousePos.y});
			}

			if (drawNeedsUpdate)
			{
				rectangles.clear();
				GluonWidget::Evaluate();
				rootWidget->BuildRenderInfos(&rectangles);
			}
		}

		ClearBackground(GetColor(backgroundColor));

		BeginDrawing();
		{
			for (const auto& rect : rectangles)
			{
				Rectangle r         = {rect.position.x, rect.position.y, rect.size.x, rect.size.y};
				f32       smallSide = Min(r.width, r.height);
				f32       roundness = Min(rect.radius, smallSide) / smallSide;

				if (rect.isImage)
				{
					if (rect.imageInfo->isVectorial)
					{
						NSVGshape* shape = rect.imageInfo->svgImage->shapes;
						while (shape != nullptr)
						{
							if ((shape->flags & NSVG_FLAGS_VISIBLE) == 0)
							{
								continue;
							}

							Color color = *(Color*)&shape->stroke.color;
							color.a     = u8(shape->opacity * 255);

							NSVGpath* path = shape->paths;
							while (path != nullptr)
							{
								for (int i = 0; i < path->npts - 1; i += 3)
								{
									float* p = &path->pts[i * 2];
									DrawLineBezierCubic(Vector2{p[0] + r.x, p[1] + r.y},
									                    Vector2{p[6] + r.x, p[7] + r.y},
									                    Vector2{p[2] + r.x, p[3] + r.y},
									                    Vector2{p[4] + r.x, p[5] + r.y},
									                    shape->strokeWidth,
									                    color);
								}

								path = path->next;
							}

							shape = shape->next;
						}
					}
					else
					{
						Texture2D* texture = rect.imageInfo->rasterImage->texture;
						DrawTexture(*texture,
						            (int)(r.x + rect.imageInfo->rasterImage->offsetX),
						            (int)(r.y + rect.imageInfo->rasterImage->offsetY),
						            GetColor(rect.fillColor));
						DrawRectangleRoundedLines(r, 0, 1, 5, BLACK);
					}
				}
				else
				{
					DrawRectangleRounded(r, roundness, 32, GetColor(rect.fillColor));

					if (rect.borderWidth > 0.0f)
					{
						DrawRectangleRoundedLines(r, roundness, 32, rect.borderWidth, GetColor(rect.borderColor));
					}
				}
			}

			DrawFPS(10, 10);
		}
		EndDrawing();
	}

	return 0;
}

void GluonApp::SetTitle(const char* title)
{
	SetWindowTitle(title);
}

void GluonApp::SetWindowSize(i32 w, i32 h)
{
	::SetWindowSize(w, h);
	width  = w;
	height = h;
}
