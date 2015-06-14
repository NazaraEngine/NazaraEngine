MODULE.Name = "Core"

MODULE.Files = { -- Les autres fichiers seront ajoutés automatiquement
	"../include/Nazara/Prerequesites.hpp",
	"../include/Nazara/Math/**.hpp",
	"../include/Nazara/Math/**.inl",
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/Core/Win32/**.hpp",
	"../src/Nazara/Core/Win32/**.cpp"
}

MODULE.OsFiles.Posix = {
	"../src/Nazara/Core/Posix/**.hpp",
	"../src/Nazara/Core/Posix/**.cpp"
}
