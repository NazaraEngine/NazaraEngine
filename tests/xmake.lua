if is_mode("asan") then
	add_defines("CATCH_CONFIG_NO_WINDOWS_SEH")
	add_defines("CATCH_CONFIG_NO_POSIX_SIGNALS")
end

add_requires("catch2", "spirv-tools")

-- Common config
set_group("Tests")
set_kind("binary")

add_deps("NazaraCore", "NazaraNetwork", "NazaraPhysics2D", "NazaraShader")
add_packages("catch2", "spirv-tools")
add_headerfiles("Engine/**.hpp")
add_files("resources.cpp")
add_files("Engine/**.cpp")
add_includedirs(".")

if xmake.version():ge("2.5.9") then
	add_rules("c++.unity_build")
end

target("NazaraClientUnitTests")
	add_deps("NazaraAudio")
	add_files("main_client.cpp")

target("NazaraUnitTests")
	add_files("main.cpp")
	-- del_headerfiles("Engine/Audio/**")
	del_files("Engine/Audio/**")
