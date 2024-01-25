local unicodeDataURL = "https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt"

task("update-unicode")

set_menu({
	-- Settings menu usage
	usage = "xmake update-unicode [options]",
	description = "Download and parse the Unicode Character Data and updates the core module files with it"
})

on_run(function()
	import("net.http")

local CategoryToString = {}
	CategoryToString["C"]  = "Category_Other"
	CategoryToString["Cc"] = "Category_Other_Control"
	CategoryToString["Cf"] = "Category_Other_Format"
	CategoryToString["Cn"] = "Category_Other_NotAssigned"
	CategoryToString["Co"] = "Category_Other_PrivateUse"
	CategoryToString["Cs"] = "Category_Other_Surrogate"
	CategoryToString["L"]  = "Category_Letter"
	CategoryToString["Ll"] = "Category_Letter_Lowercase"
	CategoryToString["Lm"] = "Category_Letter_Modifier"
	CategoryToString["Lo"] = "Category_Letter_Other"
	CategoryToString["Lt"] = "Category_Letter_Titlecase"
	CategoryToString["Lu"] = "Category_Letter_Uppercase"
	CategoryToString["M"]  = "Category_Mark"
	CategoryToString["Me"] = "Category_Mark_Enclosing"
	CategoryToString["Mn"] = "Category_Mark_NonSpacing"
	CategoryToString["Mc"] = "Category_Mark_SpacingCombining"
	CategoryToString["N"]  = "Category_Number"
	CategoryToString["Nd"] = "Category_Number_DecimalDigit"
	CategoryToString["Nl"] = "Category_Number_Letter"
	CategoryToString["No"] = "Category_Number_Other"
	CategoryToString["P"]  = "Category_Punctuation"
	CategoryToString["Pe"] = "Category_Punctuation_Close"
	CategoryToString["Pc"] = "Category_Punctuation_Connector"
	CategoryToString["Pd"] = "Category_Punctuation_Dash"
	CategoryToString["Pf"] = "Category_Punctuation_FinalQuote"
	CategoryToString["Pi"] = "Category_Punctuation_InitialQuote"
	CategoryToString["Ps"] = "Category_Punctuation_Open"
	CategoryToString["Po"] = "Category_Punctuation_Other"
	CategoryToString["S"]  = "Category_Symbol"
	CategoryToString["Sc"] = "Category_Symbol_Currency"
	CategoryToString["Sm"] = "Category_Symbol_Math"
	CategoryToString["Sk"] = "Category_Symbol_Modifier"
	CategoryToString["So"] = "Category_Symbol_Other"
	CategoryToString["Z"]  = "Category_Separator"
	CategoryToString["Zl"] = "Category_Separator_Line"
	CategoryToString["Zp"] = "Category_Separator_Paragraph"
	CategoryToString["Zs"] = "Category_Separator_Space"
	
local DirectionToString = {}
	DirectionToString["AL"]  = "Direction_Arabic_Letter"
	DirectionToString["AN"]  = "Direction_Arabic_Number"
	DirectionToString["BN"]  = "Direction_Boundary_Neutral"
	DirectionToString["CS"]  = "Direction_Common_Separator"
	DirectionToString["EN"]  = "Direction_European_Number"
	DirectionToString["ES"]  = "Direction_European_Separator"
	DirectionToString["ET"]  = "Direction_European_Terminator"
	DirectionToString["FSI"]  = "Direction_First_Strong_Isolate"
	DirectionToString["L"]	 = "Direction_Left_To_Right"
	DirectionToString["LRE"] = "Direction_Left_To_Right_Embedding"
	DirectionToString["LRI"] = "Direction_Left_To_Right_Isolate"
	DirectionToString["LRO"] = "Direction_Left_To_Right_Override"
	DirectionToString["NSM"] = "Direction_Nonspacing_Mark"
	DirectionToString["ON"]	 = "Direction_Other_Neutral"
	DirectionToString["B"]	 = "Direction_Paragraph_Separator"
	DirectionToString["PDF"] = "Direction_Pop_Directional_Formatting"
	DirectionToString["PDI"] = "Direction_Pop_Directional_Isolate"
	DirectionToString["R"]	 = "Direction_Right_To_Left"
	DirectionToString["RLE"] = "Direction_Right_To_Left_Embedding"
	DirectionToString["RLI"] = "Direction_Right_To_Left_Isolate"
	DirectionToString["RLO"] = "Direction_Right_To_Left_Override"
	DirectionToString["S"]	 = "Direction_Segment_Separator"
	DirectionToString["WS"]  = "Direction_White_Space"
	
	local unicodeSet = {}

	io.write("Downloading UnicodeData grammar... ")
	io.flush()
	
	local tempUnicodeFile = os.tmpfile() .. ".UnicodeData.txt"

	http.download(unicodeDataURL, tempUnicodeFile)

	print("Done")
	io.flush()

	io.write("Parsing... ")
	io.flush()

	local file = io.open(tempUnicodeFile, "r")

	local characters = {}
	local characterSets = {}
	local lowercaseCharacters = {}
	local titlecaseCharacters = {}
	local uppercaseCharacters = {}
	local currentBlock
	local currentBlockStartCodepoint
	local lineIndex = 1

	for line in file:lines() do
		local parts = line:split(";", {strict = true})

		local codepoint = tonumber(parts[1], 16)
		local characterName = parts[2]
		local category = parts[3]
		local direction = parts[5]
		local uppercaseMapping = tonumber(parts[13], 16)
		local lowercaseMapping = tonumber(parts[14], 16)
		local titlecaseMapping = tonumber(parts[15], 16)

		local blockName, blockId = string.match(characterName, "<(.+), (%w+)>")
		if (currentBlock) then
			if (blockId ~= "Last") then
				error("Parsing error: expected last block at line " .. lineIndex)
			end

			print("Detected set " .. blockName .. " from codepoint " .. currentBlockStartCodepoint .. " to " .. codepoint)

			table.insert(characterSets, {
				startCodepoint = currentBlockStartCodepoint,
				endCodepoint = codepoint,
				name = "<" .. blockName .. ">",
				category = category,
				direction = direction
			})

			currentBlock = nil
		else
			if (blockName) then
				if (blockId ~= "First") then
					error("Parsing error: expected first block at line " .. lineIndex)
				end

				currentBlock = blockName
				currentBlockStartCodepoint = codepoint
			else
				table.insert(characters, {
					codepoint = codepoint,
					name = characterName,
					category = category,
					direction = direction,
					upper = uppercaseMapping,
					lower = lowercaseMapping,
					title = titlecaseMapping
				})

				if (lowercaseMapping) then
					table.insert(lowercaseCharacters, {codepoint = codepoint, lower = lowercaseMapping})
				end

				if (titlecaseMapping) then
					table.insert(titlecaseCharacters, {codepoint = codepoint, title = titlecaseMapping})
				end

				if (uppercaseMapping) then
					table.insert(uppercaseCharacters, {codepoint = codepoint, upper = uppercaseMapping})
				end
			end
		end

		lineIndex = lineIndex + 1
	end

	print("Parsed " .. #characters .. " characters")
	io.flush()

	print("Writting Unicode Data to header...")
	io.flush()

	file = io.open("src/Nazara/Core/UnicodeData.hpp", "w+")
	if (not file) then
		error("Failed to open Unicode Data header")
		return
	end

	file:write([[
// this file was automatically generated and should not be edited
// no header guards

// Copyright (C) ]] .. os.date("%Y") .. [[ Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

]])

	file:write(string.format("UnicodeCharacter unicodeCharacters[%d] = {\n", #characters))

	for _, data in pairs(characters) do
		local category = CategoryToString[data.category]
		if (not category) then
			error("Unknown category " .. data.category .. " for character " .. data.codepoint)
		end

		local direction = DirectionToString[data.direction]
		if (not direction) then
			error("Unknown direction " .. data.direction .. " for character " .. data.codepoint)
		end

		file:write(string.format("\t{%d, Unicode::%s, Unicode::%s},\n", data.codepoint, category, direction))
	end
	file:write("};\n\n")

	file:write(string.format("UnicodeSet unicodeSets[%d] = {\n", #characterSets))

	for _, data in pairs(characterSets) do
		local category = CategoryToString[data.category]
		if (not category) then
			error("Unknown category " .. data.category .. " for character " .. data.codepoint)
		end

		local direction = DirectionToString[data.direction]
		if (not direction) then
			error("Unknown direction " .. data.direction .. " for character " .. data.codepoint)
		end

		file:write(string.format("\t{%d, %d, {%d, Unicode::%s, Unicode::%s}},\n", data.startCodepoint, data.endCodepoint, data.startCodepoint, category, direction))
	end
	file:write("};\n\n")

	file:write(string.format("UnicodeCharacterSimpleMapping unicodeLower[%d] = {\n", #lowercaseCharacters))
	for _, data in pairs(lowercaseCharacters) do
		file:write(string.format("\t{%d, %d},\n", data.codepoint, data.lower))
	end
	file:write("};\n\n")

	file:write(string.format("UnicodeCharacterSimpleMapping unicodeTitle[%d] = {\n", #titlecaseCharacters))
	for _, data in pairs(titlecaseCharacters) do
		file:write(string.format("\t{%d, %d},\n", data.codepoint, data.title))
	end
	file:write("};\n\n")

	file:write(string.format("UnicodeCharacterSimpleMapping unicodeUpper[%d] = {\n", #uppercaseCharacters))
	for _, data in pairs(uppercaseCharacters) do
		file:write(string.format("\t{%d, %d},\n", data.codepoint, data.upper))
	end
	file:write("};\n\n")

	file:close()

	print("Succeeded!")
	io.flush()
end)
