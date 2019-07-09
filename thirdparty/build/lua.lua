LIBRARY.Name = "lua"

LIBRARY.Language = "C++" -- Compile as C++ to make Lua use exceptions instead of SJLJ

LIBRARY.Files = {
	"../thirdparty/include/Lua/*.h",
	"../thirdparty/include/Lua/*.hpp",
	"../thirdparty/src/Lua/*.h",
	"../thirdparty/src/Lua/*.cpp"
}
