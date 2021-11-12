#include <gluon/app/app.h>

// #include <gluon/core/Utils.h>
#include <gluon/compiler/parser.h>
#include <gluon/widgets/widget.h>

#include <beard/io/io.h>

#include <SDL.h>
#include <SDL_image.h>
#include <nanosvg.h>

#include <vector>

GluonApp* GluonApp::s_instance = nullptr;

GluonApp* GluonApp::instance()
{
    return s_instance;
}

GluonApp::GluonApp(int argc, char** argv)
    : m_background_color{0.8f, 0.8f, 0.8f, 1.0f}
{
    // TODO: Do stuff with argc, argv;
    UNUSED(argc);
    UNUSED(argv);

    ASSERT(s_instance == nullptr, "Multiple initializations");

    if (s_instance == nullptr)
    {
        s_instance = this;

        // TODO: Extract window infos if set
        // TODO: Error handling
        SDL_Init(SDL_INIT_VIDEO);
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP);

        u32 window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
        m_window = SDL_CreateWindow("Hey !", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, window_flags);
        m_surface = SDL_GetWindowSurface(m_window);
    }
}

GluonApp::~GluonApp()
{
    SDL_DestroyWindow(m_window);

    m_surface = nullptr;
    m_window  = nullptr;

    IMG_Quit();
    SDL_Quit();
}

int GluonApp::Run()
{
    i64 last_write_time = 0;

    // auto ConvertColor = [](const glm::vec4& InColor) -> Color
    //{
    //     return Color{static_cast<u8>(InColor.r * 255),
    //                  static_cast<u8>(InColor.g * 255),
    //                  static_cast<u8>(InColor.b * 255),
    //                  static_cast<u8>(InColor.a * 255)};
    // };

    Widget* root_widget = nullptr;
    bool    should_quit = false;

    while (!should_quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    should_quit = true;
                    break;
            }
        }

        if (should_quit)
        {
            continue;
        }

        // Check if gluon file update is needed
        i64 write_time = -1;

        bool draw_needs_update = false;

        if (auto file_content = beard::io::read_while_file_if_newer("test.gluon", last_write_time, &write_time);
            file_content.has_value())
        {
            delete root_widget;
            root_widget       = ParseGluonBuffer(file_content.value().c_str());
            draw_needs_update = true;

            last_write_time = write_time;
        }

        if (root_widget != nullptr)
        {
            // if (IsWindowResized())
            //{
            //     i32 w = GetScreenWidth();
            //     i32 h = GetScreenHeight();
            //     bDrawNeedsUpdate |= root_widget->WindowResized(w, h);
            // }

            // Vector2 MouseDelta = GetMouseDelta();
            // if (Vector2LengthSqr(MouseDelta) > 0.0f)
            //{
            //     Vector2 MousePos = GetMousePosition();

            //    // bDrawNeedsUpdate |= RootWidget->MouseMoved({MousePos.x,
            //    // MousePos.y});
            //}

            if (draw_needs_update)
            {
                m_rectangles.clear();
                Widget::Evaluate();
                root_widget->BuildRenderInfos(&m_rectangles);
            }
        }

        // ClearBackground(ConvertColor(m_background_color));

        // BeginDrawing();
        //{
        //     for (const auto& Rect : m_rectangles)
        //     {
        //         Rectangle R = {Rect.Position.x, Rect.Position.y, Rect.Size.x, Rect.Size.y};

        //        f32 SmallSide = beard::min(R.width, R.height);
        //        f32 Roundness = beard::min(Rect.Radius, SmallSide) / SmallSide;

        //        if (Rect.bIsImage)
        //        {
        //            if (Rect.ImageInfo->bIsVectorial)
        //            {
        //                NSVGshape* Shape = Rect.ImageInfo->SvgImage->shapes;
        //                while (Shape != nullptr)
        //                {
        //                    if ((Shape->flags & NSVG_FLAGS_VISIBLE) == 0)
        //                    {
        //                        continue;
        //                    }

        //                    Color ShapeColor = *(Color*)&Shape->stroke.color;
        //                    ShapeColor.a     = u8(Shape->opacity * 255);

        //                    NSVGpath* Path = Shape->paths;
        //                    while (Path != nullptr)
        //                    {
        //                        for (int i = 0; i < Path->npts - 1; i += 3)
        //                        {
        //                            float* P = &Path->pts[i * 2];
        //                            DrawLineBezierCubic(Vector2{P[0] + R.x, P[1] + R.y},
        //                                                Vector2{P[6] + R.x, P[7] + R.y},
        //                                                Vector2{P[2] + R.x, P[3] + R.y},
        //                                                Vector2{P[4] + R.x, P[5] + R.y},
        //                                                Shape->strokeWidth,
        //                                                ShapeColor);
        //                        }

        //                        Path = Path->next;
        //                    }

        //                    Shape = Shape->next;
        //                }
        //            }
        //            else
        //            {
        //                Texture2D* Texture = Rect.ImageInfo->RasterImage->Texture;
        //                DrawTexture(*Texture,
        //                            (int)(R.x + Rect.ImageInfo->RasterImage->OffsetX),
        //                            (int)(R.y + Rect.ImageInfo->RasterImage->OffsetY),
        //                            ConvertColor(Rect.FillColor));
        //                DrawRectangleRoundedLines(R, 0, 1, 5, BLACK);
        //            }
        //        }
        //        else
        //        {
        //            DrawRectangleRounded(R, Roundness, 32, ConvertColor(Rect.FillColor));

        //            if (Rect.BorderWidth > 0.0f)
        //            {
        //                DrawRectangleRoundedLines(R, Roundness, 32, Rect.BorderWidth, ConvertColor(Rect.BorderColor));
        //            }
        //        }
        //    }
        //}
    }

    return 0;
}

void GluonApp::SetTitle(const char* title)
{
    SDL_SetWindowTitle(m_window, title);
}

void GluonApp::SetWindowSize(i32 w, i32 h)
{
    SDL_SetWindowSize(m_window, w, h);
    m_width  = w;
    m_height = h;
}
