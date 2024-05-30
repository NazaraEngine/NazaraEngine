option("tests", { description = "Build tests", default = true })

if has_config("tests") then
	if is_plat("macosx") then
		add_requires("moltenvk", { configs = { shared = true }})
		add_packages("moltenvk", { links = {} })
	end

	set_group("Tests")
	includes("*/xmake.lua")
end
