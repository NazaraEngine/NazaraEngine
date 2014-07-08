if (not _OPTIONS["united"]) then
	project "NazaraLua"
end

files
{
	"../include/Nazara/Lua/**.hpp",
	"../include/Nazara/Lua/**.inl",
	"../src/Nazara/Lua/**.hpp",
	"../src/Nazara/Lua/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/Lua/Posix/*.hpp", "../src/Nazara/Lua/Posix/*.cpp" }
else
	excludes { "../src/Nazara/Lua/Win32/*.hpp", "../src/Nazara/Lua/Win32/*.cpp" }
end

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/Lua/Debug/NewOverload.cpp"
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
	links "lua-s-d"

configuration "Release*"
	links "lua-s"
