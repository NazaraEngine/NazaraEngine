add_requires("catch2 >=3.x")

if has_config("asan") then
    add_defines("CATCH_CONFIG_NO_WINDOWS_SEH")
    add_defines("CATCH_CONFIG_NO_POSIX_SIGNALS")
end

add_deps("NazaraAudio", "NazaraCore", "NazaraNetwork", "NazaraChipmunkPhysics2D")
add_packages("catch2", "entt", "frozen")
add_headerfiles("Engine/**.hpp", { prefixdir = "private", install = false })
add_files("resources.cpp")
add_files("Engine/**.cpp")
add_includedirs(".")

if has_config("unitybuild") then
    add_rules("c++.unity_build")
end

target("UnitTests", function ()
    add_files("main.cpp", {unity_ignored = true})

    if has_config("usepch") then
        set_pcxxheader("Engine/Modules.hpp")
    end
end)
