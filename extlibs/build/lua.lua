project "lualib"
language "C++"
kind "StaticLib"
targetname "lua-s"

files
{
	"../include/Lua/*.h",
	"../include/Lua/*.hpp",
	"../src/Lua/*.h",
	"../src/Lua/*.cpp"
}
