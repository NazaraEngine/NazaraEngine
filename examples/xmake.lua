option("examples")
	set_default(true)
	set_showmenu(true)
	set_description("Build examples")

option_end()

if has_config("examples") then
	-- Common config
	set_group("Examples")
	set_kind("binary")

	includes("*/xmake.lua")
end
