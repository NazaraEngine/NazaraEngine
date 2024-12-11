option("imgui", { description = "Build Dear ImGui plugin", default = true, category = "Plugins" })

if has_config("imgui") then
	add_requires("imgui v1.91.1-docking")

	target("PluginImGui")
		set_group("Plugins")
		add_rpathdirs("$ORIGIN")
		set_license("MIT")

		add_deps("NazaraCore", "NazaraRenderer", "NazaraTextRenderer")
		add_packages("imgui")

		add_headerfiles("**.hpp", "**.inl", { prefixdir = "private", install = false })
		add_includedirs(".")
		add_files("**.cpp")
end
