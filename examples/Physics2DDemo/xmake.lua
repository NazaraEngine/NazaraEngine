target("Physics2DDemo")
	add_deps("NazaraGraphics", "NazaraPhysics2D")
	add_packages("entt")
	add_files("main.cpp")
	add_defines("NAZARA_ENTT")

	if is_plat("wasm") then
		add_ldflags("--preload-file assets/", { force = true })
	end
