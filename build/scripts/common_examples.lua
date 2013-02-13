-- Configuration générale
configurations 
{
--	"DebugStatic",
--	"ReleaseStatic",
	"DebugDLL",
	"ReleaseDLL"
}

language "C++"
location("../examples/build/" .. _ACTION)

debugdir "../examples/bin"

includedirs { "../include", "../extlibs/include" }

libdirs "../lib"

if (_OPTIONS["x64"]) then
	libdirs "../extlibs/lib/x64"
end

libdirs "../extlibs/lib/x86"

targetdir "../examples/bin"

configuration "Debug*"
	defines "NAZARA_DEBUG"
	flags "Symbols"

configuration "Release*"
	flags { "EnableSSE2", "Optimize", "OptimizeSpeed", "NoFramePointer", "NoRTTI" }

configuration "*Static"
	defines "NAZARA_STATIC"

configuration "codeblocks or codelite or gmake or xcode3*"
	buildoptions "-std=c++11"
