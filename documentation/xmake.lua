option("docgen", { description = "Enables documentation generator (requires LLVM)", default = false })

if has_config("docgen") then
	includes("generator/xmake.lua")
end
