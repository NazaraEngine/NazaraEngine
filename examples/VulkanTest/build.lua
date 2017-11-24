EXAMPLE.Name = "VulkanTest"

EXAMPLE.EnableConsole = true

EXAMPLE.OsDefines.Windows = {
	"VK_USE_PLATFORM_WIN32_KHR",
	"WIN32_LEAN_AND_MEAN",
	"NOMINMAX"
}

EXAMPLE.Files = {
	"main.cpp"
}

EXAMPLE.Libraries = {
	"NazaraCore",
	"NazaraPlatform",
	"NazaraVulkanRenderer",
	"NazaraUtility"
}
