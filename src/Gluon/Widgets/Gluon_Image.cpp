#include <Gluon/Widgets/Gluon_Image.h>

#include <Gluon/Widgets/Gluon_Hashes.h>

#include <raylib.h>

#include <nanosvg.h>

#include <loguru.hpp>

#include <filesystem>

namespace Utils
{
glm::vec4 ExtractColor(const Beard::Array<ZToken>& Tokens);
}

ZImage::~ZImage()
{
	if (ImageInfo.SvgImage != nullptr)
	{
		nsvgDelete(ImageInfo.SvgImage);
	}

	if (ImageInfo.RasterImage != nullptr)
	{
#if 0
		stbi_image_free(RasterImage->data);
#else
		UnloadTexture(*ImageInfo.RasterImage->Texture);
		delete ImageInfo.RasterImage->Texture;

		UnloadImage(*ImageInfo.RasterImage->BaseImage);
		delete (ImageInfo.RasterImage->BaseImage);
#endif
		delete ImageInfo.RasterImage;
	}
}

void ZImage::ParserPropertyInternal(Parser::ZNode::Ptr Node, const u32 node_hash)
{
	switch (node_hash)
	{
		case static_cast<u32>(ENodeHash::Url):
		{
			ASSERT(!Node->Children.IsEmpty(), "No Children is bad");

			ImageURL = Node->Children[0]->Name;
			std::filesystem::path FilePath(ImageURL);

			if (FilePath.extension().string() == ".svg")
			{
				ImageInfo.bIsVectorial = true;
			}
			else
			{
				ImageInfo.bIsVectorial = false;
				ImageInfo.RasterImage  = new RasterImage;

#if 0
			i32 componentCount = 0;
			RasterImage->data  = stbi_load(ImageURL.c_str(),
                                          &RasterImage->Width,
                                          &RasterImage->Height,
                                          &componentCount,
                                          4);
#else
				auto* BaseImage                  = new Image;
				*BaseImage                       = LoadImage(ImageURL.c_str());
				ImageInfo.RasterImage->BaseImage = BaseImage;
#endif

				if (ImageInfo.RasterImage->BaseImage == nullptr)
				{
					LOG_F(ERROR, "Cannot load BaseImage %s",
					      ImageURL.c_str()); // stbi_failure_reason());
				}
				ImageSize = {BaseImage->width, BaseImage->height};
			}
		}
		break;

		case static_cast<u32>(ENodeHash::FitMode):
		{
			std::string fit = Node->Children[0]->Name.c_str();
			std::transform(fit.begin(),
			               fit.end(),
			               fit.begin(),
			               [](char c) { return static_cast<char>(std::tolower(c)); });

			if (strcmp(fit.c_str(), "stretch") == 0)
			{
				fitMode = FitMode::Stretch;
			}
			else if (strcmp(fit.c_str(), "fit") == 0)
			{
				fitMode = FitMode::Fit;
			}
			else if (strcmp(fit.c_str(), "crop") == 0)
			{
				fitMode = FitMode::Crop;
			}
			else
			{
				LOG_F(ERROR,
				      "%s is not a valid fit mode. Valid modes are Stretch, "
				      "Fit or Crop",
				      Node->Children[0]->Name.c_str());
			}
		}
		break;

		case static_cast<u32>(ENodeHash::Tint):
		{
			imageTint = Utils::ExtractColor(Node->Children[0]->AssociatedTokens);
		}
		break;
	}
}

void ZImage::PostEvaluate()
{
	if (ImageInfo.bIsVectorial)
	{
		// TODO: This should be evaluated
		ImageInfo.SvgImage = nsvgParseFromFile(ImageURL.c_str(), "px", Beard::Min(Size.x, Size.y));
	}
	else
	{
		Image* BaseImage = ImageInfo.RasterImage->BaseImage;
		UnloadImage(*BaseImage);
		*BaseImage = LoadImage(ImageURL.c_str());

		switch (fitMode)
		{
			case FitMode::Stretch:
			{
				ImageResize(BaseImage, (i32)Size.x, (i32)Size.y);
			}
			break;

			case FitMode::Fit:
			{
				i32 TargetWidth  = 0;
				i32 TargetHeight = 0;

				f32 ImageRatio = static_cast<f32>(BaseImage->width) / static_cast<f32>(BaseImage->height);
				if (Size.x < Size.y)
				{
					TargetWidth  = static_cast<i32>(Size.x);
					TargetHeight = static_cast<i32>(Size.x / ImageRatio);
				}
				else
				{
					TargetHeight = static_cast<i32>(Size.y);
					TargetWidth  = static_cast<i32>(Size.y * ImageRatio);
				}

				ImageResize(BaseImage, TargetWidth, TargetHeight);

				// Compute offsets to center in the target quad
				ImageInfo.RasterImage->OffsetX = (Size.x - TargetWidth) * 0.5f;
				ImageInfo.RasterImage->OffsetY = (Size.y - TargetHeight) * 0.5f;
			}
			break;

			case FitMode::Crop:
			{
				i32 TargetWidth  = 0;
				i32 TargetHeight = 0;

				f32 ImageRatio = static_cast<f32>(BaseImage->width) / static_cast<f32>(BaseImage->height);
				if (Size.x > Size.y)
				{
					TargetWidth  = static_cast<i32>(Size.x);
					TargetHeight = static_cast<i32>(Size.x / ImageRatio);
				}
				else
				{
					TargetHeight = static_cast<i32>(Size.y);
					TargetWidth  = static_cast<i32>(Size.y * ImageRatio);
				}

				ImageResize(BaseImage, TargetWidth, TargetHeight);

				Rectangle r = {(TargetWidth - Size.x) * 0.5f, (TargetHeight - Size.y) * 0.5f, Size.x, Size.y};
				ImageCrop(BaseImage, r);
			}
			break;
		}

		Texture2D* Texture = new Texture2D;
		*Texture           = LoadTextureFromImage(*BaseImage);

		ImageInfo.RasterImage->Texture = Texture;
	}
}

void ZImage::BuildRenderInfosInternal(Beard::Array<RectangleInfo>* Result)
{
	RectangleInfo info = {};
	info.Position      = Pos;
	info.Size          = Size;
	info.FillColor     = imageTint;
	info.bIsImage      = true;
	info.ImageInfo     = &ImageInfo;
	Result->Add(std::move(info));
}
