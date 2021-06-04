if is_mode("asan") then
	add_defines("CATCH_CONFIG_NO_WINDOWS_SEH")
	add_defines("CATCH_CONFIG_NO_POSIX_SIGNALS")
end

target("NazaraClientUnitTests")
	set_group("Tests")
	set_kind("binary")

	add_deps("NazaraClientSDK")
	add_files("main_client.cpp")
	add_files("resources.cpp")
	add_files("Engine/**.cpp")

target("NazaraUnitTests")
	set_group("Tests")
	set_kind("binary")

	add_deps("NazaraSDK")
	add_files("main.cpp")
	add_files("resources.cpp")
	add_files("Engine/**.cpp")
	add_files("SDK/**.cpp")

	del_files("Engine/Audio/**")
	del_files("SDK/NDK/Application.cpp")
	del_files("SDK/NDK/Systems/ListenerSystem.cpp")
