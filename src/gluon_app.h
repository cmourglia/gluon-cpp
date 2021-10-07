#pragma once

#include "types.h"

#include <glm/glm.hpp>

#include <vector>

class GluonApp
{
public:
	static GluonApp* Get();

	explicit GluonApp(int argc, char** argv);
	~GluonApp();

	int Run();

	void SetTitle(const char* title);
	void SetWindowSize(u32 w, u32 h);
	void SetBackgroundColor(const glm::vec4& color)
	{
		backgroundColor = color;
	}

	glm::vec2 GetWindowSize() const
	{
		return glm::vec2{(f32)width, (f32)height};
	}

private:
	static GluonApp* s_instance;

	GluonApp(const GluonApp&) = delete;
	GluonApp(GluonApp&&)      = delete;
	void operator=(const GluonApp&) = delete;
	void operator=(GluonApp&&) = delete;

	u32 width  = 1024;
	u32 height = 768;

	glm::vec4 backgroundColor;

	std::vector<RectangleInfo> rectangles;
};
