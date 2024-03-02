if not has_config("assimp") then
	return
end

target("AnimationBlendingTest")
	set_kind("binary")
	add_deps("NazaraGraphics", "NazaraWidgets")
	if has_config("embed_plugins", "static") then
		add_deps("PluginAssimp")
	else
		add_deps("PluginAssimp", { links = {} })
	end
	add_packages("entt")
	add_files("main.cpp")
	add_defines("NAZARA_ENTT")
