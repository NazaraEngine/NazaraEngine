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
	DirectionToString["L"]	 = "Direction_Left_To_Right"
	DirectionToString["LRE"] = "Direction_Left_To_Right_Embedding"
	DirectionToString["LRO"] = "Direction_Left_To_Right_Override"
	DirectionToString["NSM"] = "Direction_Nonspacing_Mark"
	DirectionToString["ON"]	 = "Direction_Other_Neutral"
	DirectionToString["B"]	 = "Direction_Paragraph_Separator"
	DirectionToString["PDF"] = "Direction_Pop_Directional_Format"
	DirectionToString["R"]	 = "Direction_Right_To_Left"
	DirectionToString["RLE"] = "Direction_Right_To_Left_Embedding"
	DirectionToString["RLO"] = "Direction_Right_To_Left_Override"
	DirectionToString["S"]	 = "Direction_Segment_Separator"
	DirectionToString["WS"]  = "Direction_White_Space"
	
table.maxn = table.maxn or function (tab) -- Compatibilité Lua 5.2
	local maxIndex = 0
	for k,v in pairs(tab) do
		if (k > maxIndex) then
			maxIndex = k
		end
	end
end
	
function getCharacter(tab, first, index)
	local character = {}
	character.Category  = CategoryToString[tab[3]] or "Category_NoCategory"
	character.Direction = DirectionToString[tab[5]] or error("Direction not recognized")
	character.LowerCase = (string.len(tab[14]) ~= 0 and (tonumber(tab[14], 16)-first)) or index
	character.UpperCase = (string.len(tab[13]) ~= 0 and (tonumber(tab[13], 16)-first)) or index
	character.TitleCase = (string.len(tab[15]) ~= 0 and (tonumber(tab[15], 16)-first)) or character.UpperCase
	
	return character
end

function parseUnicodeData()
	local unicodeSet = {}

	file = io.open ("scripts/data/UnicodeData.txt", "r")
	if (not file) then
		error("Unable to open Unicode Data file")
		return
	end

	local t1 = os.clock()
	print("Parsing UnicodeData.txt...")
	local first = 0
	local last = 0
	unicodeSet[0] = {}
	unicodeSet[0].First = 0
	unicodeSet[0].Characters = {}
	local currentSet = 0
	local inblock = false
	local blockData = nil
	local unusedIndex = 0
	local c = 0
	for line in file:lines() do
		local old = 0
		local start = string.find(line, ';', old)
		local tab = {}
		while (start) do
			tab[#tab+1] = string.sub(line, old, start-1, old)
			old = start+1
			start = string.find(line, ';', old)
		end
		tab[#tab+1] = string.sub(line, old)
		
		local index = tonumber(tab[1], 16)
		if (index > 0 and not inblock) then
			if (index-last > 1000) then
				unicodeSet[currentSet].Last = last
				currentSet = currentSet + 1
				unicodeSet[currentSet] = {}
				unicodeSet[currentSet].First = index
				unicodeSet[currentSet].Characters = {}
				print("Set detected (Begin at " .. first .. ", end at " .. last .. ")")
				first = index
			else
				unusedIndex = unusedIndex + index-last-1
			end
		end
		
		local blockName, blockId = string.match(tab[2], "<(.+), (%w+)>")
		if (blockName ~= nil and blockId ~= nil) then
			if (blockId == "First") then
				if (inblock) then
					error("Already in block (" .. tab[1] .. ")")
				end
				inblock = true
				blockCharacter = getCharacter(tab, first)
			elseif (blockId == "Last") then
				if (not inblock) then
					error("Not in block (" .. tab[1] .. ")")
				end
				inblock = false
				for i=first, index do
					unicodeSet[currentSet].Characters[i] = getCharacter(tab, first, i)
				end
			end
		end

		unicodeSet[currentSet].Characters[index - first] = getCharacter(tab, first, index)
		if (unicodeSet[currentSet].Characters[index - first].LowerCase ~= (index - first) or 
			unicodeSet[currentSet].Characters[index - first].UpperCase ~= (index - first) or
			unicodeSet[currentSet].Characters[index - first].TitleCase ~= (index - first)) then
			c = c + 1
		end

		last = index
	end
	unicodeSet[currentSet].Last = last
	print("Set detected (Begin at " .. first .. ", end at " .. last .. ")")
	file:close()

	print("Parsed " .. last+1 .. " characters in " .. #unicodeSet .. " sets, " .. unusedIndex .. " unused indices (took " .. os.difftime(os.clock(), t1) .. " sec)")

	file = io.open("../src/Nazara/Core/UnicodeData.hpp", "w+")
	if (not file) then
		error("Unable to create Unicode Data header")
		return
	end

	print("Writting Unicode Data to header...")
	
	t1 = os.clock()
	for i=0, #unicodeSet do
		local maxn = table.maxn(unicodeSet[i].Characters)
		file:write(string.format("Character unicodeSet%d[%d] = {\n", i, maxn+1))

		for j=0, maxn do
			local v = unicodeSet[i].Characters[j]
			if (v) then
				file:write(string.format("\t{%s,%s,%d,%d,%d},\n", v.Category, v.Direction, v.LowerCase, v.TitleCase, v.UpperCase))
			else
				file:write(string.format("\t{Category_NoCategory,Direction_Boundary_Neutral,%d,%d,%d},\n", j, j, j))
			end
		end
		
		file:write("};\n\n")
	end
	file:close()

	print("Took " .. os.difftime(os.clock(), t1) .. "sec.")
end
--print(string.match("<Plane 15 Private Use, First>", "<.+, (%w+)>"))

newaction
{
	trigger     = "unicode",
	description = "Parse the Unicode Character Data and put the useful informations into a header",
	execute     = parseUnicodeData
}