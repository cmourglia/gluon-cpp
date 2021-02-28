#pragma once

#include "defines.h"

#include <glm/glm.hpp>

#include <string_view>

namespace Color
{
inline constexpr glm::vec4 FromRgba(i32 r, i32 g, i32 b, f32 a = 1.0f)
{
	const f32 fr = Clamp(r, 0, 255) / 255.0f;
	const f32 fg = Clamp(g, 0, 255) / 255.0f;
	const f32 fb = Clamp(b, 0, 255) / 255.0f;
	const f32 fa = Clamp(a, 0.0f, 1.0f);

	return {fr, fg, fb, fa};
}

inline constexpr glm::vec4 FromHsla(i32 h, i32 s, i32 l, f32 a = 1.0f)
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

inline constexpr glm::vec4 FromString(std::string_view value)
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

constexpr glm::vec4 AliceBlue            = FromRgba(240, 248, 255);
constexpr glm::vec4 AntiqueWhite         = FromRgba(250, 235, 215);
constexpr glm::vec4 Aqua                 = FromRgba(0, 255, 255);
constexpr glm::vec4 Aquamarine           = FromRgba(127, 255, 212);
constexpr glm::vec4 Azure                = FromRgba(240, 255, 255);
constexpr glm::vec4 Beige                = FromRgba(245, 245, 220);
constexpr glm::vec4 Bisque               = FromRgba(255, 228, 196);
constexpr glm::vec4 Black                = FromRgba(0, 0, 0);
constexpr glm::vec4 BlanchedAlmond       = FromRgba(255, 235, 205);
constexpr glm::vec4 Blue                 = FromRgba(0, 0, 255);
constexpr glm::vec4 BlueViolet           = FromRgba(138, 43, 226);
constexpr glm::vec4 Brown                = FromRgba(165, 42, 42);
constexpr glm::vec4 BurlyWood            = FromRgba(222, 184, 135);
constexpr glm::vec4 CadetBlue            = FromRgba(95, 158, 160);
constexpr glm::vec4 Chartreuse           = FromRgba(127, 255, 0);
constexpr glm::vec4 Chocolate            = FromRgba(210, 105, 30);
constexpr glm::vec4 Coral                = FromRgba(255, 127, 80);
constexpr glm::vec4 CornflowerBlue       = FromRgba(100, 149, 237);
constexpr glm::vec4 Cornsilk             = FromRgba(255, 248, 220);
constexpr glm::vec4 Crimson              = FromRgba(220, 20, 60);
constexpr glm::vec4 Cyan                 = FromRgba(0, 255, 255);
constexpr glm::vec4 DarkBlue             = FromRgba(0, 0, 139);
constexpr glm::vec4 DarkCyan             = FromRgba(0, 139, 139);
constexpr glm::vec4 DarkGoldenRod        = FromRgba(184, 134, 11);
constexpr glm::vec4 DarkGray             = FromRgba(169, 169, 169);
constexpr glm::vec4 DarkGrey             = FromRgba(169, 169, 169);
constexpr glm::vec4 DarkGreen            = FromRgba(0, 100, 0);
constexpr glm::vec4 DarkKhaki            = FromRgba(189, 183, 107);
constexpr glm::vec4 DarkMagenta          = FromRgba(139, 0, 139);
constexpr glm::vec4 DarkOliveGreen       = FromRgba(85, 107, 47);
constexpr glm::vec4 DarkOrange           = FromRgba(255, 140, 0);
constexpr glm::vec4 DarkOrchid           = FromRgba(153, 50, 204);
constexpr glm::vec4 DarkRed              = FromRgba(139, 0, 0);
constexpr glm::vec4 DarkSalmon           = FromRgba(233, 150, 122);
constexpr glm::vec4 DarkSeaGreen         = FromRgba(143, 188, 143);
constexpr glm::vec4 DarkSlateBlue        = FromRgba(72, 61, 139);
constexpr glm::vec4 DarkSlateGray        = FromRgba(47, 79, 79);
constexpr glm::vec4 DarkSlateGrey        = FromRgba(47, 79, 79);
constexpr glm::vec4 DarkTurquoise        = FromRgba(0, 206, 209);
constexpr glm::vec4 DarkViolet           = FromRgba(148, 0, 211);
constexpr glm::vec4 DeepPink             = FromRgba(255, 20, 147);
constexpr glm::vec4 DeepSkyBlue          = FromRgba(0, 191, 255);
constexpr glm::vec4 DimGray              = FromRgba(105, 105, 105);
constexpr glm::vec4 DimGrey              = FromRgba(105, 105, 105);
constexpr glm::vec4 DodgerBlue           = FromRgba(30, 144, 255);
constexpr glm::vec4 FireBrick            = FromRgba(178, 34, 34);
constexpr glm::vec4 FloralWhite          = FromRgba(255, 250, 240);
constexpr glm::vec4 ForestGreen          = FromRgba(34, 139, 34);
constexpr glm::vec4 Fuchsia              = FromRgba(255, 0, 255);
constexpr glm::vec4 Gainsboro            = FromRgba(220, 220, 220);
constexpr glm::vec4 GhostWhite           = FromRgba(248, 248, 255);
constexpr glm::vec4 Gold                 = FromRgba(255, 215, 0);
constexpr glm::vec4 GoldenRod            = FromRgba(218, 165, 32);
constexpr glm::vec4 Gray                 = FromRgba(128, 128, 128);
constexpr glm::vec4 Grey                 = FromRgba(128, 128, 128);
constexpr glm::vec4 Green                = FromRgba(0, 128, 0);
constexpr glm::vec4 GreenYellow          = FromRgba(173, 255, 47);
constexpr glm::vec4 HoneyDew             = FromRgba(240, 255, 240);
constexpr glm::vec4 HotPink              = FromRgba(255, 105, 180);
constexpr glm::vec4 IndianRed            = FromRgba(205, 92, 92);
constexpr glm::vec4 Indigo               = FromRgba(75, 0, 130);
constexpr glm::vec4 Ivory                = FromRgba(255, 255, 240);
constexpr glm::vec4 Khaki                = FromRgba(240, 230, 140);
constexpr glm::vec4 Lavender             = FromRgba(230, 230, 250);
constexpr glm::vec4 LavenderBlush        = FromRgba(255, 240, 245);
constexpr glm::vec4 LawnGreen            = FromRgba(124, 252, 0);
constexpr glm::vec4 LemonChiffon         = FromRgba(255, 250, 205);
constexpr glm::vec4 LightBlue            = FromRgba(173, 216, 230);
constexpr glm::vec4 LightCoral           = FromRgba(240, 128, 128);
constexpr glm::vec4 LightCyan            = FromRgba(224, 255, 255);
constexpr glm::vec4 LightGoldenRodYellow = FromRgba(250, 250, 210);
constexpr glm::vec4 LightGray            = FromRgba(211, 211, 211);
constexpr glm::vec4 LightGrey            = FromRgba(211, 211, 211);
constexpr glm::vec4 LightGreen           = FromRgba(144, 238, 144);
constexpr glm::vec4 LightPink            = FromRgba(255, 182, 193);
constexpr glm::vec4 LightSalmon          = FromRgba(255, 160, 122);
constexpr glm::vec4 LightSeaGreen        = FromRgba(32, 178, 170);
constexpr glm::vec4 LightSkyBlue         = FromRgba(135, 206, 250);
constexpr glm::vec4 LightSlateGray       = FromRgba(119, 136, 153);
constexpr glm::vec4 LightSlateGrey       = FromRgba(119, 136, 153);
constexpr glm::vec4 LightSteelBlue       = FromRgba(176, 196, 222);
constexpr glm::vec4 LightYellow          = FromRgba(255, 255, 224);
constexpr glm::vec4 Lime                 = FromRgba(0, 255, 0);
constexpr glm::vec4 LimeGreen            = FromRgba(50, 205, 50);
constexpr glm::vec4 Linen                = FromRgba(250, 240, 230);
constexpr glm::vec4 Magenta              = FromRgba(255, 0, 255);
constexpr glm::vec4 Maroon               = FromRgba(128, 0, 0);
constexpr glm::vec4 MediumAquaMarine     = FromRgba(102, 205, 170);
constexpr glm::vec4 MediumBlue           = FromRgba(0, 0, 205);
constexpr glm::vec4 MediumOrchid         = FromRgba(186, 85, 211);
constexpr glm::vec4 MediumPurple         = FromRgba(147, 112, 219);
constexpr glm::vec4 MediumSeaGreen       = FromRgba(60, 179, 113);
constexpr glm::vec4 MediumSlateBlue      = FromRgba(123, 104, 238);
constexpr glm::vec4 MediumSpringGreen    = FromRgba(0, 250, 154);
constexpr glm::vec4 MediumTurquoise      = FromRgba(72, 209, 204);
constexpr glm::vec4 MediumVioletRed      = FromRgba(199, 21, 133);
constexpr glm::vec4 MidnightBlue         = FromRgba(25, 25, 112);
constexpr glm::vec4 MintCream            = FromRgba(245, 255, 250);
constexpr glm::vec4 MistyRose            = FromRgba(255, 228, 225);
constexpr glm::vec4 Moccasin             = FromRgba(255, 228, 181);
constexpr glm::vec4 NavajoWhite          = FromRgba(255, 222, 173);
constexpr glm::vec4 Navy                 = FromRgba(0, 0, 128);
constexpr glm::vec4 OldLace              = FromRgba(253, 245, 230);
constexpr glm::vec4 Olive                = FromRgba(128, 128, 0);
constexpr glm::vec4 OliveDrab            = FromRgba(107, 142, 35);
constexpr glm::vec4 Orange               = FromRgba(255, 165, 0);
constexpr glm::vec4 OrangeRed            = FromRgba(255, 69, 0);
constexpr glm::vec4 Orchid               = FromRgba(218, 112, 214);
constexpr glm::vec4 PaleGoldenRod        = FromRgba(238, 232, 170);
constexpr glm::vec4 PaleGreen            = FromRgba(152, 251, 152);
constexpr glm::vec4 PaleTurquoise        = FromRgba(175, 238, 238);
constexpr glm::vec4 PaleVioletRed        = FromRgba(219, 112, 147);
constexpr glm::vec4 PapayaWhip           = FromRgba(255, 239, 213);
constexpr glm::vec4 PeachPuff            = FromRgba(255, 218, 185);
constexpr glm::vec4 Peru                 = FromRgba(205, 133, 63);
constexpr glm::vec4 Pink                 = FromRgba(255, 192, 203);
constexpr glm::vec4 Plum                 = FromRgba(221, 160, 221);
constexpr glm::vec4 PowderBlue           = FromRgba(176, 224, 230);
constexpr glm::vec4 Purple               = FromRgba(128, 0, 128);
constexpr glm::vec4 RebeccaPurple        = FromRgba(102, 51, 153);
constexpr glm::vec4 Red                  = FromRgba(255, 0, 0);
constexpr glm::vec4 RosyBrown            = FromRgba(188, 143, 143);
constexpr glm::vec4 RoyalBlue            = FromRgba(65, 105, 225);
constexpr glm::vec4 SaddleBrown          = FromRgba(139, 69, 19);
constexpr glm::vec4 Salmon               = FromRgba(250, 128, 114);
constexpr glm::vec4 SandyBrown           = FromRgba(244, 164, 96);
constexpr glm::vec4 SeaGreen             = FromRgba(46, 139, 87);
constexpr glm::vec4 SeaShell             = FromRgba(255, 245, 238);
constexpr glm::vec4 Sienna               = FromRgba(160, 82, 45);
constexpr glm::vec4 Silver               = FromRgba(192, 192, 192);
constexpr glm::vec4 SkyBlue              = FromRgba(135, 206, 235);
constexpr glm::vec4 SlateBlue            = FromRgba(106, 90, 205);
constexpr glm::vec4 SlateGray            = FromRgba(112, 128, 144);
constexpr glm::vec4 SlateGrey            = FromRgba(112, 128, 144);
constexpr glm::vec4 Snow                 = FromRgba(255, 250, 250);
constexpr glm::vec4 SpringGreen          = FromRgba(0, 255, 127);
constexpr glm::vec4 SteelBlue            = FromRgba(70, 130, 180);
constexpr glm::vec4 Tan                  = FromRgba(210, 180, 140);
constexpr glm::vec4 Teal                 = FromRgba(0, 128, 128);
constexpr glm::vec4 Thistle              = FromRgba(216, 191, 216);
constexpr glm::vec4 Tomato               = FromRgba(255, 99, 71);
constexpr glm::vec4 Turquoise            = FromRgba(64, 224, 208);
constexpr glm::vec4 Violet               = FromRgba(238, 130, 238);
constexpr glm::vec4 Wheat                = FromRgba(245, 222, 179);
constexpr glm::vec4 White                = FromRgba(255, 255, 255);
constexpr glm::vec4 WhiteSmoke           = FromRgba(245, 245, 245);
constexpr glm::vec4 Yellow               = FromRgba(255, 255, 0);
constexpr glm::vec4 YellowGreen          = FromRgba(154, 205, 50);
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
