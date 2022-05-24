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

	add_requires("catch2")

	-- Common config
	set_group("Tests")
	set_kind("binary")

	add_deps("NazaraAudio", "NazaraCore", "NazaraNetwork", "NazaraPhysics2D")
	add_packages("catch2", "entt")
	add_headerfiles("Engine/**.hpp")
	add_files("resources.cpp")
	add_files("Engine/**.cpp")
	add_includedirs(".")

	if has_config("unitybuild") then
		add_rules("c++.unity_build")
	end

	target("NazaraUnitTests")
		add_files("main.cpp", {unity_ignored = true})

		if has_config("usepch") then
			set_pcxxheader("Engine/Modules.hpp")
		end
end
