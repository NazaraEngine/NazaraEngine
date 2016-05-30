MODULE.Name = "Utility"

MODULE.Libraries = {
	"NazaraCore",
	"stb_image"
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/Utility/Win32/**.hpp",
	"../src/Nazara/Utility/Win32/**.cpp"
}

MODULE.OsFiles.Posix = {
	"../src/Nazara/Utility/X11/**.hpp",
	"../src/Nazara/Utility/X11/**.cpp"
}

MODULE.OsLibraries.Windows = {
	"freetype-s",
	"gdi32"
}

MODULE.OsLibraries.Posix = {
	"freetype",
	"X11",
	"xcb",
	"xcb-cursor",
	"xcb-ewmh",
	"xcb-icccm",
	"xcb-keysyms",
	"xcb-randr"
}

