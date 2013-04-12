-- Configuration générale
configurations 
{
	"Debug",
	"Release"
}

location("../extlibs/build/" .. _ACTION)
includedirs "../extlibs/include"

if (_OPTIONS["x64"]) then
	targetdir "../extlibs/lib/x64"
else
	targetdir "../extlibs/lib/x86"
end

configuration "Debug"
	flags "Symbols"
	targetsuffix "-d"

configuration "Release"
	flags { "EnableSSE", "EnableSSE2", "Optimize", "OptimizeSpeed", "NoFramePointer", "NoRTTI" }

-- Activation du SSE côté GCC
configuration { "Release*", "codeblocks or codelite or gmake or xcode3*" }
	buildoptions "-mfpmath=sse"
