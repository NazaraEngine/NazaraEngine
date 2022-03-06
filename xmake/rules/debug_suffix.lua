-- Adds -d as a debug suffix
rule("debug_suffix")
	on_load(function (target)
		if target:kind() ~= "binary" then
			target:set("basename", target:basename() .. "-d")
		end
	end)
