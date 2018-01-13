LIBRARY.Name = "chipmunk"

LIBRARY.Defines = {"CP_USE_CGTYPES=0", "TARGET_OS_IPHONE=0", "TARGET_OS_MAC=0"}

LIBRARY.Language = "C++"

LIBRARY.Files = {
	"../thirdparty/include/chipmunk/*.h",
	"../thirdparty/src/chipmunk/*.h",
	"../thirdparty/src/chipmunk/*.c",
}
