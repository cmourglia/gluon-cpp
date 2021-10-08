#pragma once

#include "types.h"

#include <glm/glm.hpp>

#include <vector>

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

	void SetTitle(const char* title);
	void SetWindowSize(i32 w, i32 h);
	void SetBackgroundColor(const glm::vec4& color)
	{
		backgroundColor = color;
	}

	[[nodiscard]] glm::vec2 GetWindowSize() const
	{
		return glm::vec2{(f32)width, (f32)height};
	}

private:
	static GluonApp* s_instance;

	i32 width  = 1024;
	i32 height = 768;

	glm::vec4 backgroundColor;

	std::vector<RectangleInfo> rectangles;
};
