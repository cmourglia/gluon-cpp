#pragma once

#include <gluon/core/types.h>

#include <beard/containers/array.h>

class GluonApp
{
public:
	static GluonApp* Get();

	GluonApp(const GluonApp&) = delete;
	GluonApp(GluonApp&&)      = delete;
	void operator=(const GluonApp&) = delete;
	void operator=(GluonApp&&) = delete;

	explicit GluonApp(int argc, char** argv);
	~GluonApp();

	int Run();

	void SetTitle(const char* Title);
	void SetWindowSize(i32 Width, i32 Height);
	void SetBackgroundColor(const glm::vec4& Color)
	{
		BackgroundColor = Color;
	}

	[[nodiscard]] glm::vec2 GetWindowSize() const
	{
		return glm::vec2{static_cast<f32>(Width), static_cast<f32>(Height)};
	}

private:
	static GluonApp* s_Instance;

	i32 Width  = 1024;
	i32 Height = 768;

	glm::vec4 BackgroundColor;

	beard::array<RectangleInfo> Rectangles;
};
