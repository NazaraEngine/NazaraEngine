if (not _OPTIONS["one-library"]) then
	project "NazaraRenderer"
end

files
{
	"../include/Nazara/Renderer/**.hpp",
	"../include/Nazara/Renderer/**.inl",
	"../src/Nazara/Renderer/**.hpp",
	"../src/Nazara/Renderer/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/Renderer/Posix/*.hpp", "../src/Nazara/Renderer/Posix/*.cpp" }
	links "gdi32"
	links "opengl32"
	links "winmm"
else
	excludes { "../src/Nazara/Renderer/Win32/*.hpp", "../src/Nazara/Renderer/Win32/*.cpp" }
end

if (_OPTIONS["one-library"]) then
	excludes "../src/Nazara/Renderer/Debug/Leaks.cpp"
else
	configuration "DebugStatic"
		links "NazaraCored-s"
		links "NazaraUtilityd-s"
		targetname "NazaraRendererd"

	configuration "ReleaseStatic"
		links "NazaraCore-s"
		links "NazaraUtility-s"
		targetname "NazaraRenderer"

	configuration "DebugDLL"
		links "NazaraCored"
		links "NazaraUtilityd"
		targetname "NazaraRendererd"

	configuration "ReleaseDLL"
		links "NazaraCore"
		links "NazaraUtility"
		targetname "NazaraRenderer"
end