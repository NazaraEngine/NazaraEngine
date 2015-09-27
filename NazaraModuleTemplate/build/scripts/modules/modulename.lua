MODULE.Name = "ModuleName"

MODULE.Libraries = {
	"NazaraCore"
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/ModuleName/Win32/**.hpp",
	"../src/Nazara/ModuleName/Win32/**.cpp"
}

MODULE.OsFiles.Posix = {
	"../src/Nazara/ModuleName/Posix/**.hpp",
	"../src/Nazara/ModuleName/Posix/**.cpp"
}
