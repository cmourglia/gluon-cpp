#include <Beard/Hash.h>

enum class ENodeHash : u32
{
	ID      = Beard::Crc32::Hash("id"),
	X       = Beard::Crc32::Hash("x"),
	Y       = Beard::Crc32::Hash("y"),
	Width   = Beard::Crc32::Hash("width"),
	Height  = Beard::Crc32::Hash("height"),
	Anchors = Beard::Crc32::Hash("anchors"),
	Padding = Beard::Crc32::Hash("padding"),
	Margins = Beard::Crc32::Hash("margins"),

	// Window
	Title        = Beard::Crc32::Hash("title"),
	WindowWidth  = Beard::Crc32::Hash("windowWidth"),
	WindowHeight = Beard::Crc32::Hash("windowHeight"),

	// Rectangle
	Color = Beard::Crc32::Hash("color"),

	// Image
	Url     = Beard::Crc32::Hash("url"),
	FitMode = Beard::Crc32::Hash("fitMode"),
	Tint    = Beard::Crc32::Hash("tint"),
};