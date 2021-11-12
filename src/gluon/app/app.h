#pragma once

#include <gluon/core/types.h>

#include <beard/containers/array.h>

struct SDL_Window;
struct SDL_Surface;

class GluonApp
{
public:
    static GluonApp* instance();

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
        m_background_color = Color;
    }

    glm::vec2 GetWindowSize() const
    {
        return glm::vec2{static_cast<f32>(m_width), static_cast<f32>(m_height)};
    }

private:
    static GluonApp* s_instance;

    i32 m_width  = 1024;
    i32 m_height = 768;

    glm::vec4 m_background_color;

    beard::array<RectangleInfo> m_rectangles;

    SDL_Window*  m_window  = nullptr;
    SDL_Surface* m_surface = nullptr;
};
