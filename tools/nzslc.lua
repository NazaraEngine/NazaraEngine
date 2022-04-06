add_requires("cxxopts")

target("NazaraShaderCompiler")
    set_group("Tools")
    set_kind("binary")
	set_basename("nzslc")

    add_deps("NazaraShader")
    add_packages("cxxopts", "fmt")

    if has_config("unitybuild") then
        add_rules("c++.unity_build")
    end

    add_includedirs("../src")
    add_headerfiles("../src/ShaderCompiler/**.hpp", "../src/ShaderCompiler/**.inl")
    add_files("../src/ShaderCompiler/**.cpp")
    set_policy("build.across_targets_in_parallel", false)
