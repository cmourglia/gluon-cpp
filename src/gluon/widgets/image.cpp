#include <Gluon/Widgets/image.h>

#include <Gluon/Widgets/hashes.h>

#include <SDL.h>
#include <SDL_image.h>
#include <nanosvg.h>

#include <loguru.hpp>

#include <filesystem>

namespace utils
{
glm::vec4 ExtractColor(const beard::array<Token>& tokens);
}

Image::~Image()
{
    if (image_info.svg_image != nullptr)
    {
        nsvgDelete(image_info.svg_image);
    }

    if (image_info.raster_image != nullptr)
    {
#if 0
		stbi_image_free(RasterImage->data);
#else
        SDL_FreeSurface(image_info.raster_image->image);
#endif
        delete image_info.raster_image;
    }
}

void Image::ParserPropertyInternal(parser::Node::Ptr node, const u32 node_hash)
{
    switch (node_hash)
    {
        case static_cast<u32>(NodeHash::kUrl):
        {
            ASSERT(!node->children.is_empty(), "No Children is bad");

            image_url = node->children[0]->name;
            std::filesystem::path FilePath(image_url);

            if (FilePath.extension().string() == ".svg")
            {
                image_info.is_vectorial = true;
            }
            else
            {
                image_info.is_vectorial = false;
                image_info.raster_image = new RasterImage;

                image_info.raster_image->image = IMG_Load(image_url.c_str());

                if (image_info.raster_image->image == nullptr)
                {
                    LOG_F(ERROR, "Cannot load BaseImage %s",
                          image_url.c_str()); // stbi_failure_reason());
                }

                image_size = {image_info.raster_image->image->w, image_info.raster_image->image->h};
            }
        }
        break;

        case static_cast<u32>(NodeHash::kFitMode):
        {
            std::string fit = node->children[0]->name.c_str();
            std::transform(fit.begin(),
                           fit.end(),
                           fit.begin(),
                           [](char c) { return static_cast<char>(std::tolower(c)); });

            if (strcmp(fit.c_str(), "stretch") == 0)
            {
                fit_mode = FitMode::Stretch;
            }
            else if (strcmp(fit.c_str(), "fit") == 0)
            {
                fit_mode = FitMode::Fit;
            }
            else if (strcmp(fit.c_str(), "crop") == 0)
            {
                fit_mode = FitMode::Crop;
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

        case static_cast<u32>(NodeHash::kTint):
        {
            image_tint = utils::ExtractColor(node->children[0]->associated_tokens);
        }
        break;
    }
}

void Image::PostEvaluate()
{
    if (image_info.is_vectorial)
    {
        // TODO: This should be evaluated
        image_info.svg_image = nsvgParseFromFile(image_url.c_str(), "px", beard::min(size.x, size.y));
    }
    else
    {
#if 0
        Image* BaseImage = image_info.RasterImage->BaseImage;
        UnloadImage(*BaseImage);
        *BaseImage = LoadImage(image_url.c_str());

        switch (fit_mode)
        {
            case FitMode::Stretch:
            {
                ImageResize(BaseImage, (i32)size.x, (i32)size.y);
            }
            break;

            case FitMode::Fit:
            {
                i32 TargetWidth  = 0;
                i32 TargetHeight = 0;

                f32 ImageRatio = static_cast<f32>(BaseImage->width) / static_cast<f32>(BaseImage->height);
                if (size.x < size.y)
                {
                    TargetWidth  = static_cast<i32>(size.x);
                    TargetHeight = static_cast<i32>(size.x / ImageRatio);
                }
                else
                {
                    TargetHeight = static_cast<i32>(size.y);
                    TargetWidth  = static_cast<i32>(size.y * ImageRatio);
                }

                ImageResize(BaseImage, TargetWidth, TargetHeight);

                // Compute offsets to center in the target quad
                image_info.raster_image->offset_x = (size.x - TargetWidth) * 0.5f;
                image_info.raster_image->offset_y = (size.y - TargetHeight) * 0.5f;
            }
            break;

            case FitMode::Crop:
            {
                i32 TargetWidth  = 0;
                i32 TargetHeight = 0;

                f32 ImageRatio = static_cast<f32>(BaseImage->width) / static_cast<f32>(BaseImage->height);
                if (size.x > size.y)
                {
                    TargetWidth  = static_cast<i32>(size.x);
                    TargetHeight = static_cast<i32>(size.x / ImageRatio);
                }
                else
                {
                    TargetHeight = static_cast<i32>(size.y);
                    TargetWidth  = static_cast<i32>(size.y * ImageRatio);
                }

                ImageResize(BaseImage, TargetWidth, TargetHeight);

                Rectangle r = {(TargetWidth - size.x) * 0.5f, (TargetHeight - size.y) * 0.5f, size.x, size.y};
                ImageCrop(BaseImage, r);
            }
            break;
        }

        Texture2D* Texture = new Texture2D;
        *Texture           = LoadTextureFromImage(*BaseImage);

        image_info.raster_image->Texture = Texture;
#endif
    }
}

void Image::BuildRenderInfosInternal(beard::array<RectangleInfo>* result)
{
    RectangleInfo info = {};
    info.position      = pos;
    info.size          = size;
    info.fill_color    = image_tint;
    info.is_image      = true;
    info.image_info    = &image_info;
    result->add(std::move(info));
}
