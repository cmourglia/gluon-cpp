#include <Gluon/Core/Utils.h>

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
static constexpr u32 Title        = Utils::Crc32("title");
static constexpr u32 WindowWidth  = Utils::Crc32("windowWidth");
static constexpr u32 WindowHeight = Utils::Crc32("windowHeight");

// Rectangle
static constexpr u32 Color = Utils::Crc32("color");

// Image
static constexpr u32 Url     = Utils::Crc32("url");
static constexpr u32 FitMode = Utils::Crc32("fitMode");
static constexpr u32 Tint    = Utils::Crc32("tint");
}