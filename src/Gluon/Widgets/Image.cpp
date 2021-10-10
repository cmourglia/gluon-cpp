#include <Gluon/Widgets/Image.h>

#include <Gluon/Widgets/Hashes.h>

#include <raylib.h>

#include <nanosvg.h>

#include <loguru.hpp>

#include <filesystem>

namespace Utils
{
glm::vec4 ExtractColor(const std::vector<Token>& tokens);
}

GluonImage::~GluonImage()
{
	if (imageInfo.svgImage != nullptr)
	{
		nsvgDelete(imageInfo.svgImage);
	}

	if (imageInfo.rasterImage != nullptr)
	{
#if 0
		stbi_image_free(rasterImage->data);
#else
		UnloadTexture(*imageInfo.rasterImage->texture);
		delete imageInfo.rasterImage->texture;

		UnloadImage(*imageInfo.rasterImage->image);
		delete (imageInfo.rasterImage->image);
#endif
		delete imageInfo.rasterImage;
	}
}

void GluonImage::ParsePropertyInternal(Parser::Node::Ptr node,
                                       const u32         nodeHash)
{
	switch (nodeHash)
	{
		case NodeHash::Url:
		{
			Assert(!node->children.empty(), "No children is bad");

			imageUrl = node->children[0]->name;
			std::filesystem::path fp(imageUrl);

			if (fp.extension().string() == ".svg")
			{
				imageInfo.isVectorial = true;
			}
			else
			{
				imageInfo.isVectorial = false;
				imageInfo.rasterImage = new RasterImage;

#if 0
			i32 componentCount = 0;
			rasterImage->data  = stbi_load(imageUrl.c_str(),
                                          &rasterImage->width,
                                          &rasterImage->height,
                                          &componentCount,
                                          4);
#else
				Image* image                 = new Image;
				*image                       = LoadImage(imageUrl.c_str());
				imageInfo.rasterImage->image = image;
#endif

				if (imageInfo.rasterImage->image == nullptr)
				{
					LOG_F(ERROR,
					      "Cannot load image %s",
					      imageUrl.c_str()); // stbi_failure_reason());
				}
				imageSize = {image->width, image->height};
			}
		}
		break;

		case NodeHash::FitMode:
		{
			std::string fit = node->children[0]->name.c_str();
			std::transform(fit.begin(),
			               fit.end(),
			               fit.begin(),
			               [](char c)
			               { return static_cast<char>(std::tolower(c)); });

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
				      node->children[0]->name.c_str());
			}
		}
		break;

		case NodeHash::Tint:
		{
			imageTint = Utils::ExtractColor(
			    node->children[0]->associatedTokens);
		}
		break;
	}
}

void GluonImage::PostEvaluate()
{
	if (imageInfo.isVectorial)
	{
		// TODO: This should be evaluated
		imageInfo.svgImage = nsvgParseFromFile(imageUrl.c_str(),
		                                       "px",
		                                       Min(size.x, size.y));
	}
	else
	{
		Image* image = imageInfo.rasterImage->image;
		UnloadImage(*image);
		*image = LoadImage(imageUrl.c_str());

		switch (fitMode)
		{
			case FitMode::Stretch:
			{
				ImageResize(image, (i32)size.x, (i32)size.y);
			}
			break;

			case FitMode::Fit:
			{
				i32 targetWidth  = 0;
				i32 targetHeight = 0;

				f32 imageRatio = static_cast<f32>(image->width) /
				                 static_cast<f32>(image->height);
				if (size.x < size.y)
				{
					targetWidth  = static_cast<i32>(size.x);
					targetHeight = static_cast<i32>(size.x / imageRatio);
				}
				else
				{
					targetHeight = static_cast<i32>(size.y);
					targetWidth  = static_cast<i32>(size.y * imageRatio);
				}

				ImageResize(image, targetWidth, targetHeight);

				// Compute offsets to center in the target quad
				imageInfo.rasterImage->offsetX = (size.x - targetWidth) * 0.5f;
				imageInfo.rasterImage->offsetY = (size.y - targetHeight) * 0.5f;
			}
			break;

			case FitMode::Crop:
			{
				i32 targetWidth  = 0;
				i32 targetHeight = 0;

				f32 imageRatio = static_cast<f32>(image->width) /
				                 static_cast<f32>(image->height);
				if (size.x > size.y)
				{
					targetWidth  = static_cast<i32>(size.x);
					targetHeight = static_cast<i32>(size.x / imageRatio);
				}
				else
				{
					targetHeight = static_cast<i32>(size.y);
					targetWidth  = static_cast<i32>(size.y * imageRatio);
				}

				ImageResize(image, targetWidth, targetHeight);

				Rectangle r = {(targetWidth - size.x) * 0.5f,
				               (targetHeight - size.y) * 0.5f,
				               size.x,
				               size.y};
				ImageCrop(image, r);
			}
			break;
		}

		Texture2D* texture = new Texture2D;
		*texture           = LoadTextureFromImage(*image);

		imageInfo.rasterImage->texture = texture;
	}
}

void GluonImage::BuildRenderInfosInternal(std::vector<RectangleInfo>* result)
{
	RectangleInfo info = {};
	info.position      = pos;
	info.size          = size;
	info.fillColor     = imageTint;
	info.isImage       = true;
	info.imageInfo     = &imageInfo;
	result->push_back(std::move(info));
}
