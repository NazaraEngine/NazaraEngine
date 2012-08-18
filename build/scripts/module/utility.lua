if (not _OPTIONS["one-library"]) then
	project "NazaraUtility"
end

defines "STBI_NO_STDIO"

files
{
	"../include/Nazara/Utility/**.hpp",
	"../include/Nazara/Utility/**.inl",
	"../src/Nazara/Utility/**.hpp",
	"../src/Nazara/Utility/**.c",
	"../src/Nazara/Utility/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/Utility/Posix/*.hpp", "../src/Nazara/Utility/Posix/*.cpp" }
	links "gdi32"
else
	excludes { "../src/Nazara/Utility/Win32/*.hpp", "../src/Nazara/Utility/Win32/*.cpp" }
end

if (_OPTIONS["one-library"]) then
	excludes "../src/Nazara/Utility/Debug/Leaks.cpp"
else
	configuration "DebugStatic"
		links "NazaraCore-s-d"

	configuration "ReleaseStatic"
		links "NazaraCore-s"

	configuration "DebugDLL"
		links "NazaraCore-d"

	configuration "ReleaseDLL"
		links "NazaraCore"
end