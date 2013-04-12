project "lualib"
language "C"
kind "StaticLib"
targetname "lua-s"

includedirs "../extlibs/include/Lua"

files
{
	"../include/Lua/*.h",
	"../include/Lua/*.hpp",
	"../src/Lua/*.h",
	"../src/Lua/*.c"
}
