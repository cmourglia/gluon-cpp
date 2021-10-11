#pragma once

#include <Gluon/Core/Types.h>

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

	void set_title(const char* title);
	void set_window_size(i32 w, i32 h);
	void set_background_color(const glm::vec4& color)
	{
		background_color = color;
	}

	[[nodiscard]] glm::vec2 get_window_size() const
	{
		return glm::vec2{static_cast<f32>(width), static_cast<f32>(height)};
	}

private:
	static GluonApp* s_instance;

	i32 width  = 1024;
	i32 height = 768;

	glm::vec4 background_color;

	std::vector<RectangleInfo> rectangles;
};
