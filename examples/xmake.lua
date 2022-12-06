option("examples", { description = "Build examples", default = true })

if has_config("examples") then
	set_group("Examples")

	if is_plat("wasm") then
		add_ldflags("--preload-file assets/", { force = true })
	end

	includes("*/xmake.lua")
end
