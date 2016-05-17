MODULE.Name = "Audio"

MODULE.Defines = {
	"NAZARA_AUDIO_OPENAL"
}

MODULE.Libraries = {
	"NazaraCore",
	"sndfile-1"
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/Audio/Win32/**.hpp",
	"../src/Nazara/Audio/Win32/**.cpp"
}

MODULE.OsDynLib.Windows = {
	"soft_oal"
}

MODULE.OsFiles.Posix = {
	"../src/Nazara/Audio/Posix/**.hpp",
	"../src/Nazara/Audio/Posix/**.cpp"
}
