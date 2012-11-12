if (not _OPTIONS["united"]) then
	project "NazaraNetwork"
end

files
{
 "../include/Nazara/Network/**.hpp",
 "../include/Nazara/Network/**.inl",
 "../src/Nazara/Network/**.hpp",
 "../src/Nazara/Network/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/Network/Posix/*.hpp", "../src/Nazara/Network/Posix/*.cpp" }
	links "OpenAL32"
	links "sndfile-1"
	links "ws2_32"
	links "Mswsock"
else
	excludes { "../src/Nazara/Network/Win32/*.hpp", "../src/Nazara/Network/Win32/*.cpp" }
	-- Link posix ?
end

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/Network/Debug/Leaks.cpp"
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