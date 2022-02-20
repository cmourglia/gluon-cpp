#pragma once

#include <beard/containers/hash_map.h>
#include <beard/core/macros.h>
#include <beard/math/math.h>

#if 0
START_EXTERNAL_INCLUDE
#include <glm/glm.hpp>
END_EXTERNAL_INCLUDE

#include <string_view>

namespace color {
inline glm::vec4 FromRGBA(i32 r, i32 g, i32 b, f32 a = 1.0f) {
  const f32 fr = beard::clamp(static_cast<f32>(r), 0.0f, 255.0f) / 255.0f;
  const f32 fg = beard::clamp(static_cast<f32>(g), 0.0f, 255.0f) / 255.0f;
  const f32 fb = beard::clamp(static_cast<f32>(b), 0.0f, 255.0f) / 255.0f;
  const f32 fa = beard::clamp(a, 0.0f, 1.0f);

  return {fr, fg, fb, fa};
}

inline glm::vec4 FromHSLA(i32 h, i32 s, i32 l, f32 a = 1.0f) {
  const f32 fh = beard::clamp(static_cast<f32>(h), 0.0f, 360.0f);
  const f32 fs = beard::clamp(static_cast<f32>(s), 0.0f, 100.0f) / 100.0f;
  const f32 fl = beard::clamp(static_cast<f32>(l), 0.0f, 100.0f) / 100.0f;
  const f32 fa = beard::clamp(a, 0.0f, 1.0f);

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

inline glm::vec4 FromString(const std::string& str) {
  std::string value = str;
  auto HexToFloat = [](char* v) {
    return static_cast<f32>(strtol(v, nullptr, 16)) / 255.0f;
  };

  char s[3] = {};

  if (value[0] == '#') {
    value = value.substr(1);
  }

  switch (value.size()) {
    case 2: {
      s[0] = value[0];
      s[1] = value[1];
      const f32 r = HexToFloat(s);
      return {r, r, r, 1.0f};
    }

    case 6: {
      s[0] = value[0];
      s[1] = value[1];
      const f32 r = HexToFloat(s);

      s[0] = value[2];
      s[1] = value[3];
      const f32 g = HexToFloat(s);

      s[0] = value[4];
      s[1] = value[5];
      const f32 b = HexToFloat(s);

      return {r, g, b, 1.0f};
    }

    case 8: {
      s[0] = value[0];
      s[1] = value[1];
      const f32 r = HexToFloat(s);

      s[0] = value[2];
      s[1] = value[3];
      const f32 g = HexToFloat(s);

      s[0] = value[4];
      s[1] = value[5];
      const f32 b = HexToFloat(s);

      s[0] = value[6];
      s[1] = value[7];
      const f32 a = HexToFloat(s);
      return {r, g, b, a};
    }

    default:
      break;
  }

  return {};
}

extern const glm::vec4 kAliceBlue;
extern const glm::vec4 kAntiqueWhite;
extern const glm::vec4 kAqua;
extern const glm::vec4 kAquamarine;
extern const glm::vec4 kAzure;
extern const glm::vec4 kBeige;
extern const glm::vec4 kBisque;
extern const glm::vec4 kBlack;
extern const glm::vec4 kBlanchedAlmond;
extern const glm::vec4 kBlue;
extern const glm::vec4 kBlueViolet;
extern const glm::vec4 kBrown;
extern const glm::vec4 kBurlyWood;
extern const glm::vec4 kCadetBlue;
extern const glm::vec4 kChartreuse;
extern const glm::vec4 kChocolate;
extern const glm::vec4 kCoral;
extern const glm::vec4 kCornflowerBlue;
extern const glm::vec4 kCornsilk;
extern const glm::vec4 kCrimson;
extern const glm::vec4 kCyan;
extern const glm::vec4 kDarkBlue;
extern const glm::vec4 kDarkCyan;
extern const glm::vec4 kDarkGoldenRod;
extern const glm::vec4 kDarkGray;
extern const glm::vec4 kDarkGrey;
extern const glm::vec4 kDarkGreen;
extern const glm::vec4 kDarkKhaki;
extern const glm::vec4 kDarkMagenta;
extern const glm::vec4 kDarkOliveGreen;
extern const glm::vec4 kDarkOrange;
extern const glm::vec4 kDarkOrchid;
extern const glm::vec4 kDarkRed;
extern const glm::vec4 kDarkSalmon;
extern const glm::vec4 kDarkSeaGreen;
extern const glm::vec4 kDarkSlateBlue;
extern const glm::vec4 kDarkSlateGray;
extern const glm::vec4 kDarkSlateGrey;
extern const glm::vec4 kDarkTurquoise;
extern const glm::vec4 kDarkViolet;
extern const glm::vec4 kDeepPink;
extern const glm::vec4 kDeepSkyBlue;
extern const glm::vec4 kDimGray;
extern const glm::vec4 kDimGrey;
extern const glm::vec4 kDodgerBlue;
extern const glm::vec4 kFireBrick;
extern const glm::vec4 kFloralWhite;
extern const glm::vec4 kForestGreen;
extern const glm::vec4 kFuchsia;
extern const glm::vec4 kGainsboro;
extern const glm::vec4 kGhostWhite;
extern const glm::vec4 kGold;
extern const glm::vec4 kGoldenRod;
extern const glm::vec4 kGray;
extern const glm::vec4 kGrey;
extern const glm::vec4 kGreen;
extern const glm::vec4 kGreenYellow;
extern const glm::vec4 kHoneyDew;
extern const glm::vec4 kHotPink;
extern const glm::vec4 kIndianRed;
extern const glm::vec4 kIndigo;
extern const glm::vec4 kIvory;
extern const glm::vec4 kKhaki;
extern const glm::vec4 kLavender;
extern const glm::vec4 kLavenderBlush;
extern const glm::vec4 kLawnGreen;
extern const glm::vec4 kLemonChiffon;
extern const glm::vec4 kLightBlue;
extern const glm::vec4 kLightCoral;
extern const glm::vec4 kLightCyan;
extern const glm::vec4 kLightGoldenRodYellow;
extern const glm::vec4 kLightGray;
extern const glm::vec4 kLightGrey;
extern const glm::vec4 kLightGreen;
extern const glm::vec4 kLightPink;
extern const glm::vec4 kLightSalmon;
extern const glm::vec4 kLightSeaGreen;
extern const glm::vec4 kLightSkyBlue;
extern const glm::vec4 kLightSlateGray;
extern const glm::vec4 kLightSlateGrey;
extern const glm::vec4 kLightSteelBlue;
extern const glm::vec4 kLightYellow;
extern const glm::vec4 kLime;
extern const glm::vec4 kLimeGreen;
extern const glm::vec4 kLinen;
extern const glm::vec4 kMagenta;
extern const glm::vec4 kMaroon;
extern const glm::vec4 kMediumAquaMarine;
extern const glm::vec4 kMediumBlue;
extern const glm::vec4 kMediumOrchid;
extern const glm::vec4 kMediumPurple;
extern const glm::vec4 kMediumSeaGreen;
extern const glm::vec4 kMediumSlateBlue;
extern const glm::vec4 kMediumSpringGreen;
extern const glm::vec4 kMediumTurquoise;
extern const glm::vec4 kMediumVioletRed;
extern const glm::vec4 kMidnightBlue;
extern const glm::vec4 kMintCream;
extern const glm::vec4 kMistyRose;
extern const glm::vec4 kMoccasin;
extern const glm::vec4 kNavajoWhite;
extern const glm::vec4 kNavy;
extern const glm::vec4 kOldLace;
extern const glm::vec4 kOlive;
extern const glm::vec4 kOliveDrab;
extern const glm::vec4 kOrange;
extern const glm::vec4 kOrangeRed;
extern const glm::vec4 kOrchid;
extern const glm::vec4 kPaleGoldenRod;
extern const glm::vec4 kPaleGreen;
extern const glm::vec4 kPaleTurquoise;
extern const glm::vec4 kPaleVioletRed;
extern const glm::vec4 kPapayaWhip;
extern const glm::vec4 kPeachPuff;
extern const glm::vec4 kPeru;
extern const glm::vec4 kPink;
extern const glm::vec4 kPlum;
extern const glm::vec4 kPowderBlue;
extern const glm::vec4 kPurple;
extern const glm::vec4 kRebeccaPurple;
extern const glm::vec4 kRed;
extern const glm::vec4 kRosyBrown;
extern const glm::vec4 kRoyalBlue;
extern const glm::vec4 kSaddleBrown;
extern const glm::vec4 kSalmon;
extern const glm::vec4 kSandyBrown;
extern const glm::vec4 kSeaGreen;
extern const glm::vec4 kSeaShell;
extern const glm::vec4 kSienna;
extern const glm::vec4 kSilver;
extern const glm::vec4 kSkyBlue;
extern const glm::vec4 kSlateBlue;
extern const glm::vec4 kSlateGray;
extern const glm::vec4 kSlateGrey;
extern const glm::vec4 kSnow;
extern const glm::vec4 kSpringGreen;
extern const glm::vec4 kSteelBlue;
extern const glm::vec4 kTan;
extern const glm::vec4 kTeal;
extern const glm::vec4 kThistle;
extern const glm::vec4 kTomato;
extern const glm::vec4 kTurquoise;
extern const glm::vec4 kViolet;
extern const glm::vec4 kWheat;
extern const glm::vec4 kWhite;
extern const glm::vec4 kWhiteSmoke;
extern const glm::vec4 kYellow;
extern const glm::vec4 kYellowGreen;

extern const beard::string_hash_map<glm::vec4> kColorsByName;
}  // namespace color

struct NSVGimage;
struct SDL_Surface;

struct RasterImage {
  i32 widget = 0;
  i32 height = 0;
  f32 offset_x = 0.0f;
  f32 offset_y = 0.0f;
  SDL_Surface* image = nullptr;
};

struct ImageInfo {
  bool is_vectorial = false;
  NSVGimage* svg_image = nullptr;
  RasterImage* raster_image = nullptr;
};

struct RectangleInfo {
  glm::vec2 position = {0.0f, 0.0f};
  glm::vec2 size = {0.0f, 0.0f};
  glm::vec4 fill_color = {0.0f, 0.0f, 0.0f, 1.0f};
  f32 radius = 0.0f;
  glm::vec4 border_color = {0.0f, 0.0f, 0.0f, 1.0f};
  f32 border_width = 0.0f;

  ImageInfo* image_info = nullptr;

  bool is_image = false;
  // glm::vec2 dropShadowOffset   = {0.0f, 0.0f};
  // glm::vec4 dropShadowColor    = {0.3f, 0.3f, 0.3f, 1.0f};
  // f32       dropShadowScale    = 0.0f;
  // f32       dropShadowStrength = 15.0f;
};
#endif