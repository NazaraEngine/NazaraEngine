option("examples", { description = "Build examples", default = true })

if has_config("examples") then
	-- Common config
	set_group("Examples")
	set_kind("binary")

	includes("*/xmake.lua")
end
