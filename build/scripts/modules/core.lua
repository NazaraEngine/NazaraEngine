MODULE.Name = "Core"
MODULE.Excludable = false -- Excluding the core makes no sense as everything relies on it

MODULE.Files = { -- Other files will be automatically added
	"../include/Nazara/Prerequisites.hpp",
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

MODULE.OsLibraries.Posix = {
	"dl",
	"pthread"
}
