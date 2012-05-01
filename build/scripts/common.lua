-- Configuration générale
configurations 
{
	"DebugStatic",
	"ReleaseStatic",
	"DebugDLL",
	"ReleaseDLL"
}

defines "NAZARA_BUILD"
language "C++"
includedirs
{
	"../include", 
	"../src/"
}
libdirs "../lib"
targetdir "../lib"

configuration "Debug*"
	defines "NAZARA_DEBUG"
	flags "Symbols"

configuration "Release*"
	flags { "Optimize", "OptimizeSpeed" }

configuration "*Static"
	defines "NAZARA_STATIC"
	kind "StaticLib"
	targetsuffix "-s"

configuration "*DLL"
	kind "SharedLib"

configuration { "linux or bsd or macosx", "gmake" }
	buildoptions "-fvisibility=hidden"