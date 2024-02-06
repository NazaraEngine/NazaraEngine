target("PhysicsPlayground")
	add_deps("NazaraGraphics", "NazaraJoltPhysics3D")
	add_packages("entt")
	add_files("main.cpp")
	add_defines("NAZARA_ENTT")

	if is_plat("wasm") then
		add_ldflags("--preload-file assets/examples/dev_grey.png", { force = true })
		add_ldflags("--preload-file assets/examples/Spaceship", { force = true })
	end
