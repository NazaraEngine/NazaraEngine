TOOL.Name = "ClientSDK"

TOOL.Directory = "../SDK"
TOOL.Kind = "Library"
TOOL.TargetDirectory = "../lib"

TOOL.Defines = {
	"NDK_CLIENT_BUILD"
}

TOOL.Includes = {
	"../SDK/include",
	"../SDK/src"
}

TOOL.Files = {
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
	"NazaraSDK",
	"NazaraAudio",
	"NazaraGraphics",
	"NazaraRenderer",
	"NazaraPlatform"
}
