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

private:
	static GluonApp* s_instance;

	GluonApp(const GluonApp&) = delete;
	GluonApp(GluonApp&&)      = delete;
	void operator=(const GluonApp&) = delete;
	void operator=(GluonApp&&) = delete;

	u32 width  = 1024;
	u32 height = 768;

	std::vector<RectangleInfo> rectangles;
};
