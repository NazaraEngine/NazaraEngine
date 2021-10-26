
task("check-files")

set_menu({
	-- Settings menu usage
	usage = "xmake check-files [options]",
	description = "Check every file for consistency (can fix some errors)",
	options =
	{
		-- Set k mode as key-only bool parameter
		{'f', "fix", "k", nil, "Attempt to automatically fix files." }
	}
})

local function CompareLines(referenceLines, lines, firstLine, lineCount)
	firstLine = firstLine or 1
	lineCount = lineCount or (#lines - firstLine + 1)
	if lineCount ~= #referenceLines then
		return false
	end

	for i = 1, lineCount do
		if lines[firstLine + i - 1] ~= referenceLines[i] then
			print(lines[firstLine + i])
			print(referenceLines[i])
			return false
		end
	end

	return true
end

on_run(function ()
	import("core.base.option")

	local modules = os.dirs("include/Nazara/*")

	local fileLines = {}
	local updatedFiles = {}
	local function GetFile(filePath)
		filePath = path.translate(filePath)

		local lines = fileLines[filePath]
		if not lines then
			lines = table.to_array(io.lines(filePath))
			if not lines then
				os.raise("failed to open " .. filePath)
			end

			fileLines[filePath] = lines
		end

		return lines
	end

	local function UpdateFile(filePath, lines)
		filePath = path.translate(filePath)

		if lines then
			fileLines[filePath] = lines
		end

		updatedFiles[filePath] = true
	end

	local checks = {}

	table.insert(checks, {
		Name = "remove empty lines at the beginning",
		Check = function (moduleName)
			local files = table.join(
				os.files("include/Nazara/" .. moduleName .. "/**.hpp"),
				os.files("include/Nazara/" .. moduleName .. "/**.inl"),
				os.files("src/Nazara/" .. moduleName .. "/**.hpp"),
				os.files("src/Nazara/" .. moduleName .. "/**.inl"),
				os.files("src/Nazara/" .. moduleName .. "/**.cpp")
			)

			local fixes = {}

			for _, filePath in pairs(files) do
				local lines = GetFile(filePath)

				for i = 1, #lines do
					if not lines[i]:match("^%s*$") then
						if i ~= 1 then
							print(filePath .. " starts with empty lines")

							table.insert(fixes, {
								File = filePath,
								Func = function (lines)
									for j = 1, i - 1 do
										table.remove(lines, 1)
									end

									UpdateFile(filePath, lines)
								end
							})
						end
						
						break
					end
				end
			end

			return fixes
		end
	})

	table.insert(checks, {
		Name = "copyright",
		Check = function (moduleName)
			local files = table.join(
				os.files("include/Nazara/" .. moduleName .. "/**.hpp|Config.hpp"),
				os.files("include/Nazara/" .. moduleName .. "/**.inl"),
				os.files("src/Nazara/" .. moduleName .. "/**.hpp"),
				os.files("src/Nazara/" .. moduleName .. "/**.inl"),
				os.files("src/Nazara/" .. moduleName .. "/**.cpp")
			)

			local currentYear = os.date("%Y")
			local engineAuthor = "Jérôme \"Lynix\" Leclercq (lynix680@gmail.com)"
			local prevAuthor = "Jérôme Leclercq"
			local fixes = {}

			local moduleDesc
			if #moduleName > 8 and moduleName:endswith("Renderer") then
				moduleDesc = moduleName:sub(1, -9) .. " renderer"
			else
				moduleDesc = moduleName .. " module"
			end

			-- Config
			do
				local configFilePath = path.translate("include/Nazara/" .. moduleName .. "/Config.hpp")
				local lines = GetFile(configFilePath)
				local pragmaLine
				for i = 1, #lines do
					if lines[i] == "#pragma once" then
						pragmaLine = i
						break
					end
				end

				local canFix = true

				repeat
					if not pragmaLine then
						print(configFilePath .. ": pragma once not found")
						canFix = false
						break
					end

					local licenseText = [[

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
	of the Software, and to permit persons to whom the Software is furnished to do
	so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once]]

					local licenseLines = licenseText:split("\r?\n", { strict = true })
					local shouldFix = false

					-- Try to retrieve year and authors
					if lines[1] ~= "/*" then
						print(configFilePath .. ": file doesn't begin with block comment")
						break
					end


					if lines[2]:match("\tNazara Engine - " .. moduleDesc) then
						print(configFilePath .. ": module description doesn't match")
						shouldFix = true
					end

					if lines[3] ~= "" then
						-- Not really empty because of space characters? Not a big deal
						if lines[3]:match("^%s*$") then
							shouldFix = true
						else
							print(configFilePath .. ": expected space after project name")
							break
						end
					end

					local year, moduleAuthor = lines[4]:match("^\tCopyright %(C%) (Y?E?A?R?%d*) (.+)$")
					if not year then
						print(configFilePath .. ": couldn't parse copyright date and author")
						break
					end

					if year ~= currentYear then
						print(configFilePath .. ": incorrect copyright year")
						shouldFix = true
					end

					local additionalAuthors = {}
					for i = 5, #lines do
						if lines[i]:match("^%s*$") then
							-- Empty line
							if not CompareLines(licenseLines, lines, i, pragmaLine - i + 1) then
								shouldFix = true
							end

							break
						end

						if lines[i]:match("%s*Permission is hereby granted") then
							print(configFilePath .. ": missing empty line before licence text")
							shouldFix = true
							break
						end

						table.insert(additionalAuthors, lines[i]:match("%s*(.+)"))
					end

					if shouldFix then
						table.insert(fixes, {
							File = configFilePath,
							Func = function (lines)
								local newLines = {
									"/*",
									"\tNazara Engine - " .. moduleDesc,
									"",
								}

								local copyrightNotice = "Copyright (C) " .. currentYear
								table.insert(newLines, "\t" .. copyrightNotice .. " " .. moduleAuthor)

								for _, author in ipairs(additionalAuthors) do
									table.insert(newLines, "\t" .. string.rep(" ", #copyrightNotice) .. " " .. author)
								end

								for _, line in ipairs(licenseLines) do
									table.insert(newLines, line)
								end

								for i = pragmaLine + 1, #lines do
									table.insert(newLines, lines[i])
								end

								UpdateFile(configFilePath, newLines)
							end
						})
					end
				until true

				if not canFix then
					print(configFilePath .. ": header couldn't be parsed, no automated fix will be done")
				end
			end

			-- Headers
			for _, filePath in pairs(files) do
				local lines = GetFile(filePath)

				local hasCopyright
				local shouldFix = false

				if lines[1]:lower():match("^// this file was automatically generated") then
					goto skip
				end

				local year, authors = lines[1]:match("^// Copyright %(C%) (Y?E?A?R?%d*) (.+)$")
				hasCopyright = year ~= nil

				if authors == "AUTHORS" then
					authors = engineAuthor
				else
					local fixedAuthors = authors:gsub(prevAuthor, engineAuthor)
					if fixedAuthors ~= authors then
						authors = fixedAuthors
						shouldFix = true
					end
				end

				if hasCopyright then
					if year ~= currentYear then
						print(filePath .. ": copyright year error")
						shouldFix = true
					end

					local copyrightModule = lines[2]:match("^// This file is part of the \"Nazara Engine %- (.+)\"$")
					if copyrightModule ~= moduleDesc then
						print(filePath .. ": copyright module error")
						shouldFix = true
					end

					if lines[3] ~= "// For conditions of distribution and use, see copyright notice in Config.hpp" then
						print(filePath .. ": copyright file reference error")
						shouldFix = true
					end
				else
					print(filePath .. ": copyright not found")
					shouldFix = true
				end

				if shouldFix then
					table.insert(fixes, {
						File = filePath,
						Func = function(lines)
							local copyrightLines = {
								"// Copyright (C) " .. currentYear .. " " .. (authors or engineAuthor),
								"// This file is part of the \"Nazara Engine - " .. moduleDesc .. "\"",
								"// For conditions of distribution and use, see copyright notice in Config.hpp"
							}

							if hasCopyright then
								for i, line in ipairs(copyrightLines) do
									lines[i] = line
								end
							else
								for i, line in ipairs(copyrightLines) do
									table.insert(lines, i, line)
								end
								table.insert(lines, #copyrightLines + 1, "")
							end

							UpdateFile(filePath, lines)
						end
					})
				end

				::skip::
			end

			return fixes
		end
	})

	local shouldFix = option.get("fix") or false

	for _, check in pairs(checks) do
		print("Running " .. check.Name .. " check...")

		local fixes = {}
		for _, modulePath in pairs(modules) do
			local moduleName = modulePath:match(".*[\\/](.*)")

			table.join2(fixes, check.Check(moduleName))
		end

		if shouldFix then
			for _, fix in pairs(fixes) do
				print("Fixing " .. fix.File)
				fix.Func(assert(fileLines[fix.File]))
			end

			for filePath, _ in pairs(updatedFiles) do
				local lines = assert(fileLines[filePath])
				if lines[#lines] ~= "" then
					table.insert(lines, "")
				end

				print("Saving changes to " .. filePath)
				io.writefile(filePath, table.concat(lines, "\n"))
			end
		end
	end
end)
