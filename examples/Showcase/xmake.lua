if not has_config("assimp") then
	return
end

target("Showcase")
	set_group("Examples")
	set_kind("binary")
	add_deps("NazaraAudio", "NazaraGraphics", "NazaraChipmunkPhysics2D", "NazaraJoltPhysics3D", "NazaraWidgets")
	if has_config("embed_plugins", "static") then
		add_deps("PluginAssimp")
	else
		add_deps("PluginAssimp", { links = {} })
	end
	add_packages("entt")
	add_files("main.cpp")
	add_defines("NAZARA_ENTT")
