TOOL.Name = "SDK"

TOOL.Directory = "../SDK"
TOOL.Kind = "Library"
TOOL.TargetDirectory = "../lib"

TOOL.Defines = {
	"NDK_BUILD"
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
	"../SDK/**/Client*.*",
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
	"../SDK/**/*Layout*.*",
	"../SDK/**/*Widget*.*"
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
