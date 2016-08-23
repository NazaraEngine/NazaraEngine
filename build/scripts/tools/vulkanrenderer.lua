TOOL.Name = "VulkanRenderer"

TOOL.ClientOnly = true

TOOL.Kind = "Library"
TOOL.TargetDirectory = "../lib"

TOOL.Defines = {
	"NAZARA_BUILD",
	"NAZARA_VULKANRENDERER_BUILD"
}

TOOL.Includes = {
	"../include",
	"../src/",
	"../extlibs/include"
}

TOOL.Files = {
	"../include/Nazara/VulkanRenderer/**.hpp",
	"../include/Nazara/VulkanRenderer/**.inl",
	"../src/Nazara/VulkanRenderer/**.hpp",
	"../src/Nazara/VulkanRenderer/**.inl",
	"../src/Nazara/VulkanRenderer/**.cpp"
}

TOOL.Libraries = {
	"NazaraCore",
	"NazaraRenderer",
	"NazaraUtility"
}

TOOL.OsDefines.Linux = {
--	"VK_USE_PLATFORM_MIR_KHR",
	"VK_USE_PLATFORM_XCB_KHR"
--	"VK_USE_PLATFORM_XLIB_KHR",
--	"VK_USE_PLATFORM_WAYLAND_KHR"
}

TOOL.OsDefines.BSD = TOOL.OsDefines.Linux
TOOL.OsDefines.Solaris = TOOL.OsDefines.Linux

TOOL.OsDefines.Windows = {
	"VK_USE_PLATFORM_WIN32_KHR"
}

TOOL.OsFiles.Windows = {
	"../src/Nazara/VulkanRenderer/Win32/**.hpp",
	"../src/Nazara/VulkanRenderer/Win32/**.cpp"
}
