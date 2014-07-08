if (not _OPTIONS["united"]) then
	project "NazaraRenderer"
end

defines "NAZARA_RENDERER_OPENGL"

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

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/Renderer/Debug/NewOverload.cpp"
else
	configuration "DebugStatic"
		links "NazaraCore-s-d"
		links "NazaraUtility-s-d"

	configuration "ReleaseStatic"
		links "NazaraCore-s"
		links "NazaraUtility-s"

	configuration "DebugDLL"
		links "NazaraCore-d"
		links "NazaraUtility-d"

	configuration "ReleaseDLL"
		links "NazaraCore"
		links "NazaraUtility"
end