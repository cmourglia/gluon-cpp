#pragma once

#include "types.h"

#include <string>

namespace FileUtils
{
i64         GetFileWriteTime(const char* filename);
std::string ReadWholeFile(const char* filename);
bool        ReadFileIfNewer(const char* filename, i64 lastWrite, i64* newLastWrite, std::string* content);
}