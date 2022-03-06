-- Turns resources into includables headers
rule("embed_resources")
	before_build(function (target, opt)
		import("core.base.option")
		if xmake.version():ge("2.5.9") then
			import("utils.progress")
		elseif not import("utils.progress", { try = true }) then
			import("private.utils.progress")
		end

		local function GenerateEmbedHeader(filepath, targetpath)
			local bufferSize = 1024 * 1024

			progress.show(opt.progress, "${color.build.object}embedding %s", filepath)

			local resource = assert(io.open(filepath, "rb"))
			local targetFile = assert(io.open(targetpath, "w+"))

			local resourceSize = resource:size()

			local remainingSize = resourceSize
			local headerSize = 0

			while remainingSize > 0 do
				local readSize = math.min(remainingSize, bufferSize)
				local data = resource:read(readSize)
				remainingSize = remainingSize - readSize

				local headerContentTable = {}
				for i = 1, #data do
					table.insert(headerContentTable, string.format("%d,", data:byte(i)))
				end
				local content = table.concat(headerContentTable)

				headerSize = headerSize + #content

				targetFile:write(content)
			end

			resource:close()
			targetFile:close()
		end

		for _, sourcebatch in pairs(target:sourcebatches()) do
			if sourcebatch.rulename == "embed_resources" then
				for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
					local targetpath = sourcefile .. ".h"
					if option.get("rebuild") or os.mtime(sourcefile) >= os.mtime(targetpath) then
						GenerateEmbedHeader(sourcefile, targetpath)
					end
				end
			end
		end
	end)
