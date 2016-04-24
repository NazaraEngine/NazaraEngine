MODULE.Name = "Vulkan"

MODULE.Defines = {
	"VK_NO_PROTOTYPES"
}

MODULE.Libraries = {
	"NazaraCore",
	"NazaraUtility"
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/Vulkan/Win32/**.hpp",
	"../src/Nazara/Vulkan/Win32/**.cpp"
}
