if (not _OPTIONS["united"]) then
	project "Nazara2D"
end

files
{
	"../include/Nazara/2D/**.hpp",
	"../include/Nazara/2D/**.inl",
	"../src/Nazara/2D/**.hpp",
	"../src/Nazara/2D/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/2D/Posix/*.hpp", "../src/Nazara/2D/Posix/*.cpp" }
else
	excludes { "../src/Nazara/2D/Win32/*.hpp", "../src/Nazara/2D/Win32/*.cpp" }
end

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/2D/Debug/Leaks.cpp"
else
	configuration "DebugStatic"
		links "NazaraCore-s-d"
		links "NazaraUtility-s-d"
		links "NazaraRenderer-s-d"

	configuration "ReleaseStatic"
		links "NazaraCore-s"
		links "NazaraUtility-s"
		links "NazaraRenderer-s"

	configuration "DebugDLL"
		links "NazaraCore-d"
		links "NazaraUtility-d"
		links "NazaraRenderer-d"

	configuration "ReleaseDLL"
		links "NazaraCore"
		links "NazaraUtility"
		links "NazaraRenderer"
end