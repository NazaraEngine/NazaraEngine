project "NazaraNoise"

files
{
	"../include/Nazara/Noise/**.hpp",
	"../include/Nazara/Noise/**.inl",
	"../src/Nazara/Noise/**.hpp",
	"../src/Nazara/Noise/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/ModuleName/Posix/*.hpp", "../src/Nazara/ModuleName/Posix/*.cpp" }
else
	excludes { "../src/Nazara/ModuleName/Win32/*.hpp", "../src/Nazara/ModuleName/Win32/*.cpp" }
end

configuration "DebugStatic"
	links "NazaraCored-s"
	targetname "NazaraNoised"

configuration "ReleaseStatic"
	links "NazaraCore-s"
	targetname "NazaraNoise"

configuration "DebugDLL"
	links "NazaraCored"
	targetname "NazaraNoised"

configuration "ReleaseDLL"
	links "NazaraCore"
	targetname "NazaraNoise"