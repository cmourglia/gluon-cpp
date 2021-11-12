#include <beard/misc/hash.h>

struct NodeHash
{
    enum Enum : u32
    {
        kId      = beard::crc32::hash("id"),
        kX       = beard::crc32::hash("x"),
        kY       = beard::crc32::hash("y"),
        kWidth   = beard::crc32::hash("width"),
        kHeight  = beard::crc32::hash("height"),
        kAnchors = beard::crc32::hash("anchors"),
        kPadding = beard::crc32::hash("padding"),
        kMargins = beard::crc32::hash("margins"),

        // Window
        kTitle        = beard::crc32::hash("title"),
        kWindowWidth  = beard::crc32::hash("windowWidth"),
        kWindowHeight = beard::crc32::hash("windowHeight"),

        // Rectangle
        kColor = beard::crc32::hash("color"),

        // Image
        kUrl     = beard::crc32::hash("url"),
        kFitMode = beard::crc32::hash("fitMode"),
        kTint    = beard::crc32::hash("tint"),
    };
};