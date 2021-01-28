LIBRARY.Name = "newton"

LIBRARY.Defines = {
	"_CRT_SECURE_NO_WARNINGS",
	"_NEWTON_STATIC_LIB",
}

LIBRARY.OsDefines.Windows = {
	"_WINDOWS"
}

LIBRARY.Language = "C++"
LIBRARY.DisableWarnings = true -- Newton has many warnings we can't really fix

LIBRARY.Files = {
	"../thirdparty/include/newton/**.h",
	"../thirdparty/src/newton/**.h",
	"../thirdparty/src/newton/**.c",
	"../thirdparty/src/newton/**.cpp",
}

LIBRARY.Includes = {
	"../thirdparty/src/newton/dgCore",
	"../thirdparty/src/newton/dgMeshUtil",
	"../thirdparty/src/newton/dgPhysics",
	"../thirdparty/src/newton/dgNewton",
	"../thirdparty/src/newton/dContainers",
	"../thirdparty/src/newton/dMath"
}

LIBRARY.Custom = function()
	vectorextensions("SSE3")

	filter({"architecture:x86_64", "system:linux"})
		defines("_POSIX_VER_64")

	filter({"architecture:x86", "system:linux"})
		defines("_POSIX_VER")
		
	filter({"architecture:x86_64", "system:macosx"})
		defines("_MACOSX_VER")
end
