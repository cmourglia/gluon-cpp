#include <beard/misc/hash.h>

enum class ENodeHash : u32
{
	ID      = beard::crc32::hash("id"),
	X       = beard::crc32::hash("x"),
	Y       = beard::crc32::hash("y"),
	Width   = beard::crc32::hash("width"),
	Height  = beard::crc32::hash("height"),
	Anchors = beard::crc32::hash("anchors"),
	Padding = beard::crc32::hash("padding"),
	Margins = beard::crc32::hash("margins"),

	// Window
	Title        = beard::crc32::hash("title"),
	WindowWidth  = beard::crc32::hash("windowWidth"),
	WindowHeight = beard::crc32::hash("windowHeight"),

	// Rectangle
	Color = beard::crc32::hash("color"),

	// Image
	Url     = beard::crc32::hash("url"),
	FitMode = beard::crc32::hash("fitMode"),
	Tint    = beard::crc32::hash("tint"),
};