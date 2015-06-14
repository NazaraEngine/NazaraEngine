MODULE.Name = "Utility"

MODULE.Libraries = {
	"freetype-s",
	"NazaraCore",
	"stb_image"
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/Utility/Win32/**.hpp",
	"../src/Nazara/Utility/Win32/**.cpp"
}

MODULE.OsFiles.Posix = {
	"../src/Nazara/Utility/Posix/**.hpp",
	"../src/Nazara/Utility/Posix/**.cpp"
}

MODULE.OsLibraries.Windows = {
	"gdi32"
}

