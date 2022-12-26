option("examples", { description = "Build examples", default = true })

if has_config("examples") then
	set_group("Examples")

	includes("*/xmake.lua")
end
