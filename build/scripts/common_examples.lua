-- Configuration générale
configurations 
{
	"DebugStatic",
	"ReleaseStatic",
	"DebugDLL",
	"ReleaseDLL"
}

language "C++"
location("../examples/build/" .. _ACTION)

includedirs
{
	"../include"
}

debugdir "../examples/bin"
libdirs "../lib"
targetdir "../examples/bin"

configuration "Debug*"
	defines "NAZARA_DEBUG"
	flags "Symbols"

configuration "Release*"
	flags { "EnableSSE2", "Optimize", "OptimizeSpeed", "NoFramePointer", "NoRTTI" }

configuration "*Static"
	defines "NAZARA_STATIC"

configuration "gmake"
	buildoptions "-std=c++11"
