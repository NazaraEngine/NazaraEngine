LIBRARY.Name = "chipmunk"

LIBRARY.Defines = {
	"CP_USE_CGTYPES=0",
	"DRAW_CLOSEST=0",
	"DRAW_EPA=0",
	"DRAW_GJK=0",
	"TARGET_IPHONE_SIMULATOR=0",
	"TARGET_OS_IPHONE=0",
	"TARGET_OS_MAC=0",
}

LIBRARY.Language = "C++"

LIBRARY.Files = {
	"../thirdparty/include/chipmunk/*.h",
	"../thirdparty/src/chipmunk/*.h",
	"../thirdparty/src/chipmunk/*.c",
}
