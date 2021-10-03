#include "gluon_app.h"

#include "utils.h"
#include "compiler/parser.h"

#include <raylib.h>

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
				DrawRectangleRounded(r, roundness, 32, GetColor(rect.fillColor));

				if (rect.borderWidth > 0.0f)
				{
					DrawRectangleRoundedLines(r, roundness, 32, rect.borderWidth, GetColor(rect.borderColor));
				}
			}

			DrawFPS(10, 10);
		}
		EndDrawing();
	}

	return 0;
}
