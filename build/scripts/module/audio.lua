project "NazaraAudio"

files
{
	"../include/Nazara/Audio/**.hpp",
	"../include/Nazara/Audio/**.inl",
	"../src/Nazara/Audio/**.hpp",
	"../src/Nazara/Audio/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/Audio/Posix/*.hpp", "../src/Nazara/Audio/Posix/*.cpp" }
else
	excludes { "../src/Nazara/Audio/Win32/*.hpp", "../src/Nazara/Audio/Win32/*.cpp" }
end

configuration "DebugStatic"
	links "NazaraCored-s"
	targetname "NazaraAudiod"

configuration "ReleaseStatic"
	links "NazaraCore-s"
	targetname "NazaraAudio"

configuration "DebugDLL"
	links "NazaraCored"
	targetname "NazaraAudiod"

configuration "ReleaseDLL"
	links "NazaraCore"
	targetname "NazaraAudio"