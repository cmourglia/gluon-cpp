#pragma once

#include "defines.h"

#include <glm/glm.hpp>

#include <string_view>

namespace Color
{
inline glm::vec4 FromRgba(i32 r, i32 g, i32 b, f32 a = 1.0f)
{
	const f32 fr = Clamp(r, 0, 255) / 255.0f;
	const f32 fg = Clamp(g, 0, 255) / 255.0f;
	const f32 fb = Clamp(b, 0, 255) / 255.0f;
	const f32 fa = Clamp(a, 0.0f, 1.0f);

	return {fr, fg, fb, fa};
}

inline glm::vec4 FromHsla(i32 h, i32 s, i32 l, f32 a = 1.0f)
{
	const f32 fh = Clamp(h, 0, 360);
	const f32 fs = Clamp(s, 0, 100) / 100.0f;
	const f32 fl = Clamp(l, 0, 100) / 100.0f;
	const f32 fa = Clamp(a, 0.0f, 1.0f);

	const f32 c = (1.0f - Abs(2.0f * fl - 1.0f)) * fs;
	const f32 x = c * (1.0f - Abs(fmodf(fh / 60.0f, 2.0f) - 1.0f));
	const f32 m = fl - c * 0.5f;

	const glm::vec3 cprime = h < 60.0f    ? glm::vec3(c, x, 0.0f)
	                         : h < 120.0f ? glm::vec3(x, c, 0.0f)
	                         : h < 180.0f ? glm::vec3(0.0f, c, x)
	                         : h < 240.0f ? glm::vec3(0.0f, x, c)
	                         : h < 300.0f ? glm::vec3(x, 0.0f, c)
	                                      : glm::vec3(c, 0.0f, x);

	return {cprime.r + m, cprime.g + m, cprime.b + m, a};
}

inline glm::vec4 FromString(std::string_view value)
{
	auto HexToFloat = [](char* v) { return strtol(v, nullptr, 16) / 255.0f; };

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

extern glm::vec4 AliceBlue;
extern glm::vec4 AntiqueWhite;
extern glm::vec4 Aqua;
extern glm::vec4 Aquamarine;
extern glm::vec4 Azure;
extern glm::vec4 Beige;
extern glm::vec4 Bisque;
extern glm::vec4 Black;
extern glm::vec4 BlanchedAlmond;
extern glm::vec4 Blue;
extern glm::vec4 BlueViolet;
extern glm::vec4 Brown;
extern glm::vec4 BurlyWood;
extern glm::vec4 CadetBlue;
extern glm::vec4 Chartreuse;
extern glm::vec4 Chocolate;
extern glm::vec4 Coral;
extern glm::vec4 CornflowerBlue;
extern glm::vec4 Cornsilk;
extern glm::vec4 Crimson;
extern glm::vec4 Cyan;
extern glm::vec4 DarkBlue;
extern glm::vec4 DarkCyan;
extern glm::vec4 DarkGoldenRod;
extern glm::vec4 DarkGray;
extern glm::vec4 DarkGrey;
extern glm::vec4 DarkGreen;
extern glm::vec4 DarkKhaki;
extern glm::vec4 DarkMagenta;
extern glm::vec4 DarkOliveGreen;
extern glm::vec4 DarkOrange;
extern glm::vec4 DarkOrchid;
extern glm::vec4 DarkRed;
extern glm::vec4 DarkSalmon;
extern glm::vec4 DarkSeaGreen;
extern glm::vec4 DarkSlateBlue;
extern glm::vec4 DarkSlateGray;
extern glm::vec4 DarkSlateGrey;
extern glm::vec4 DarkTurquoise;
extern glm::vec4 DarkViolet;
extern glm::vec4 DeepPink;
extern glm::vec4 DeepSkyBlue;
extern glm::vec4 DimGray;
extern glm::vec4 DimGrey;
extern glm::vec4 DodgerBlue;
extern glm::vec4 FireBrick;
extern glm::vec4 FloralWhite;
extern glm::vec4 ForestGreen;
extern glm::vec4 Fuchsia;
extern glm::vec4 Gainsboro;
extern glm::vec4 GhostWhite;
extern glm::vec4 Gold;
extern glm::vec4 GoldenRod;
extern glm::vec4 Gray;
extern glm::vec4 Grey;
extern glm::vec4 Green;
extern glm::vec4 GreenYellow;
extern glm::vec4 HoneyDew;
extern glm::vec4 HotPink;
extern glm::vec4 IndianRed;
extern glm::vec4 Indigo;
extern glm::vec4 Ivory;
extern glm::vec4 Khaki;
extern glm::vec4 Lavender;
extern glm::vec4 LavenderBlush;
extern glm::vec4 LawnGreen;
extern glm::vec4 LemonChiffon;
extern glm::vec4 LightBlue;
extern glm::vec4 LightCoral;
extern glm::vec4 LightCyan;
extern glm::vec4 LightGoldenRodYellow;
extern glm::vec4 LightGray;
extern glm::vec4 LightGrey;
extern glm::vec4 LightGreen;
extern glm::vec4 LightPink;
extern glm::vec4 LightSalmon;
extern glm::vec4 LightSeaGreen;
extern glm::vec4 LightSkyBlue;
extern glm::vec4 LightSlateGray;
extern glm::vec4 LightSlateGrey;
extern glm::vec4 LightSteelBlue;
extern glm::vec4 LightYellow;
extern glm::vec4 Lime;
extern glm::vec4 LimeGreen;
extern glm::vec4 Linen;
extern glm::vec4 Magenta;
extern glm::vec4 Maroon;
extern glm::vec4 MediumAquaMarine;
extern glm::vec4 MediumBlue;
extern glm::vec4 MediumOrchid;
extern glm::vec4 MediumPurple;
extern glm::vec4 MediumSeaGreen;
extern glm::vec4 MediumSlateBlue;
extern glm::vec4 MediumSpringGreen;
extern glm::vec4 MediumTurquoise;
extern glm::vec4 MediumVioletRed;
extern glm::vec4 MidnightBlue;
extern glm::vec4 MintCream;
extern glm::vec4 MistyRose;
extern glm::vec4 Moccasin;
extern glm::vec4 NavajoWhite;
extern glm::vec4 Navy;
extern glm::vec4 OldLace;
extern glm::vec4 Olive;
extern glm::vec4 OliveDrab;
extern glm::vec4 Orange;
extern glm::vec4 OrangeRed;
extern glm::vec4 Orchid;
extern glm::vec4 PaleGoldenRod;
extern glm::vec4 PaleGreen;
extern glm::vec4 PaleTurquoise;
extern glm::vec4 PaleVioletRed;
extern glm::vec4 PapayaWhip;
extern glm::vec4 PeachPuff;
extern glm::vec4 Peru;
extern glm::vec4 Pink;
extern glm::vec4 Plum;
extern glm::vec4 PowderBlue;
extern glm::vec4 Purple;
extern glm::vec4 RebeccaPurple;
extern glm::vec4 Red;
extern glm::vec4 RosyBrown;
extern glm::vec4 RoyalBlue;
extern glm::vec4 SaddleBrown;
extern glm::vec4 Salmon;
extern glm::vec4 SandyBrown;
extern glm::vec4 SeaGreen;
extern glm::vec4 SeaShell;
extern glm::vec4 Sienna;
extern glm::vec4 Silver;
extern glm::vec4 SkyBlue;
extern glm::vec4 SlateBlue;
extern glm::vec4 SlateGray;
extern glm::vec4 SlateGrey;
extern glm::vec4 Snow;
extern glm::vec4 SpringGreen;
extern glm::vec4 SteelBlue;
extern glm::vec4 Tan;
extern glm::vec4 Teal;
extern glm::vec4 Thistle;
extern glm::vec4 Tomato;
extern glm::vec4 Turquoise;
extern glm::vec4 Violet;
extern glm::vec4 Wheat;
extern glm::vec4 White;
extern glm::vec4 WhiteSmoke;
extern glm::vec4 Yellow;
extern glm::vec4 YellowGreen;
}

struct RectangleInfo
{
	glm::vec2 position           = {0.0f, 0.0f};
	glm::vec2 size               = {0.0f, 0.0f};
	glm::vec4 fillColor          = {0.0f, 0.0f, 0.0f, 1.0f};
	f32       radius             = 0.0f;
	glm::vec4 borderColor        = {0.0f, 0.0f, 0.0f, 1.0f};
	f32       borderWidth        = 0.0f;
	glm::vec2 dropShadowOffset   = {0.0f, 0.0f};
	glm::vec4 dropShadowColor    = {0.3f, 0.3f, 0.3f, 1.0f};
	f32       dropShadowScale    = 0.0f;
	f32       dropShadowStrength = 15.0f;
};
