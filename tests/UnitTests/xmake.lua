add_requires("catch2 >=3.x")

target("UnitTests_sub1", function ()
    add_deps("NazaraCore")

    add_files("subprocess1.cpp")
end)

target("UnitTests_sub2", function ()
    add_deps("NazaraCore")

    add_files("subprocess2.cpp")
end)

target("UnitTests", function ()
    if has_config("asan") then
        add_defines("CATCH_CONFIG_NO_WINDOWS_SEH")
        add_defines("CATCH_CONFIG_NO_POSIX_SIGNALS")
    end

    add_deps("NazaraAudio2", "NazaraCore", "NazaraNetwork", "NazaraPhysics2D", "NazaraTextRenderer")
    add_deps("UnitTests_sub1", "UnitTests_sub2", { links = {} })
    add_packages("catch2", "entt", "frozen")
    add_headerfiles("Engine/**.hpp", { prefixdir = "private", install = false })
    add_files("resources.cpp")
    add_files("Engine/**.cpp")
    add_includedirs(".")
    add_tests("run_tests")

    if has_config("unitybuild") then
        add_rules("c++.unity_build")
    end

    add_files("main.cpp", {unity_ignored = true})

    if has_config("usepch") then
        set_pcxxheader("Engine/Modules.hpp")
    end
end)
