TOOL.Name = "SDKServer"

TOOL.Directory = "../SDK/lib"
TOOL.Kind = "Library"

TOOL.Defines = {
	"NDK_BUILD",
	"NDK_SERVER"
}

TOOL.Includes = {
	"../SDK/include"
}

TOOL.Files = {
	"../SDK/include/NDK/**.hpp",
	"../SDK/include/NDK/**.inl",
	"../SDK/src/NDK/**.hpp",
	"../SDK/src/NDK/**.inl",
	"../SDK/src/NDK/**.cpp"
}

-- Exlude client-only files
TOOL.FilesExclusion = {
	"../SDK/**/CameraComponent.*",
	"../SDK/**/GraphicsComponent.*",
	"../SDK/**/LightComponent.*",
	"../SDK/**/ListenerComponent.*",
	"../SDK/**/ListenerSystem.*",
	"../SDK/**/RenderSystem.*",
	"../SDK/**/LuaInterface_Audio.*"
}

TOOL.Libraries = {
	"NazaraCore",
	"NazaraLua",
	"NazaraNoise",
	"NazaraPhysics",
	"NazaraUtility"
}
