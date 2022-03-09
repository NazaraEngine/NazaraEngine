option("tests")
	set_default(false)
	set_showmenu(true)
	set_description("Build unit tests")

option_end()

if has_config("tests") then
	if is_mode("asan") then
		add_defines("CATCH_CONFIG_NO_WINDOWS_SEH")
		add_defines("CATCH_CONFIG_NO_POSIX_SIGNALS")
	end

	add_requires("catch2", "glslang", "spirv-tools")

	-- Common config
	set_group("Tests")
	set_kind("binary")

	add_deps("NazaraCore", "NazaraNetwork", "NazaraPhysics2D", "NazaraShader")
	add_packages("catch2", "entt", "glslang", "spirv-tools")
	add_headerfiles("Engine/**.hpp")
	add_files("resources.cpp")
	add_files("Engine/**.cpp")
	add_includedirs(".")

	target("NazaraClientUnitTests")
		add_deps("NazaraAudio")
		add_files("main_client.cpp")

		if has_config("usepch") then
			set_pcxxheader("Engine/ClientModules.hpp")
		end

	target("NazaraUnitTests")
		add_files("main.cpp")
		remove_headerfiles("Engine/Audio/**")
		remove_files("Engine/Audio/**")

		if has_config("usepch") then
			set_pcxxheader("Engine/Modules.hpp")
		end
end
