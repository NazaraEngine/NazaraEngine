if not has_config("imgui") then
	return
end

target("ImGuiTest")
	add_deps("NazaraRenderer")
	add_files("main.cpp")
	if has_config("embed_plugins", "static") then
		add_deps("PluginImGui")
	else
		add_deps("PluginImGui", { links = {} })
	end
	add_packages("imgui")

	if is_plat("wasm") then
		add_ldflags("--preload-file assets/examples/", {force = true})
	end
