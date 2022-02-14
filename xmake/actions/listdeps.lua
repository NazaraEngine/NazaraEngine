task("list-deps")

set_menu({
	-- Settings menu usage
	usage = "xmake list-deps [target]",
	description = "List dependencies used by the engine or a specific target",
	options =
	{
		{'t', "target", "v", nil, "Target name" }
	}
})

on_run(function ()
	import("core.base.option")
	import("core.base.task")
	import("core.project.project")
	import("private.action.require.impl.package")

    task.run("config", {require = false})
    os.cd(project.directory())

	local requires, requires_extra = project.requires_str()
	if not requires or #requires == 0 then
		return
	end

	local usedpackages

	local targetname = option.get("target")
	if targetname then
		local target = project.target(targetname)
		if not target then
			os.raise("unknown target " .. targetname)
		end
		usedpackages = target:pkgs()
	else
		usedpackages = project.required_packages()
	end

	local projectdeps = {}
	for _, instance in ipairs(package.load_packages(requires, {requires_extra = requires_extra})) do
		if usedpackages[instance:name()] then
			table.insert(projectdeps, {
				name = instance:name(),
				desc = instance:description(),
				homepage = instance:get("homepage")
			})
		end
	end

	table.sort(projectdeps, function (a, b) return a.name < b.name end)

	for _, dep in pairs(projectdeps) do
		print(string.format("- [%s](%s): %s", dep.name, dep.homepage, dep.desc))
	end
end)