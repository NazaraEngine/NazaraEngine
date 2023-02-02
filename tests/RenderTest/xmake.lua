target("RenderTest")
	add_deps("NazaraRenderer")
	add_files("main.cpp")

	if is_plat("wasm") then
		add_ldflags("--preload-file assets/examples/", {force = true})
	end
