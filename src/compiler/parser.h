#pragma once

#include "types.h"

#include <vector>
#include <optional>
#include <string>

struct ParsedApplication
{
	std::optional<std::string> applicationName{};
	// std::optional <
};

// TODO: Output an intermediate scene graph instead of a flat RectangleInfo vector
std::vector<RectangleInfo> ParseGluonFile(const char* file);