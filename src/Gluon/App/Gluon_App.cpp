#include <Gluon/App/Gluon_App.h>

// #include <Gluon/Core/Utils.h>
#include <Gluon/Compiler/Gluon_Parser.h>
#include <Gluon/Widgets/Gluon_Widget.h>

#include <Beard/IO.h>

#include <nanosvg.h>
#include <raylib.h>
#include <raymath.h>

#include <vector>

GluonApp* GluonApp::s_Instance = nullptr;

GluonApp* GluonApp::Get()
{
	return s_Instance;
}

GluonApp::GluonApp(int argc, char** argv)
    : BackgroundColor{0.8f, 0.8f, 0.8f, 1.0f}
{
	// TODO: Do stuff with argc, argv;
	UNUSED(argc);
	UNUSED(argv);

	ASSERT(s_Instance == nullptr, "Multiple initializations");

	if (s_Instance == nullptr)
	{
		s_Instance = this;

		// TODO: Extract window infos if set
		SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
		InitWindow(1024, 768, "Gluon Muon Whatever");

		SetTargetFPS(-1);
	}
}

GluonApp::~GluonApp()
{
	CloseWindow();
}

int GluonApp::Run()
{
	i64 LastWriteTime = 0;

	auto ConvertColor = [](const glm::vec4& InColor) -> Color
	{
		return Color{static_cast<u8>(InColor.r * 255),
		             static_cast<u8>(InColor.g * 255),
		             static_cast<u8>(InColor.b * 255),
		             static_cast<u8>(InColor.a * 255)};
	};

	SetTargetFPS(120);

	ZWidget* RootWidget = nullptr;

	while (!WindowShouldClose())
	{
		// Check if gluon file update is needed
		i64 write_time = -1;

		bool bDrawNeedsUpdate = false;

		if (auto file_content = Beard::IO::ReadWholeFileIfNewer("test.gluon", LastWriteTime, &write_time);
		    file_content.has_value())
		{
			delete RootWidget;
			RootWidget       = parse_gluon_buffer(file_content.value().c_str());
			bDrawNeedsUpdate = true;

			LastWriteTime = write_time;
		}

		if (RootWidget != nullptr)
		{
			if (IsWindowResized())
			{
				i32 w = GetScreenWidth();
				i32 h = GetScreenHeight();
				bDrawNeedsUpdate |= RootWidget->WindowResized(w, h);
			}

			Vector2 MouseDelta = GetMouseDelta();
			if (Vector2LengthSqr(MouseDelta) > 0.0f)
			{
				Vector2 MousePos = GetMousePosition();

				// bDrawNeedsUpdate |= RootWidget->MouseMoved({MousePos.x,
				// MousePos.y});
			}

			if (bDrawNeedsUpdate)
			{
				Rectangles.Clear();
				ZWidget::Evaluate();
				RootWidget->BuildRenderInfos(&Rectangles);
			}
		}

		ClearBackground(ConvertColor(BackgroundColor));

		BeginDrawing();
		{
			for (const auto& Rect : Rectangles)
			{
				Rectangle R = {Rect.Position.x, Rect.Position.y, Rect.Size.x, Rect.Size.y};

				f32 SmallSide = Beard::Min(R.width, R.height);
				f32 Roundness = Beard::Min(Rect.Radius, SmallSide) / SmallSide;

				if (Rect.bIsImage)
				{
					if (Rect.ImageInfo->bIsVectorial)
					{
						NSVGshape* Shape = Rect.ImageInfo->SvgImage->shapes;
						while (Shape != nullptr)
						{
							if ((Shape->flags & NSVG_FLAGS_VISIBLE) == 0)
							{
								continue;
							}

							Color ShapeColor = *(Color*)&Shape->stroke.color;
							ShapeColor.a     = u8(Shape->opacity * 255);

							NSVGpath* Path = Shape->paths;
							while (Path != nullptr)
							{
								for (int i = 0; i < Path->npts - 1; i += 3)
								{
									float* P = &Path->pts[i * 2];
									DrawLineBezierCubic(Vector2{P[0] + R.x, P[1] + R.y},
									                    Vector2{P[6] + R.x, P[7] + R.y},
									                    Vector2{P[2] + R.x, P[3] + R.y},
									                    Vector2{P[4] + R.x, P[5] + R.y},
									                    Shape->strokeWidth,
									                    ShapeColor);
								}

								Path = Path->next;
							}

							Shape = Shape->next;
						}
					}
					else
					{
						Texture2D* Texture = Rect.ImageInfo->RasterImage->Texture;
						DrawTexture(*Texture,
						            (int)(R.x + Rect.ImageInfo->RasterImage->OffsetX),
						            (int)(R.y + Rect.ImageInfo->RasterImage->OffsetY),
						            ConvertColor(Rect.FillColor));
						DrawRectangleRoundedLines(R, 0, 1, 5, BLACK);
					}
				}
				else
				{
					DrawRectangleRounded(R, Roundness, 32, ConvertColor(Rect.FillColor));

					if (Rect.BorderWidth > 0.0f)
					{
						DrawRectangleRoundedLines(R, Roundness, 32, Rect.BorderWidth, ConvertColor(Rect.BorderColor));
					}
				}
			}

			DrawFPS(10, 10);
		}
		EndDrawing();
	}

	return 0;
}

void GluonApp::SetTitle(const char* Title)
{
	SetWindowTitle(Title);
}

void GluonApp::SetWindowSize(i32 w, i32 h)
{
	::SetWindowSize(w, h);
	Width  = w;
	Height = h;
}
