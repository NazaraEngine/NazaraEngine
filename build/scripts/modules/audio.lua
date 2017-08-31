MODULE.Name = "Audio"

MODULE.ClientOnly = true

MODULE.Defines = {
	"NAZARA_AUDIO_OPENAL"
}

MODULE.Libraries = {
	"NazaraCore"
}

MODULE.OsLibraries.Windows = {
	"sndfile-1"
}

MODULE.OsLibraries.Posix = {
	"sndfile"
}

MODULE.OsDynLib.Windows = {
	"soft_oal"
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/Audio/Win32/**.hpp",
	"../src/Nazara/Audio/Win32/**.cpp"
}

MODULE.OsFiles.Posix = {
	"../src/Nazara/Audio/Posix/**.hpp",
	"../src/Nazara/Audio/Posix/**.cpp"
}
