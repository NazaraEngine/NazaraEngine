MODULE.Name = "Physics3D"

MODULE.Defines = {
	"_NEWTON_STATIC_LIB"
}

MODULE.OsDefines.Windows = {
	"_WINDOWS"
}

MODULE.Libraries = {
	"NazaraCore",
	"newton" -- Newton Game Dynamics
}

MODULE.Custom = function()
	vectorextensions("SSE3")

	filter({"architecture:x86_64", "system:linux"})
		defines("_POSIX_VER_64")

	filter({"architecture:x86", "system:linux"})
		defines("_POSIX_VER")
end
