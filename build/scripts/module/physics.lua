if (not _OPTIONS["united"]) then
	project "NazaraPhysics"
end

files
{
	"../include/Nazara/Physics/**.hpp",
	"../include/Nazara/Physics/**.inl",
	"../src/Nazara/Physics/**.hpp",
	"../src/Nazara/Physics/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/Physics/Posix/*.hpp", "../src/Nazara/Physics/Posix/*.cpp" }
else
	excludes { "../src/Nazara/Physics/Win32/*.hpp", "../src/Nazara/Physics/Win32/*.cpp" }
end

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/Physics/Debug/NewOverload.cpp"
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

configuration "Debug*"
	links "newton_d"

configuration "Release*"
	links "newton"
