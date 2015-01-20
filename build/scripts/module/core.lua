MODULE.Name = "Core"

MODULE.Files = {
	"../include/Nazara/Prerequesites.hpp",
	"../include/Nazara/Core/**.hpp",
	"../include/Nazara/Core/**.inl",
	"../include/Nazara/Math/**.hpp",
	"../include/Nazara/Math/**.inl",
	"../src/Nazara/Core/**.hpp",
	"../src/Nazara/Core/**.cpp"
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/Core/Win32/**.hpp",
	"../src/Nazara/Core/Win32/**.cpp"
}

MODULE.OsFiles.Posix = {
	"../src/Nazara/Core/Posix/**.hpp",
	"../src/Nazara/Core/Posix/**.cpp"
}
