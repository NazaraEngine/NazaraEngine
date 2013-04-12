if (not _OPTIONS["united"]) then
	project "NazaraGraphics"
end

files
{
	"../include/Nazara/Graphics/**.hpp",
	"../include/Nazara/Graphics/**.inl",
	"../src/Nazara/Graphics/**.hpp",
	"../src/Nazara/Graphics/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/Graphics/Posix/*.hpp", "../src/Nazara/Graphics/Posix/*.cpp" }
else
	excludes { "../src/Nazara/Graphics/Win32/*.hpp", "../src/Nazara/Graphics/Win32/*.cpp" }
end

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/Graphics/Debug/Leaks.cpp"
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
