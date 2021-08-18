add_requires("assimp")

target("PluginAssimp")
	set_kind("shared")
	set_group("Plugins")

	add_deps("NazaraUtility")
	add_packages("assimp")

	add_headerfiles("**.hpp")
	add_headerfiles("**.inl")
	add_includedirs(".")
	add_files("**.cpp")
