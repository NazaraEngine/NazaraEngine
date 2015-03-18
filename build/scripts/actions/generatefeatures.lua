function PrintTable ( t, indent, done )
	done = done or {}
	indent = indent or 0

	local txt = {}
	for key, value in pairs (t) do
		table.insert(txt, string.rep (" ", indent))
		if (type(value) == "table" and not done[value]) then
			done [value] = true
			table.insert(txt, tostring(key) .. ":" .. "\n")
			table.insert(txt, PrintTable (value, indent + 2, done))
		else
			table.insert(txt, tostring (key) .. "\t=\t" )
			table.insert(txt, tostring(value) .. "\n" )
		end
	end
	
	return table.concat(txt)
end

Feature = {}

-- Tables de vérité
local andTable =
{ 
   {0,0},
   {0,1},
}
local orTable =
{ 
   {0,1},
   {1,1},
}

local xorTable =
{ 
   {0,1},
   {1,0},
}

local bitFunc = function (a, b, truthTable)
	local power = 1
	local c = 0
	while (a > 0 or b > 0) do
		c = c + (truthTable[(a % 2)+1][(b % 2)+1] * power)
		a = math.floor(a/2)
		b = math.floor(b/2)
		power = power * 2
	end

	return c
end

function Feature.AND(a, b)
	return bitFunc(a, b, andTable)
end

function Feature.OR(a, b)
	return bitFunc(a, b, orTable)
end

function Feature.XOR(a, b)
	return bitFunc(a, b, xorTable)
end

Feature.NotApplicable = 0
Feature.Windows       = 2 ^ 0 
Feature.Linux         = 2 ^ 1
Feature.MacOSX        = 2 ^ 2
Feature.RaspberryPi   = 2 ^ 3
Feature.POSIX         = Feature.Linux + Feature.MacOSX + Feature.RaspberryPi

function Feature.CompleteData(tab, requiredPortability)
	if (not requiredPortability) then
		assert(tab.RequiredPortability)
		requiredPortability = tab.RequiredPortability
	elseif (tab.RequiredPortability) then
		requiredPortability = Feature.OR(requiredPortability, tab.RequiredPortability)
	end
	
	tab.RequiredPortability = requiredPortability

	if (not tab.Portability) then
		tab.Portability = Feature.NotApplicable
	end

	if (tab.Features) then
		local acc = 0
		for k,v in pairs(tab.Features) do
			if (type(v) == "string") then
				v = {Title = v}
				tab.Features[k] = v
			end

			Feature.CompleteData(v, requiredPortability)

			v.Progress = v.Progress or 100
			
			acc = acc + v.Progress
		end

		tab.Progress = acc/#tab.Features
	end
end

function Feature.Generate()
	local files = os.matchfiles("scripts/features/*.lua")
	
	local modules = {}
	
	for k, filePath in pairs(files) do
		local moduleName = filePath:match(".*/(.*).lua")

		local data = dofile(filePath)
		Feature.CompleteData(data)

		modules[moduleName] = data
	end
	
	local content = {}

	local contentType = 
	{
		["(%s*)%%MODULELIST%%"] = Feature.GenerateModuleList,
		["(%s*)%%MODULEDESCRIPTION%%"] = Feature.GenerateModuleDescriptions,
		["(%s*)%%DATE%%"] = function (dontcare, space, content)
			table.insert(content, string.format("%s%s", space, os.date("%d/%m/%Y")))
		end,
	}
	
	local index = io.open("scripts/features/index_template.html")
	for line in index:lines() do
		local matched = false
		for k,v in pairs(contentType) do
			local space = line:match(k)
			if (space) then
				matched = true
				v(modules, space, content)
				break
			end
		end

		if (not matched) then
			table.insert(content, line)
		end
	end
	
	io.open("scripts/features/index.html", "w+"):write(table.concat(content, "\n"))
end

function Feature.Interpolate(from, to, p)
	return from + (to - from)*p
end

function Feature.ComputeColor(progress)
	local stable = {0, 200, 0}
	local partial = {255, 127, 0}
	local unusable = {255, 0, 0}

	local a, b, p
	if (progress < 20) then
		a = unusable
		b = partial
		p = progress/20.0
	else
		a = partial
		b = stable
		p = math.min(20 * 1.020321705^(progress - 20), 100.0)/100.0 -- Je me complique certainement la vie pour ce qui est d'avoir une interpolation exponentielle, mais ça remonte tout ça ...
	end

	local color = {nil, nil, nil}
	for i = 1, 3 do
		color[i] = Feature.Interpolate(a[i], b[i], p)
	end

	return color
end

function Feature.GenerateModuleList(modules, space, content)
	for k,v in pairs(modules) do
		local c = Feature.ComputeColor(v.Progress)

		table.insert(content, string.format([[%s<tr>]], space))
		table.insert(content, string.format([[%s	<td><a href="#%s">%s</a></td>]], space, k, v.Title))
		table.insert(content, string.format([[%s	<td style="color: rgb(%d, %d, %d);">%d%%</td>]], space, c[1], c[2], c[3], v.Progress))
		table.insert(content, string.format([[%s</tr>]], space))
	end
end

function Feature.GenerateModuleDescriptions(modules, space, content)
	for k,v in pairs(modules) do
		table.insert(content, string.format([[%s<div class="module">]], space))
		table.insert(content, string.format([[%s	<hr />]], space, k, v.Title))
		table.insert(content, string.format([[%s	<span id="%s" class="modulename">%s (%s) : %d%%</span>]], space, k, v.Title, v.LibName, math.floor(v.Progress)))
		
		table.insert(content, string.format([[%s	<h4>Fonctionnalités:</h4>]], space))
		Feature.GenerateFeatureList(v.Features, space .. "\t\t", content)

		table.insert(content, string.format([[%s</div>]], space))
	end
end

function Feature.GenerateFeatureList(featureTable, space, content)
	table.insert(content, string.format("%s<ul>", space))
	for k,v in pairs(featureTable) do
		local progress = v.Progress
		local c = Feature.ComputeColor(progress)
		local desc = (progress == 100) and "" or string.format(" (%d%%)", math.floor(progress))

		table.insert(content, string.format("%s	<li>", space))
		table.insert(content, string.format([[%s		<span style="color: rgb(%d, %d, %d);">%s%s</span>]], space, c[1], c[2], c[3], v.Title, desc))

		if (v.Description) then
			table.insert(content, string.format([[%s		<br><span class="description">%s</span>]], space, v.Description))
		end

		if (v.Features) then
			Feature.GenerateFeatureList(v.Features, space .. "\t\t\t", content)
		end

		if (v.Note) then
			table.insert(content, string.format([[%s			<br><span class="note">Note: <span class="notedesc">%s</span></span>]], space, v.Note))
		end
		
		if (v.Portability ~= Feature.NotApplicable and Feature.AND(v.Portability, v.RequiredPortability) ~= v.RequiredPortability) then
			table.insert(content, string.format([[%s		<br><span class="portability">Il manque une implémentation sur au moins un des OS supportés</span>]], space))
		end

		table.insert(content, string.format("%s	</li>", space))
	end
	table.insert(content, string.format("%s</ul>", space))
end

newaction
{
	trigger     = "generatefeatures",
	description = "Generate a web page describing each module's feature",
	execute     = Feature.Generate
}
