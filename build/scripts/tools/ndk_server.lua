TOOL.Name = "SDKServer"

TOOL.Directory = "../SDK"
TOOL.Kind = "Library"
TOOL.TargetDirectory = "../lib"

TOOL.Defines = {
	"NDK_BUILD",
	"NDK_SERVER"
}

TOOL.Includes = {
	"../include",
	"../src"
}

TOOL.Files = {
	"../include/NazaraSDK/**.hpp",
	"../include/NazaraSDK/**.inl",
	"../src/NazaraSDK/**.hpp",
	"../src/NazaraSDK/**.inl",
	"../src/NazaraSDK/**.cpp"
}

-- Excludes client-only files
TOOL.FilesExcluded = {
	"../*/NazaraSDK/BaseWidget.*",
	"../*/NazaraSDK/Canvas.*",
	"../*/NazaraSDK/Console.*",
	"../*/NazaraSDK/**/CameraComponent.*",
	"../*/NazaraSDK/**/DebugComponent.*",
	"../*/NazaraSDK/**/DebugSystem.*",
	"../*/NazaraSDK/**/GraphicsComponent.*",
	"../*/NazaraSDK/**/LightComponent.*",
	"../*/NazaraSDK/**/ListenerComponent.*",
	"../*/NazaraSDK/**/ListenerSystem.*",
	"../*/NazaraSDK/**/Particle*Component.*",
	"../*/NazaraSDK/**/ParticleSystem.*",
	"../*/NazaraSDK/**/RenderSystem.*",
	"../*/NazaraSDK/**/*Layout*.*",
	"../*/NazaraSDK/**/*Widget*.*"
}

TOOL.Libraries = {
	"NazaraCore",
	"NazaraNetwork",
	"NazaraPhysics2D",
	"NazaraPhysics3D",
	"NazaraShader",
	"NazaraUtility"
}
