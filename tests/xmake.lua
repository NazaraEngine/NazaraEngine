if is_mode("asan") then
	add_defines("CATCH_CONFIG_NO_WINDOWS_SEH")
	add_defines("CATCH_CONFIG_NO_POSIX_SIGNALS")
end

add_requires("catch2")

target("NazaraClientUnitTests")
	set_group("Tests")
	set_kind("binary")

	add_deps("NazaraAudio", "NazaraCore", "NazaraNetwork", "NazaraPhysics2D", "NazaraShader")
	add_packages("catch2")

	add_files("main_client.cpp")
	add_files("resources.cpp")
	add_files("Engine/**.cpp")

target("NazaraUnitTests")
	set_group("Tests")
	set_kind("binary")

	add_deps("NazaraCore", "NazaraNetwork", "NazaraPhysics2D", "NazaraShader")
	add_packages("catch2")

	add_files("main.cpp")
	add_files("resources.cpp")
	add_files("Engine/**.cpp")
