-- Builds renderer plugins if linked to NazaraRenderer
rule("build_rendererplugins")
	on_load(function (target)
		local deps = table.wrap(target:get("deps"))

		if target:kind() == "binary" and (table.contains(deps, "NazaraRenderer") or table.contains(deps, "NazaraGraphics")) then
			for name, _ in pairs(modules) do
				local depName = "Nazara" .. name
				if name:match("^.+Renderer$") and not table.contains(deps, depName) then -- don't overwrite dependency
					target:add("deps", depName, {inherit = false})
				end
			end
		end
	end)
