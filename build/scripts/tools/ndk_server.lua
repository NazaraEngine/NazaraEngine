TOOL.Name = "SDKServer"

TOOL.Directory = "../SDK"
TOOL.Kind = "Library"
TOOL.TargetDirectory = "../lib"

TOOL.Defines = {
	"NDK_BUILD",
	"NDK_SERVER"
}

TOOL.Includes = {
	"../SDK/include",
	"../SDK/src"
}

TOOL.Files = {
	"../SDK/include/NDK/**.hpp",
	"../SDK/include/NDK/**.inl",
	"../SDK/src/NDK/**.hpp",
	"../SDK/src/NDK/**.inl",
	"../SDK/src/NDK/**.cpp"
}

-- Excludes client-only files
TOOL.FilesExcluded = {
	"../SDK/**/CameraComponent.*",
	"../SDK/**/Canvas.*",
	"../SDK/**/Console.*",
	"../SDK/**/DebugComponent.*",
	"../SDK/**/DebugSystem.*",
	"../SDK/**/GraphicsComponent.*",
	"../SDK/**/LightComponent.*",
	"../SDK/**/ListenerComponent.*",
	"../SDK/**/ListenerSystem.*",
	"../SDK/**/Particle*Component.*",
	"../SDK/**/ParticleSystem.*",
	"../SDK/**/RenderSystem.*",
	"../SDK/**/*Widget*.*",
	"../SDK/**/LuaBinding_Audio.*",
	"../SDK/**/LuaBinding_Graphics.*",
	"../SDK/**/LuaBinding_Renderer.*",
	"../SDK/**/LuaBinding_Platform.*"
}


TOOL.Libraries = {
	"NazaraCore",
	"NazaraLua",
	"NazaraNetwork",
	"NazaraNoise",
	"NazaraPhysics2D",
	"NazaraPhysics3D",
	"NazaraUtility"
}
