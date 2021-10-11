#include <Gluon/Core/Utils.h>

namespace NodeHash
{
// Generic node names
static constexpr u32 ID      = Utils::crc32("id");
static constexpr u32 X       = Utils::crc32("x");
static constexpr u32 Y       = Utils::crc32("y");
static constexpr u32 Width   = Utils::crc32("width");
static constexpr u32 Height  = Utils::crc32("height");
static constexpr u32 Anchors = Utils::crc32("anchors");
static constexpr u32 Padding = Utils::crc32("padding");
static constexpr u32 Margins = Utils::crc32("margins");

// Window
static constexpr u32 Title        = Utils::crc32("title");
static constexpr u32 WindowWidth  = Utils::crc32("windowWidth");
static constexpr u32 WindowHeight = Utils::crc32("windowHeight");

// Rectangle
static constexpr u32 Color = Utils::crc32("color");

// Image
static constexpr u32 Url     = Utils::crc32("url");
static constexpr u32 FitMode = Utils::crc32("fitMode");
static constexpr u32 Tint    = Utils::crc32("tint");
}