MODULE.Name = "Vulkan"

MODULE.Defines = {
	"VK_NO_PROTOTYPES"
}

MODULE.Libraries = {
	"NazaraCore",
	"NazaraUtility"
}

MODULE.OsDefines.Windows = {
	"VK_USE_PLATFORM_WIN32_KHR"
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/Vulkan/Win32/**.hpp",
	"../src/Nazara/Vulkan/Win32/**.cpp"
}
