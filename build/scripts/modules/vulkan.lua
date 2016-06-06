MODULE.Name = "Vulkan"

MODULE.ClientOnly = true

MODULE.Defines = {
	"VK_NO_PROTOTYPES"
}

MODULE.Libraries = {
	"NazaraCore",
	"NazaraUtility"
}

MODULE.OsDefines.Linux = {
--	"VK_USE_PLATFORM_MIR_KHR",
	"VK_USE_PLATFORM_XCB_KHR"
--	"VK_USE_PLATFORM_XLIB_KHR",
--	"VK_USE_PLATFORM_WAYLAND_KHR"
}

MODULE.OsDefines.BSD = MODULE.OsDefines.Linux
MODULE.OsDefines.Solaris = MODULE.OsDefines.Linux

MODULE.OsDefines.Windows = {
	"VK_USE_PLATFORM_WIN32_KHR"
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/Vulkan/Win32/**.hpp",
	"../src/Nazara/Vulkan/Win32/**.cpp"
}
