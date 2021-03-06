#include "gluon_app.h"

#include "compiler/parser.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#if BX_PLATFORM_LINUX
#	define GLFW_EXPOSE_NATIVE_X11
#	define GLFW_EXPOSE_NATIVE_GLX
#elif BX_PLATFORM_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32
#	define GLFW_EXPOSE_NATIVE_WGL
#else
#	error "Uh oh"
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <loguru.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

constexpr u32 MAX_FRAMES_IN_FLIGHT = 2u;

std::vector<glm::vec4> defaultColors = {
    {1.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f, 1.0f},
    {0.0f, 1.0f, 1.0f, 1.0f},
};

namespace
{

void KeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) { }
} // namespace

GluonApp* GluonApp::Get()
{
	return s_instance;
}

GluonApp::GluonApp(int argc, char** argv)
{
	assert(s_instance == nullptr);
	s_instance = this;

	// TODO: Extract window infos if set

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(width, height, "Gluon App", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	// TODO: Setup callbacks

	glfwSetKeyCallback(window, KeyCallback);

	bgfx::PlatformData pd = {};
#if BX_PLATFORM_LINUX
	pd.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
	pd.ndt = glfwGetX11Display();
#elif BX_PLATFORM_WINDOWS
	pd.nwh = glfwGetWin32Window(window);
#else
#	error "Uh oh"
#endif
	bgfx::setPlatformData(pd);

	bgfx::Init init        = {};
	init.resolution.width  = width;
	init.resolution.height = height;
	init.resolution.reset  = BGFX_RESET_NONE;
	init.type              = bgfx::RendererType::Vulkan;
	bgfx::init(init);

	bgfx::setDebug(BGFX_DEBUG_TEXT);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030FF, 1.0f, 0);
}

GluonApp::~GluonApp() { }

int GluonApp::Run()
{
	u32 currentFrame = 0;

	f64 t0         = glfwGetTime();
	u32 frameCount = 0;

	glm::mat4 proj;
	glm::mat4 view = glm::mat4(1.0f); // Identity

	namespace fs                     = std::filesystem;
	fs::file_time_type lastWriteTime = {};

	double avgFps       = 0.0;
	double avgFrameTime = 0.0;

	while (!glfwWindowShouldClose(window))
	{
		if (++frameCount == 100)
		{
			const f64 t1 = glfwGetTime();
			const f64 t  = t1 - t0;
			t0           = t1;
			frameCount   = 0;

			avgFps       = 100.0 / t;
			avgFrameTime = (t / 100.0) * 1000.0;
		}

		glfwPollEvents();

		// Check if gluon file update is needed
		auto writeTime = fs::last_write_time("test.gluon");
		if (writeTime > lastWriteTime)
		{
			rectangles = ParseGluonFile("test.gluon");
			UpdateRectangles();
			lastWriteTime = writeTime;
		}

		bgfx::setViewRect(0, 0, 0, width, height);
		bgfx::touch(0);

		const bgfx::Stats* stats = bgfx::getStats();

		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 1, 0x0f, "Avg fps: %lf, Avg frame time: %lf", avgFps, avgFrameTime);
		bgfx::frame();
	}

	return 0;
}

void GluonApp::UpdateRectangles()
{
	vertices.clear();
	vertices.reserve(rectangles.size() * 4);

	indices.clear();
	indices.reserve(rectangles.size() * 5);

	u32 i = 0;
	for (const auto& rect : rectangles)
	{
		const glm::vec2 p = rect.position;
		const glm::vec2 s = rect.size * 0.5f;

		const f32 l = p.x - s.x;
		const f32 r = p.x + s.x;
		const f32 b = p.y - s.y;
		const f32 t = p.y + s.y;

		vertices.push_back({{l, b}, (f32)i});
		vertices.push_back({{r, b}, (f32)i});
		vertices.push_back({{l, t}, (f32)i});
		vertices.push_back({{r, t}, (f32)i});

		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
		indices.push_back(0xFFFFFFFF);

		++i;
	}

	vertexBufferDirty    = true;
	rectangleBufferDirty = true;
}
