if not has_config("assimp") then
	return
end

target("Showcase")
	set_group("Examples")
	set_kind("binary")
	add_deps("NazaraAudio", "NazaraGraphics", "NazaraPhysics2D", "NazaraPhysics3D", "NazaraWidgets")
	if has_config("embed_plugins", "static") then
		add_deps("PluginAssimp")
	else
		add_deps("PluginAssimp", { links = {} })
	end
	add_packages("entt")
	add_files("main.cpp")
	add_defines("NAZARA_ENTT")

	if is_plat("wasm") then
		add_ldflags("--preload-file assets/examples/", {force = true})
	end
