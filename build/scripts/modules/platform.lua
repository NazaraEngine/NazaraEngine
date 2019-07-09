MODULE.Name = "Platform"

MODULE.ClientOnly = true

MODULE.Libraries = {
	"NazaraCore",
	"NazaraUtility"
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/Platform/Win32/**.hpp",
	"../src/Nazara/Platform/Win32/**.cpp"
}

MODULE.OsFiles.Posix = {
	"../src/Nazara/Platform/X11/**.hpp",
	"../src/Nazara/Platform/X11/**.cpp"
}

MODULE.OsLibraries.Windows = {
	"gdi32"
}

MODULE.OsLibraries.Posix = {
	"X11",
	"xcb",
	"xcb-cursor",
	"xcb-ewmh",
	"xcb-icccm",
	"xcb-keysyms",
	"xcb-randr"
}

