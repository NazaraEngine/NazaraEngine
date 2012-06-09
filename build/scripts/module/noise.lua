project "NazaraNoise"

files
{
 "../include/Nazara/Noise/**.hpp",
 "../include/Nazara/Noise/**.inl",
 "../src/Nazara/Noise/**.hpp",
 "../src/Nazara/Noise/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/Noise/Posix/*.hpp", "../src/Nazara/Noise/Posix/*.cpp" }
else
	excludes { "../src/Nazara/Noise/Win32/*.hpp", "../src/Nazara/Noise/Win32/*.cpp" }
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