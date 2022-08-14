target("GraphicsTest")
	add_deps("NazaraGraphics")
	add_files("main.cpp")

	if is_plat("wasm") then
		add_ldflags("--preload-file assets/", {force = true})
	end
