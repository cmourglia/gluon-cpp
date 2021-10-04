#include "widget.h"

#include "parser.h"
#include "utils.h"

#include <raylib.h>

#include <nanosvg.h>
#include <stb_image.h>
#include <stb_image_resize.h>

#include <loguru.hpp>

#include <filesystem>

namespace NodeHash
{
// Generic node names
static constexpr u32 ID      = Utils::Crc32("id");
static constexpr u32 X       = Utils::Crc32("x");
static constexpr u32 Y       = Utils::Crc32("y");
static constexpr u32 Width   = Utils::Crc32("width");
static constexpr u32 Height  = Utils::Crc32("height");
static constexpr u32 Anchors = Utils::Crc32("anchors");
static constexpr u32 Padding = Utils::Crc32("padding");
static constexpr u32 Margins = Utils::Crc32("margins");

// Window
static constexpr u32 Title = Utils::Crc32("title");

// Rectangle
static constexpr u32 Color = Utils::Crc32("color");

// Image
static constexpr u32 Url     = Utils::Crc32("url");
static constexpr u32 FitMode = Utils::Crc32("fitMode");
static constexpr u32 Tint    = Utils::Crc32("tint");
}

namespace
{

inline glm::vec4 ExtractColor(const std::vector<Token>& tokens)
{
	glm::vec4 color = {};

	if (!tokens.empty())
	{
		Token firstToken = tokens.front();
		switch (firstToken.type)
		{
			case TokenType::String:
			{
				assert(tokens.size() == 1);
				firstToken.text.erase(std::remove_if(firstToken.text.begin(),
				                                     firstToken.text.end(),
				                                     [](const char c) { return c == '"'; }),
				                      firstToken.text.end());

				color = MuColor::FromString(firstToken.text);
			}
			break;

			case TokenType::Identifier:
			{
				static constexpr auto RGB_HASH  = Utils::Crc32("rgb");
				static constexpr auto RGBA_HASH = Utils::Crc32("rgba");
				static constexpr auto HSL_HASH  = Utils::Crc32("hsl");
				static constexpr auto HSLA_HASH = Utils::Crc32("hsla");

				const auto hash = Utils::Crc32(firstToken.text);

				std::vector<f32> values;
				for (const auto& t : tokens)
				{
					if (t.type == TokenType::Number)
					{
						values.push_back(t.number);
					}
				}

				switch (hash)
				{
					case RGB_HASH:
					{
						if (values.size() == 3)
						{
							color = MuColor::FromRgba(values[0], values[1], values[2]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed rgb() call, should be rgb(red, green, blue)",
							      firstToken.line);
						}
					}
					break;

					case RGBA_HASH:
					{
						if (values.size() == 4)
						{
							color = MuColor::FromRgba(values[0], values[1], values[2], values[3]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed rgba() call, should be rgba(red, green, blue, alpha)",
							      firstToken.line);
						}
					}
					break;

					case HSL_HASH:
					{
						if (values.size() == 3)
						{
							color = MuColor::FromHsla(values[0], values[1], values[2]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed hsl() call, should be hsl(hue, saturation, lightness)",
							      firstToken.line);
						}
					}
					break;

					case HSLA_HASH:
					{
						if (values.size() == 4)
						{
							color = MuColor::FromHsla(values[0], values[1], values[2], values[3]);
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Malformed hsla() call, should be hsla(hue, saturation, lightness, alpha",
							      firstToken.line);
						}
					}
					break;
					default:
					{
						if (firstToken.text.compare("Color") == 0)
						{
							assert(tokens.size() == 3 && tokens[2].type == TokenType::Identifier);
							auto it = MuColor::ColorsByName.find(tokens[2].text);
							if (it != MuColor::ColorsByName.end())
							{
								color = it->second;
							}
							else
							{
								LOG_F(ERROR,
								      "ling %d: Color '%s' is not a valid color name. See "
								      "https://www.w3schools.com/colors/colors_names.asp for a complete list.",
								      firstToken.line,
								      tokens[2].text.c_str());
							}
						}
						else
						{
							LOG_F(ERROR,
							      "line %d: Could not parse color given '%s' value",
							      firstToken.line,
							      firstToken.text.c_str());
						}
					}
					break;
				}
			}
		}
	}

	return color;
}

}

std::vector<MuWidget::Ptr> MuWidget::widgetMap = {};

extern std::unordered_map<std::string, WidgetFactory*> widgetFactories;

void MuWidget::Deserialize(Parser::Node::Ptr node)
{
	for (auto c : node->children)
	{
		if (c->type == Parser::Node_Structure)
		{
			auto child = (*widgetFactories[c->name])();
			widgetMap.push_back(child);
			children.push_back(child);

			child->parent = this;

			child->Deserialize(c);
		}
		else if (c->type == Parser::Node_Property)
		{
			ParseProperty(c);
		}
	}
}

void MuWidget::BuildRenderInfos(std::vector<RectangleInfo>* result)
{
	BuildRenderInfosInternal(result);
	for (auto c : children)
	{
		c->BuildRenderInfos(result);
	}
}

void MuWidget::Evaluate()
{
	if (dirty)
	{
		for (auto d : dependencies)
		{
			d->Evaluate();
		}

		for (auto&& eval : evaluators)
		{
			*eval.first = eval.second.Evaluate();
		}

		dirty = false;
	}
}

void MuWidget::Touch()
{
	dirty = true;
	for (auto d : dependants)
	{
		d->Touch();
	}
}

void MuWidget::ParseProperty(Parser::Node::Ptr node)
{
	const auto nodeHash = Utils::Crc32(node->name);
	switch (nodeHash)
	{
		case NodeHash::ID:
		{
			assert(node->children.size() == 1);
			id = node->children[0]->name;
		}
		break;

		case NodeHash::X:
		case NodeHash::Y:
		case NodeHash::Width:
		case NodeHash::Height:
		{
			auto infos = node->children[0];

			geometryExpressions[nodeHash] = infos->associatedTokens;

			for (usize i = 0; i < infos->associatedTokens.size() - 1; ++i)
			{
				if (infos->associatedTokens[i].type == TokenType::Identifier &&
				    infos->associatedTokens[i + 1].type == TokenType::Dot)
				{
					dependencyIds.insert(infos->associatedTokens[i].text);
				}
			}
		}
		break;

		case NodeHash::Anchors:
		{
			LOG_F(WARNING, "Anchors not handled yet");
		}
		break;

		case NodeHash::Padding:
		{
			LOG_F(WARNING, "Padding not handled yet");
		}
		break;

		case NodeHash::Margins:
		{
			LOG_F(WARNING, "Margins not handled yet");
		}
		break;

		default:
		{
			ParsePropertyInternal(node, nodeHash);
		}
		break;
	}
}

void MuWindow::ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash)
{
	pos  = {0.0f, 0.0f};
	size = {1024.0f, 768.0f};
}

void MuWindow::BuildRenderInfosInternal(std::vector<RectangleInfo>* result)
{
	// Passthrough
}

void MuRectangle::ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash)
{
	if (nodeHash == NodeHash::Color)
	{
		assert(!node->children.empty());
		fillColor = ExtractColor(node->children[0]->associatedTokens);
	}
}

void MuRectangle::BuildRenderInfosInternal(std::vector<RectangleInfo>* result)
{
	RectangleInfo rect = {
	    .position  = pos,
	    .size      = size,
	    .fillColor = fillColor,
	};

	result->push_back(rect);
}

MuImage::~MuImage()
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
		Texture2D* texture = (Texture2D*)imageInfo.rasterImage->data;
		UnloadTexture(*texture);
		delete texture;
#endif
		delete imageInfo.rasterImage;
	}
}

void MuImage::ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash)
{
	switch (nodeHash)
	{
		case NodeHash::Url:
		{
			assert(!node->children.empty());

			imageUrl = node->children[0]->name;
			std::filesystem::path fp(imageUrl);

			if (fp.extension().string() == "svg")
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
				Image* image                = new Image;
				*image                      = LoadImage(imageUrl.c_str());
				imageInfo.rasterImage->data = (void*)image;
#endif

				size.x = image->width;
				size.y = image->height;

				if (imageInfo.rasterImage->data == nullptr)
				{
					LOG_F(ERROR, "Cannot load image %s: %s", imageUrl.c_str(), stbi_failure_reason());
				}
			}
		}
		break;

		case NodeHash::FitMode:
		{
			std::string fit = node->children[0]->name.c_str();
			std::transform(fit.begin(), fit.end(), fit.begin(), ::tolower);

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
				      "%s is not a valid fit mode. Valid modes are Stretch, Fit or Crop",
				      node->children[0]->name.c_str());
			}
		}
		break;

		case NodeHash::Tint:
		{
			imageTint = ExtractColor(node->children[0]->associatedTokens);
		}
		break;
	}
}

void MuImage::PostEvaluate()
{
	if (imageInfo.isVectorial)
	{
		// TODO: This should be evaluated
		imageInfo.svgImage = nsvgParseFromFile(imageUrl.c_str(), "px", Min(size.x, size.y));
	}
	else
	{
		Image* image = (Image*)imageInfo.rasterImage->data;

		switch (fitMode)
		{
			case FitMode::Stretch:
			{
				ImageResize(image, (i32)size.x, (i32)size.y);
			}
			break;

			case FitMode::Fit:
			{
				i32 targetWidth, targetHeight;

				f32 imageRatio = (f32)image->width / image->height;
				if (size.x < size.y)
				{
					targetWidth  = (i32)size.x;
					targetHeight = size.x / imageRatio;
				}
				else
				{
					targetHeight = (i32)size.y;
					targetWidth  = size.y * imageRatio;
				}

				ImageResize(image, targetWidth, targetHeight);

				// Compute offsets to center in the target quad
				imageInfo.rasterImage->offsetX = (size.x - targetWidth) * 0.5f;
				imageInfo.rasterImage->offsetY = (size.y - targetHeight) * 0.5f;
			}
			break;

			case FitMode::Crop:
			{
				i32 targetWidth, targetHeight;

				f32 imageRatio = (f32)image->width / image->height;
				if (size.x > size.y)
				{
					targetWidth  = (i32)size.x;
					targetHeight = size.x / imageRatio;
				}
				else
				{
					targetHeight = (i32)size.y;
					targetWidth  = size.y * imageRatio;
				}

				ImageResize(image, targetWidth, targetHeight);

				Rectangle r = {(targetWidth - size.x) * 0.5f, (targetHeight - size.y) * 0.5f, size.x, size.y};
				ImageCrop(image, r);
			}
			break;
		}

		Texture2D* texture = new Texture2D;
		*texture           = LoadTextureFromImage(*image);

		imageInfo.rasterImage->data = (void*)texture;

		UnloadImage(*image);
		delete image;
	}
}

void MuImage::BuildRenderInfosInternal(std::vector<RectangleInfo>* result)
{
	RectangleInfo info = {};
	info.position      = pos;
	info.size          = size;
	info.fillColor     = imageTint;
	info.isImage       = true;
	info.imageInfo     = &imageInfo;
	result->push_back(std::move(info));
}

MuWidget* GetWidgetById(MuWidget* rootWidget, const std::string& name)
{
	if (rootWidget->id == name)
	{
		return rootWidget;
	}

	for (auto child : rootWidget->children)
	{
		if (auto node = GetWidgetById(child.get(), name); node != nullptr)
		{
			return node;
		}
	}

	return nullptr;
}

void BuildDependencyGraph(MuWidget* rootWidget, MuWidget* currentWidget)
{
	for (auto id : currentWidget->dependencyIds)
	{
		MuWidget* dep = id == "parent" ? currentWidget->parent : GetWidgetById(rootWidget, id);

		if (dep != nullptr)
		{
			currentWidget->dependencies.push_back(dep);
			dep->dependants.push_back(currentWidget);
		}
	}

	for (auto c : currentWidget->children)
	{
		BuildDependencyGraph(rootWidget, c.get());
	}
}

void BuildExpressionEvaluators(MuWidget* rootWidget, MuWidget* currentWidget)
{
	auto GetHashIndex = [](u32 hash)
	{
		switch (hash)
		{
			case NodeHash::X:
				return 0;
			case NodeHash::Y:
				return 1;
			case NodeHash::Width:
				return 2;
			case NodeHash::Height:
				return 3;

			default:
				assert(false);
				return -1;
		}
	};

	auto GetPropertyPtr = [&currentWidget](u32 hash) -> f32*
	{
		switch (hash)
		{
			case NodeHash::X:
				return &currentWidget->pos.x;
			case NodeHash::Y:
				return &currentWidget->pos.y;
			case NodeHash::Width:
				return &currentWidget->size.x;
			case NodeHash::Height:
				return &currentWidget->size.y;

			default:
				assert(false);
				return nullptr;
		}
	};

	std::unordered_set<u32> remainingAttributes = {NodeHash::X, NodeHash::Y, NodeHash::Width, NodeHash::Height};
	for (auto expr : currentWidget->geometryExpressions)
	{
		remainingAttributes.erase(expr.first);

		auto expression = ShuntingYard::Expression::Build(expr.second, rootWidget, currentWidget);

		currentWidget->evaluators.push_back(std::make_pair(GetPropertyPtr(expr.first), expression));
	}

	for (auto c : currentWidget->children)
	{
		BuildExpressionEvaluators(rootWidget, c.get());
	}
}
