option("shadernodes", { description = "Build ShaderNodes tool (requires Qt)", default = false })

if has_config("shadernodes") then
	add_requires("nodeeditor", "qt5core", "qt5gui", "qt5widgets", {debug = is_mode("debug")})

	target("NazaraShaderNodes", function ()
		set_group("Tools")
		set_kind("binary")

		add_deps("NazaraCore")
		add_packages("nzsl")
		add_packages("nodeeditor")
		add_packages("qt5core", "qt5gui", "qt5widgets")
		if not is_plat("windows") then
			add_cxflags("-fPIC")
		end

		if has_config("unitybuild") then
			add_rules("c++.unity_build")
		end

		add_includedirs("../src")
		add_headerfiles("../src/ShaderNode/**.hpp", "../src/ShaderNode/**.inl", { prefixdir = "private", install = false })
		add_files("../src/ShaderNode/**.cpp")
		add_files("../src/ShaderNode/Previews/PreviewValues.cpp", { unity_ignored = true }) -- fixes an issue with MSVC and operator*
	end)
end
