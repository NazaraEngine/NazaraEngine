local modules = NazaraRendererBackends

-- Builds renderer plugins if linked to NazaraRenderer
rule("build.rendererplugins")
	on_load(function (target)
		if has_config("embed_rendererbackends") then
			return
		end

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
