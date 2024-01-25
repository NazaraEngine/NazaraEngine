local baseDownloadURL = "https://nazara.digitalpulse.software"

local function downloadAssetsRule(name)
	rule("download.assets." .. name)
		set_kind("project")
		before_build(function (opt)
			import("net.http")
			import("utils.archive")

			local referenceVersion = tonumber(io.readfile("assets/" .. name .. "_version.txt"))
			local currentVersion = os.exists("assets/" .. name .. "/version.txt") and tonumber(io.readfile("assets/" .. name .. "/version.txt"))
			if referenceVersion == currentVersion then
				utils.vprintf(name .. " assets are up-to-date, ignoring\n")
				return
			end

			local text = {}
			table.insert(text, currentVersion and "update " or "download ")
			table.insert(text, name)
			table.insert(text, " assets?")
			if currentVersion then
				table.insert(text, " current version is " .. currentVersion .. " and remote version is " .. referenceVersion)
			end
			table.insert(text, "\nthis is required to run them")

			local confirm = utils.confirm({description = table.concat(text), default = true})
			if not confirm then
				utils.vprintf("aborting " .. name .. " assets downloading\n")
				return
			end

			os.rm("assets/" .. name)
			http.download(baseDownloadURL .. "/assets_" .. name .. ".zip", "assets_" .. name .. ".zip")
			archive.extract("assets_" .. name .. ".zip", "assets")
			os.rm("assets_" .. name .. ".zip")

			print(name .. " assets downloaded!")
		end)
end

downloadAssetsRule("examples")
downloadAssetsRule("unittests")