if (not _OPTIONS["united"]) then
	project "NazaraNoise"
end

files
{
	"../include/Nazara/Noise/**.hpp",
	"../include/Nazara/Noise/**.inl",
	"../src/Nazara/Noise/**.hpp",
	"../src/Nazara/Noise/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/Noise/Posix/*.hpp", "../src/Nazara/Noise/Posix/*.cpp" }
else
	excludes { "../src/Nazara/Noise/Win32/*.hpp", "../src/Nazara/Noise/Win32/*.cpp" }
end

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/Noise/Debug/Leaks.cpp"
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