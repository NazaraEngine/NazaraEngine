
add_requires("nodeeditor", {debug = is_mode("debug"), optional = true})

target("NazaraSDK")
	set_group("SDK")
	set_kind("shared")
    add_deps("NazaraCore", "NazaraPhysics2D", "NazaraPhysics3D", "NazaraNetwork", "NazaraShader", "NazaraUtility", { public = true })

	add_defines("NDK_BUILD")
	add_defines("NDK_SERVER", { public = true })
	add_includedirs("../src")
	add_headerfiles("../include/NazaraSDK/**.hpp", "../include/NazaraSDK/**.inl", "../src/NazaraSDK/**.hpp", "../src/NazaraSDK/**.inl")
	add_files("../src/NazaraSDK/**.cpp")

	--del_headerfiles("../*/NazaraSDK/Client*.*")
	--del_headerfiles("../*/NazaraSDK/*/ListenerComponent*.*")
	--del_headerfiles("../*/NazaraSDK/*/ListenerSystem*.*")
	del_files("../*/NazaraSDK/Client*.*")
	del_files("../*/NazaraSDK/*/ListenerComponent*.*")
	del_files("../*/NazaraSDK/*/ListenerSystem*.*")

target("NazaraClientSDK")
	set_group("SDK")
	set_kind("shared")
    add_deps("NazaraSDK", "NazaraAudio", "NazaraGraphics", "NazaraRenderer", { public = true })

	add_defines("NDK_CLIENT_BUILD")
	add_includedirs("../src")

	add_headerfiles("../include/NazaraSDK/Client*.hpp")
	add_headerfiles("../include/NazaraSDK/Client*.inl")
	add_headerfiles("../include/NazaraSDK/Components/ListenerComponent.hpp")
	add_headerfiles("../include/NazaraSDK/Components/ListenerComponent.inl")
	add_headerfiles("../include/NazaraSDK/Systems/ListenerSystem.hpp")
	add_headerfiles("../include/NazaraSDK/Systems/ListenerSystem.inl")
	add_files("../src/NazaraSDK/Client*.cpp")
	add_files("../src/NazaraSDK/Components/ListenerComponent.cpp")
	add_files("../src/NazaraSDK/Systems/ListenerSystem.cpp")

target("NazaraShaderNodes")
	set_group("Tools")
	set_kind("binary")
	add_rules("qt.console", "qt.moc")

	add_deps("NazaraShader")
	add_frameworks("QtCore", "QtGui", "QtWidgets")
	add_packages("nodeeditor")

	add_includedirs("../src")
	add_headerfiles("../src/ShaderNode/**.hpp", "../src/ShaderNode/**.inl")
	add_files("../src/ShaderNode/**.cpp")

	on_load(function (target)
		import("detect.sdks.find_qt")

		if (not has_package("nodeeditor") or not find_qt()) then
			-- Disable building by default if nodeeditor or Qt is not found
			target:set("default", false)
		end
	end)
