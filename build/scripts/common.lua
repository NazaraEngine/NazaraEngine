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
location(_ACTION)

includedirs
{
	"../include",
	"../src/",
	"../extlibs/include"
}

libdirs "../lib"

if (_OPTIONS["x64"]) then
	libdirs "../extlibs/lib/x64"
end

libdirs "../extlibs/lib/x86"

targetdir "../lib"

configuration "Debug*"
	defines "NAZARA_DEBUG"
	flags "Symbols"

configuration "Release*"
	flags { "EnableSSE2", "Optimize", "OptimizeSpeed", "NoFramePointer", "NoRTTI" }

configuration "*Static"
	defines "NAZARA_STATIC"
	kind "StaticLib"
	targetsuffix "-s"

configuration "*DLL"
	kind "SharedLib"

configuration "codeblocks or codelite or gmake or xcode3*"
	buildoptions "-std=c++11"
	
configuration { "linux or bsd or macosx", "gmake" }
	buildoptions "-fvisibility=hidden"