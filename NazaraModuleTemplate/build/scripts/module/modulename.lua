if (not _OPTIONS["one-library"]) then
	project "NazaraModuleName"
end

files
{
	"../include/Nazara/ModuleName/**.hpp",
	"../include/Nazara/ModuleName/**.inl",
	"../src/Nazara/ModuleName/**.hpp",
	"../src/Nazara/ModuleName/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/ModuleName/Posix/*.hpp", "../src/Nazara/ModuleName/Posix/*.cpp" }
else
	excludes { "../src/Nazara/ModuleName/Win32/*.hpp", "../src/Nazara/ModuleName/Win32/*.cpp" }
end

if (_OPTIONS["one-library"]) then
	excludes "../src/Nazara/ModuleName/Debug/Leaks.cpp"
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
end