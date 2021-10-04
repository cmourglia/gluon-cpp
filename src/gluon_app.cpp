#include "gluon_app.h"

#include "utils.h"
#include "compiler/parser.h"

#include <raylib.h>
#include <nanosvg.h>
#include <nanovg.h>

#include <loguru.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <unordered_set>
#include <filesystem>
#include <chrono>

#include <assert.h>

GluonApp* GluonApp::s_instance = nullptr;

struct FrameInfos
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec2 viewport;
};

std::vector<glm::vec4> defaultColors = {
    {1.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f, 1.0f},
    {0.0f, 1.0f, 1.0f, 1.0f},
};

GluonApp* GluonApp::Get()
{
	return s_instance;
}

GluonApp::GluonApp(int argc, char** argv)
{
	assert(s_instance == nullptr);
	s_instance = this;

	// TODO: Extract window infos if set
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1024, 768, "Gluon Muon Whatever");

	SetTargetFPS(-1);
}

GluonApp::~GluonApp()
{
	CloseWindow();
}

int GluonApp::Run()
{
	u32 currentFrame = 0;

	f64 t0         = GetTime();
	u32 frameCount = 0;

	glm::mat4 proj;
	glm::mat4 view = glm::mat4(1.0f); // Identity

	i64 lastWriteTime = 0;

	f64 avgFps       = 0.0;
	f64 avgFrameTime = 0.0;

	auto GetColor = [](const glm::vec4& color) -> Color {
		return Color{(u8)(color.r * 255), (u8)(color.g * 255), (u8)(color.b * 255), (u8)(color.a * 255)};
	};

	SetTargetFPS(120);

	while (!WindowShouldClose())
	{
		// Check if gluon file update is needed
		i64 writeTime;

		std::string fileContent;
		if (FileUtils::ReadFileIfNewer("test.gluon", lastWriteTime, &writeTime, &fileContent))
		{
			rectangles    = ParseGluonBuffer(fileContent);
			lastWriteTime = writeTime;
		}

		ClearBackground(RAYWHITE);

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
							if (shape->flags & NSVG_FLAGS_VISIBLE == 0)
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
						Texture2D* texture = (Texture2D*)rect.imageInfo->rasterImage->data;
						DrawTexture(*texture,
						            r.x + rect.imageInfo->rasterImage->offsetX,
						            r.y + rect.imageInfo->rasterImage->offsetY,
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
