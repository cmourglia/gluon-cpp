#include <Beard/Hash.h>

namespace NodeHash
{
// Generic node names
static constexpr u32 ID      = Beard::Crc32::Hash("id");
static constexpr u32 X       = Beard::Crc32::Hash("x");
static constexpr u32 Y       = Beard::Crc32::Hash("y");
static constexpr u32 Width   = Beard::Crc32::Hash("width");
static constexpr u32 Height  = Beard::Crc32::Hash("height");
static constexpr u32 Anchors = Beard::Crc32::Hash("anchors");
static constexpr u32 Padding = Beard::Crc32::Hash("padding");
static constexpr u32 Margins = Beard::Crc32::Hash("margins");

// Window
static constexpr u32 Title        = Beard::Crc32::Hash("title");
static constexpr u32 WindowWidth  = Beard::Crc32::Hash("windowWidth");
static constexpr u32 WindowHeight = Beard::Crc32::Hash("windowHeight");

// Rectangle
static constexpr u32 Color = Beard::Crc32::Hash("color");

// Image
static constexpr u32 Url     = Beard::Crc32::Hash("url");
static constexpr u32 FitMode = Beard::Crc32::Hash("fitMode");
static constexpr u32 Tint    = Beard::Crc32::Hash("tint");
}