option("examples", { description = "Build examples", default = true })

if has_config("examples") then
	if is_plat("macosx") then
		add_requires("moltenvk", { configs = { shared = true }})
		add_packages("moltenvk", { links = {} })
	end

	set_group("Examples")
	includes("*/xmake.lua")
end
