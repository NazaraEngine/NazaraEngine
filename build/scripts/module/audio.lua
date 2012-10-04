if (not _OPTIONS["united"]) then
	project "NazaraAudio"
end

files
{
 "../include/Nazara/Audio/**.hpp",
 "../include/Nazara/Audio/**.inl",
 "../src/Nazara/Audio/**.hpp",
 "../src/Nazara/Audio/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/Audio/Posix/*.hpp", "../src/Nazara/Audio/Posix/*.cpp" }
	links "OpenAL32"
	links "sndfile-1"
else
	excludes { "../src/Nazara/Audio/Win32/*.hpp", "../src/Nazara/Audio/Win32/*.cpp" }
	-- Link posix ?
end

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/Audio/Debug/Leaks.cpp"
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