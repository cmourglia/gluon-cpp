#include <Gluon/Widgets/Image.h>

#include <Gluon/Widgets/Hashes.h>

#include <raylib.h>

#include <nanosvg.h>

#include <loguru.hpp>

#include <filesystem>

namespace Utils
{
glm::vec4 extract_color(const std::vector<Token>& tokens);
}

GluonImage::~GluonImage()
{
	if (image_info.svg_image != nullptr)
	{
		nsvgDelete(image_info.svg_image);
	}

	if (image_info.raster_image != nullptr)
	{
#if 0
		stbi_image_free(raster_image->data);
#else
		UnloadTexture(*image_info.raster_image->texture);
		delete image_info.raster_image->texture;

		UnloadImage(*image_info.raster_image->image);
		delete (image_info.raster_image->image);
#endif
		delete image_info.raster_image;
	}
}

void GluonImage::parse_property_internal(Parser::Node::Ptr node,
                                         const u32         node_hash)
{
	switch (node_hash)
	{
		case NodeHash::Url:
		{
			ASSERT(!node->children.empty(), "No children is bad");

			imageUrl = node->children[0]->name;
			std::filesystem::path fp(imageUrl);

			if (fp.extension().string() == ".svg")
			{
				image_info.is_vectorial = true;
			}
			else
			{
				image_info.is_vectorial = false;
				image_info.raster_image = new RasterImage;

#if 0
			i32 componentCount = 0;
			raster_image->data  = stbi_load(imageUrl.c_str(),
                                          &raster_image->width,
                                          &raster_image->height,
                                          &componentCount,
                                          4);
#else
				Image* image                   = new Image;
				*image                         = LoadImage(imageUrl.c_str());
				image_info.raster_image->image = image;
#endif

				if (image_info.raster_image->image == nullptr)
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
			imageTint = Utils::extract_color(
			    node->children[0]->associated_tokens);
		}
		break;
	}
}

void GluonImage::post_evaluate()
{
	if (image_info.is_vectorial)
	{
		// TODO: This should be evaluated
		image_info.svg_image = nsvgParseFromFile(imageUrl.c_str(),
		                                         "px",
		                                         min(size.x, size.y));
	}
	else
	{
		Image* image = image_info.raster_image->image;
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
				image_info.raster_image->offset_x = (size.x - targetWidth) *
				                                    0.5f;
				image_info.raster_image->offset_y = (size.y - targetHeight) *
				                                    0.5f;
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

		image_info.raster_image->texture = texture;
	}
}

void GluonImage::build_render_infos_internal(std::vector<RectangleInfo>* result)
{
	RectangleInfo info = {};
	info.position      = pos;
	info.size          = size;
	info.fill_color    = imageTint;
	info.is_image      = true;
	info.image_info    = &image_info;
	result->push_back(std::move(info));
}
