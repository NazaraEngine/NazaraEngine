project "NazaraUtility"

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
else
	excludes { "../src/Nazara/Utility/Win32/*.hpp", "../src/Nazara/Utility/Win32/*.cpp" }
end

configuration "DebugStatic"
	links "NazaraCored-s"
	targetname "NazaraUtilityd"

configuration "ReleaseStatic"
	links "NazaraCore-s"
	targetname "NazaraUtility"

configuration "DebugDLL"
	links "NazaraCored"
	targetname "NazaraUtilityd"

configuration "ReleaseDLL"
	links "NazaraCore"
	targetname "NazaraUtility"