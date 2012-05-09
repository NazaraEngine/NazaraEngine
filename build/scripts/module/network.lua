project "NazaraNetwork"

if (os.is("windows")) then
	links "ws2_32"
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
else
	excludes { "../src/Nazara/Network/Win32/*.hpp", "../src/Nazara/Network/Win32/*.cpp" }
end

configuration "DebugStatic"
	links "NazaraCored-s"
	targetname "NazaraNetworkd"

configuration "ReleaseStatic"
	links "NazaraCore-s"
	targetname "NazaraNetwork"

configuration "DebugDLL"
	links "NazaraCored"
	targetname "NazaraNetworkd"

configuration "ReleaseDLL"
	links "NazaraCore"
	targetname "NazaraNetwork"