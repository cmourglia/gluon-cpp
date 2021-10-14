#include <Gluon/App/GluonApp.h>

// #include <Gluon/Core/Utils.h>
#include <Gluon/Compiler/Parser.h>
#include <Gluon/Widgets/Widget.h>

#include <Beard/IO.h>

#include <raylib.h>
#include <raymath.h>
#include <nanosvg.h>

#include <vector>

GluonApp* GluonApp::s_instance = nullptr;

GluonApp* GluonApp::Get() { return s_instance; }

GluonApp::GluonApp(int argc, char** argv)
    : background_color{0.8f, 0.8f, 0.8f, 1.0f}
{
	// TODO: Do stuff with argc, argv;
	UNUSED(argc);
	UNUSED(argv);

	ASSERT(s_instance == nullptr, "Multiple initializations");

	if (s_instance == nullptr)
	{
		s_instance = this;

		// TODO: Extract window infos if set
		SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
		InitWindow(1024, 768, "Gluon Muon Whatever");

		SetTargetFPS(-1);
	}
}

GluonApp::~GluonApp() { CloseWindow(); }

int GluonApp::Run()
{
	i64 last_write_time = 0;

	auto convert_color = [](const glm::vec4& color) -> Color
	{
		return Color{static_cast<u8>(color.r * 255),
		             static_cast<u8>(color.g * 255),
		             static_cast<u8>(color.b * 255),
		             static_cast<u8>(color.a * 255)};
	};

	SetTargetFPS(120);

	GluonWidget* root_widget = nullptr;

	while (!WindowShouldClose())
	{
		// Check if gluon file update is needed
		i64 write_time = -1;

		bool draw_needs_update = false;

		if (auto file_content = Beard::IO::ReadWholeFileIfNewer("test.gluon",
		                                                        last_write_time,
		                                                        &write_time);
		    file_content.has_value())
		{
			delete root_widget;
			root_widget       = parse_gluon_buffer(file_content.value().c_str());
			draw_needs_update = true;

			last_write_time = write_time;
		}

		if (root_widget != nullptr)
		{
			if (IsWindowResized())
			{
				i32 w = GetScreenWidth();
				i32 h = GetScreenHeight();
				draw_needs_update |= root_widget->window_resized(w, h);
			}

			Vector2 mouseDelta = GetMouseDelta();
			if (Vector2LengthSqr(mouseDelta) > 0.0f)
			{
				Vector2 mousePos = GetMousePosition();

				// draw_needs_update |= root_widget->MouseMoved({mousePos.x,
				// mousePos.y});
			}

			if (draw_needs_update)
			{
				rectangles.clear();
				GluonWidget::evaluate();
				root_widget->build_render_infos(&rectangles);
			}
		}

		ClearBackground(convert_color(background_color));

		BeginDrawing();
		{
			for (const auto& rect : rectangles)
			{
				Rectangle r = {rect.position.x, rect.position.y, rect.size.x, rect.size.y};

				f32 small_side = Beard::Min(r.width, r.height);
				f32 roundness  = Beard::Min(rect.radius, small_side) / small_side;

				if (rect.is_image)
				{
					if (rect.image_info->is_vectorial)
					{
						NSVGshape* shape = rect.image_info->svg_image->shapes;
						while (shape != nullptr)
						{
							if ((shape->flags & NSVG_FLAGS_VISIBLE) == 0)
							{
								continue;
							}

							Color color = *(Color*)&shape->stroke.color;
							color.a     = u8(shape->opacity * 255);

							NSVGpath* path = shape->paths;
							while (path != nullptr)
							{
								for (int i = 0; i < path->npts - 1; i += 3)
								{
									float* p = &path->pts[i * 2];
									DrawLineBezierCubic(Vector2{p[0] + r.x, p[1] + r.y},
									                    Vector2{p[6] + r.x, p[7] + r.y},
									                    Vector2{p[2] + r.x, p[3] + r.y},
									                    Vector2{p[4] + r.x, p[5] + r.y},
									                    shape->strokeWidth,
									                    color);
								}

								path = path->next;
							}

							shape = shape->next;
						}
					}
					else
					{
						Texture2D* texture = rect.image_info->raster_image->texture;
						DrawTexture(*texture,
						            (int)(r.x + rect.image_info->raster_image->offset_x),
						            (int)(r.y + rect.image_info->raster_image->offset_y),
						            convert_color(rect.fill_color));
						DrawRectangleRoundedLines(r, 0, 1, 5, BLACK);
					}
				}
				else
				{
					DrawRectangleRounded(r, roundness, 32, convert_color(rect.fill_color));

					if (rect.border_width > 0.0f)
					{
						DrawRectangleRoundedLines(r,
						                          roundness,
						                          32,
						                          rect.border_width,
						                          convert_color(rect.border_color));
					}
				}
			}

			DrawFPS(10, 10);
		}
		EndDrawing();
	}

	return 0;
}

void GluonApp::set_title(const char* title) { SetWindowTitle(title); }

void GluonApp::set_window_size(i32 w, i32 h)
{
	::SetWindowSize(w, h);
	width  = w;
	height = h;
}
