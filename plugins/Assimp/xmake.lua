option("assimp", { description = "Build Assimp plugin", default = true })

if has_config("assimp") then
	add_requires("assimp v5.2.3")

	target("PluginAssimp")
		set_kind("shared")
		set_group("Plugins")
		add_rpathdirs("$ORIGIN")

		add_deps("NazaraUtility")
		add_packages("assimp")

		add_headerfiles("**.hpp", "**.inl", { prefixdir = "private", install = false })
		add_includedirs(".")
		add_files("**.cpp")
end
