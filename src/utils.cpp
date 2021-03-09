#include "utils.h"

#include <loguru.hpp>
#include <bx/platform.h>

#if BX_PLATFORM_WINDOWS
#	include <Windows.h>
#elif
#	error TODO
#endif

namespace
{
#if BX_PLATFORM_WINDOWS
struct HandleRAII
{
	HandleRAII(HANDLE handle)
	    : handle(handle)
	{
	}

	~HandleRAII()
	{
		if (handle != 0)
		{
			CloseHandle(handle);
			handle = 0;
		}
	}

	operator HANDLE() const
	{
		return handle;
	}

	HANDLE handle = {0};
};
#endif
}

namespace FileUtils
{
i64 GetFileWriteTime(const char* filename)
{
#if BX_PLATFORM_WINDOWS
	HandleRAII fileHandle{CreateFileA(filename,
	                                  GENERIC_READ,
	                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
	                                  nullptr,
	                                  OPEN_EXISTING,
	                                  FILE_ATTRIBUTE_NORMAL,
	                                  nullptr)};

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();

		if (error != ERROR_SHARING_VIOLATION)
		{
			LOG_F(ERROR, "Error code %d", error);
		}

		return 0;
	}

	FILE_BASIC_INFO infos;

	bool ok = GetFileInformationByHandleEx(fileHandle, FileBasicInfo, &infos, sizeof(infos));
	if (!ok)
	{
		DWORD error = GetLastError();
		LOG_F(ERROR, "Error code %d", error);

		return 0;
	}

	return infos.LastWriteTime.QuadPart;
#else
	static_assert(false, "TODO");
#endif
}

std::string ReadWholeFile(const char* filename)
{
	std::string result;
#if BX_PLATFORM_WINDOWS
	HandleRAII fileHandle{CreateFileA(filename,
	                                  GENERIC_READ,
	                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
	                                  nullptr,
	                                  OPEN_EXISTING,
	                                  FILE_ATTRIBUTE_NORMAL,
	                                  nullptr)};

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();

		if (error != ERROR_SHARING_VIOLATION)
		{
			LOG_F(ERROR, "Error code %d", error);
		}
		else
		{
			LOG_F(WARNING, "Hello");
		}

		return result;
	}

	FILE_STANDARD_INFO infos;

	bool ok = GetFileInformationByHandleEx(fileHandle, FileStandardInfo, &infos, sizeof(infos));
	if (!ok)
	{
		DWORD error = GetLastError();
		LOG_F(ERROR, "Error code %d", error);

		return result;
	}

	result.resize(infos.EndOfFile.QuadPart);

	DWORD bytesRead = 0;
	ReadFile(fileHandle, result.data(), infos.EndOfFile.QuadPart, &bytesRead, nullptr);

#else
	static_assert(false, "TODO");
#endif

	return result;
}

bool ReadFileIfNewer(const char* filename, i64 lastWrite, i64* newLastWrite, std::string* content)
{
#if BX_PLATFORM_WINDOWS
	HandleRAII fileHandle{CreateFileA(filename,
	                                  GENERIC_READ,
	                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
	                                  nullptr,
	                                  OPEN_EXISTING,
	                                  FILE_ATTRIBUTE_NORMAL,
	                                  nullptr)};

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();

		if (error != ERROR_SHARING_VIOLATION)
		{
			LOG_F(ERROR, "Error code %d", error);
		}
		else
		{
			LOG_F(WARNING, "Hello");
		}

		return false;
	}

	bool ok = true;

	FILE_BASIC_INFO basicInfos;
	ok = GetFileInformationByHandleEx(fileHandle, FileBasicInfo, &basicInfos, sizeof(basicInfos));
	if (!ok)
	{
		DWORD error = GetLastError();
		LOG_F(ERROR, "Error code %d", error);

		return false;
	}

	if (lastWrite >= basicInfos.LastWriteTime.QuadPart)
	{
		return false;
	}

	FILE_STANDARD_INFO standardInfos;
	ok = GetFileInformationByHandleEx(fileHandle, FileStandardInfo, &standardInfos, sizeof(standardInfos));
	if (!ok)
	{
		DWORD error = GetLastError();
		LOG_F(ERROR, "Error code %d", error);

		return false;
	}

	if (standardInfos.EndOfFile.QuadPart == 0)
	{
		return false;
	}

	std::string contentTmp;
	DWORD       bytesRead = 0;
	contentTmp.resize(standardInfos.EndOfFile.QuadPart);

	ok = ReadFile(fileHandle, contentTmp.data(), standardInfos.EndOfFile.QuadPart, &bytesRead, nullptr);

	if (!ok)
	{
		DWORD error = GetLastError();
		LOG_F(ERROR, "Error code %d", error);

		return false;
	}

	*newLastWrite = basicInfos.LastWriteTime.QuadPart;
	*content      = std::move(contentTmp);

	return true;
#else
	static_assert(false, "TODO");
#endif
}
}