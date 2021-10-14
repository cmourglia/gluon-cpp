#pragma once

#include <Beard/Macros.h>

START_EXTERNAL_INCLUDE
#include <glm/glm.hpp>
END_EXTERNAL_INCLUDE

#include <string_view>
#include <unordered_map>

namespace GluonColor
{
inline glm::vec4 from_rgba(i32 r, i32 g, i32 b, f32 a = 1.0f)
{
	const f32 fr = Beard::Clamp(static_cast<f32>(r), 0.0f, 255.0f) / 255.0f;
	const f32 fg = Beard::Clamp(static_cast<f32>(g), 0.0f, 255.0f) / 255.0f;
	const f32 fb = Beard::Clamp(static_cast<f32>(b), 0.0f, 255.0f) / 255.0f;
	const f32 fa = Beard::Clamp(a, 0.0f, 1.0f);

	return {fr, fg, fb, fa};
}

inline glm::vec4 from_hsla(i32 h, i32 s, i32 l, f32 a = 1.0f)
{
	const f32 fh = Beard::Clamp(static_cast<f32>(h), 0.0f, 360.0f);
	const f32 fs = Beard::Clamp(static_cast<f32>(s), 0.0f, 100.0f) / 100.0f;
	const f32 fl = Beard::Clamp(static_cast<f32>(l), 0.0f, 100.0f) / 100.0f;
	const f32 fa = Beard::Clamp(a, 0.0f, 1.0f);

	const f32 c = (1.0f - abs(2.0f * fl - 1.0f)) * fs;
	const f32 x = c * (1.0f - abs(fmodf(fh / 60.0f, 2.0f) - 1.0f));
	const f32 m = fl - c * 0.5f;

	const glm::vec3 cprime = h < 60    ? glm::vec3{c, x, 0.0f}
	                         : h < 120 ? glm::vec3{x, c, 0.0f}
	                         : h < 180 ? glm::vec3{0.0f, c, x}
	                         : h < 240 ? glm::vec3{0.0f, x, c}
	                         : h < 300 ? glm::vec3{x, 0.0f, c}
	                                   : glm::vec3{c, 0.0f, x};

	return {cprime.r + m, cprime.g + m, cprime.b + m, fa};
}

inline glm::vec4 from_string(const std::string& str)
{
	std::string value = str;
	auto HexToFloat   = [](char* v) { return static_cast<f32>(strtol(v, nullptr, 16)) / 255.0f; };

	char s[3] = {};

	if (value[0] == '#')
	{
		value = value.substr(1);
	}

	switch (value.size())
	{
		case 2:
		{
			s[0]        = value[0];
			s[1]        = value[1];
			const f32 r = HexToFloat(s);
			return {r, r, r, 1.0f};
		}

		case 6:
		{
			s[0]        = value[0];
			s[1]        = value[1];
			const f32 r = HexToFloat(s);

			s[0]        = value[2];
			s[1]        = value[3];
			const f32 g = HexToFloat(s);

			s[0]        = value[4];
			s[1]        = value[5];
			const f32 b = HexToFloat(s);

			return {r, g, b, 1.0f};
		}

		case 8:
		{
			s[0]        = value[0];
			s[1]        = value[1];
			const f32 r = HexToFloat(s);

			s[0]        = value[2];
			s[1]        = value[3];
			const f32 g = HexToFloat(s);

			s[0]        = value[4];
			s[1]        = value[5];
			const f32 b = HexToFloat(s);

			s[0]        = value[6];
			s[1]        = value[7];
			const f32 a = HexToFloat(s);
			return {r, g, b, a};
		}

		default:
			break;
	}

	return {};
}

extern const glm::vec4 AliceBlue;
extern const glm::vec4 AntiqueWhite;
extern const glm::vec4 Aqua;
extern const glm::vec4 Aquamarine;
extern const glm::vec4 Azure;
extern const glm::vec4 Beige;
extern const glm::vec4 Bisque;
extern const glm::vec4 Black;
extern const glm::vec4 BlanchedAlmond;
extern const glm::vec4 Blue;
extern const glm::vec4 BlueViolet;
extern const glm::vec4 Brown;
extern const glm::vec4 BurlyWood;
extern const glm::vec4 CadetBlue;
extern const glm::vec4 Chartreuse;
extern const glm::vec4 Chocolate;
extern const glm::vec4 Coral;
extern const glm::vec4 CornflowerBlue;
extern const glm::vec4 Cornsilk;
extern const glm::vec4 Crimson;
extern const glm::vec4 Cyan;
extern const glm::vec4 DarkBlue;
extern const glm::vec4 DarkCyan;
extern const glm::vec4 DarkGoldenRod;
extern const glm::vec4 DarkGray;
extern const glm::vec4 DarkGrey;
extern const glm::vec4 DarkGreen;
extern const glm::vec4 DarkKhaki;
extern const glm::vec4 DarkMagenta;
extern const glm::vec4 DarkOliveGreen;
extern const glm::vec4 DarkOrange;
extern const glm::vec4 DarkOrchid;
extern const glm::vec4 DarkRed;
extern const glm::vec4 DarkSalmon;
extern const glm::vec4 DarkSeaGreen;
extern const glm::vec4 DarkSlateBlue;
extern const glm::vec4 DarkSlateGray;
extern const glm::vec4 DarkSlateGrey;
extern const glm::vec4 DarkTurquoise;
extern const glm::vec4 DarkViolet;
extern const glm::vec4 DeepPink;
extern const glm::vec4 DeepSkyBlue;
extern const glm::vec4 DimGray;
extern const glm::vec4 DimGrey;
extern const glm::vec4 DodgerBlue;
extern const glm::vec4 FireBrick;
extern const glm::vec4 FloralWhite;
extern const glm::vec4 ForestGreen;
extern const glm::vec4 Fuchsia;
extern const glm::vec4 Gainsboro;
extern const glm::vec4 GhostWhite;
extern const glm::vec4 Gold;
extern const glm::vec4 GoldenRod;
extern const glm::vec4 Gray;
extern const glm::vec4 Grey;
extern const glm::vec4 Green;
extern const glm::vec4 GreenYellow;
extern const glm::vec4 HoneyDew;
extern const glm::vec4 HotPink;
extern const glm::vec4 IndianRed;
extern const glm::vec4 Indigo;
extern const glm::vec4 Ivory;
extern const glm::vec4 Khaki;
extern const glm::vec4 Lavender;
extern const glm::vec4 LavenderBlush;
extern const glm::vec4 LawnGreen;
extern const glm::vec4 LemonChiffon;
extern const glm::vec4 LightBlue;
extern const glm::vec4 LightCoral;
extern const glm::vec4 LightCyan;
extern const glm::vec4 LightGoldenRodYellow;
extern const glm::vec4 LightGray;
extern const glm::vec4 LightGrey;
extern const glm::vec4 LightGreen;
extern const glm::vec4 LightPink;
extern const glm::vec4 LightSalmon;
extern const glm::vec4 LightSeaGreen;
extern const glm::vec4 LightSkyBlue;
extern const glm::vec4 LightSlateGray;
extern const glm::vec4 LightSlateGrey;
extern const glm::vec4 LightSteelBlue;
extern const glm::vec4 LightYellow;
extern const glm::vec4 Lime;
extern const glm::vec4 LimeGreen;
extern const glm::vec4 Linen;
extern const glm::vec4 Magenta;
extern const glm::vec4 Maroon;
extern const glm::vec4 MediumAquaMarine;
extern const glm::vec4 MediumBlue;
extern const glm::vec4 MediumOrchid;
extern const glm::vec4 MediumPurple;
extern const glm::vec4 MediumSeaGreen;
extern const glm::vec4 MediumSlateBlue;
extern const glm::vec4 MediumSpringGreen;
extern const glm::vec4 MediumTurquoise;
extern const glm::vec4 MediumVioletRed;
extern const glm::vec4 MidnightBlue;
extern const glm::vec4 MintCream;
extern const glm::vec4 MistyRose;
extern const glm::vec4 Moccasin;
extern const glm::vec4 NavajoWhite;
extern const glm::vec4 Navy;
extern const glm::vec4 OldLace;
extern const glm::vec4 Olive;
extern const glm::vec4 OliveDrab;
extern const glm::vec4 Orange;
extern const glm::vec4 OrangeRed;
extern const glm::vec4 Orchid;
extern const glm::vec4 PaleGoldenRod;
extern const glm::vec4 PaleGreen;
extern const glm::vec4 PaleTurquoise;
extern const glm::vec4 PaleVioletRed;
extern const glm::vec4 PapayaWhip;
extern const glm::vec4 PeachPuff;
extern const glm::vec4 Peru;
extern const glm::vec4 Pink;
extern const glm::vec4 Plum;
extern const glm::vec4 PowderBlue;
extern const glm::vec4 Purple;
extern const glm::vec4 RebeccaPurple;
extern const glm::vec4 Red;
extern const glm::vec4 RosyBrown;
extern const glm::vec4 RoyalBlue;
extern const glm::vec4 SaddleBrown;
extern const glm::vec4 Salmon;
extern const glm::vec4 SandyBrown;
extern const glm::vec4 SeaGreen;
extern const glm::vec4 SeaShell;
extern const glm::vec4 Sienna;
extern const glm::vec4 Silver;
extern const glm::vec4 SkyBlue;
extern const glm::vec4 SlateBlue;
extern const glm::vec4 SlateGray;
extern const glm::vec4 SlateGrey;
extern const glm::vec4 Snow;
extern const glm::vec4 SpringGreen;
extern const glm::vec4 SteelBlue;
extern const glm::vec4 Tan;
extern const glm::vec4 Teal;
extern const glm::vec4 Thistle;
extern const glm::vec4 Tomato;
extern const glm::vec4 Turquoise;
extern const glm::vec4 Violet;
extern const glm::vec4 Wheat;
extern const glm::vec4 White;
extern const glm::vec4 WhiteSmoke;
extern const glm::vec4 Yellow;
extern const glm::vec4 YellowGreen;

extern const std::unordered_map<std::string, glm::vec4> s_colors_by_name;
}

struct NSVGimage;
struct Image;
struct Texture;

struct RasterImage
{
	i32 width    = 0;
	i32 height   = 0;
	f32 offset_x = 0.0f;
	f32 offset_y = 0.0f;

	Image*   image   = nullptr;
	Texture* texture = nullptr;
};

struct ImageInfo
{
	bool         is_vectorial = false;
	NSVGimage*   svg_image    = nullptr;
	RasterImage* raster_image = nullptr;
};

struct RectangleInfo
{
	glm::vec2 position     = {0.0f, 0.0f};
	glm::vec2 size         = {0.0f, 0.0f};
	glm::vec4 fill_color   = {0.0f, 0.0f, 0.0f, 1.0f};
	f32       radius       = 0.0f;
	glm::vec4 border_color = {0.0f, 0.0f, 0.0f, 1.0f};
	f32       border_width = 0.0f;

	ImageInfo* image_info;

	bool is_image = false;
	// glm::vec2 dropShadowOffset   = {0.0f, 0.0f};
	// glm::vec4 dropShadowColor    = {0.3f, 0.3f, 0.3f, 1.0f};
	// f32       dropShadowScale    = 0.0f;
	// f32       dropShadowStrength = 15.0f;
};
