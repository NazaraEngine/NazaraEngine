task("generate-doc")

set_menu({
	-- Settings menu usage
	usage = "xmake generate-doc [options]",
	description = "Parses the docgen.json to generate documentation"
})

local log = false

on_run(function ()
    import("core.base.json")

    local docgen = assert(json.decode(io.readfile("documentation/docgen.json")))
    local classFiles = {}
    local typelinks = {}
    for moduleName, module in pairs(docgen.modules) do
        local folder = "documentation/generated" .. "/" .. moduleName
        for _, class in pairs(module.classes) do
            assert(class.name:startswith("Nz::"))
            local classkey = class.name:sub(5)
            local lastsep = classkey:lastof("::")
            local classname = lastsep and classkey:sub(lastsep + 2) or classkey

            local filepath = folder .. "/" .. classkey:gsub("::", ".") .. ".md"
            local classData = {
                fullname = class.name,
                filename = filepath,
                classname = classname,
                class = class
            }
            table.insert(classFiles, classData)
            local classnames = {class.name, classkey}

            local link = string.format("`[`%s`](%s)`", classkey, filepath)
            for _, name in ipairs(classnames) do
                table.insert(typelinks, {
                    key = classData,
                    pattern = "(.?)(" .. name .. ")(.?)",
                    replacement = function (b, n, e)
                        if #b > 0 and not b:match("^[`<%s]") then
                            return
                        end

                        if #e > 0 and not e:match("^[>&*%s]") then
                            return
                        end

                        local r = {}
                        table.insert(r, b)
                        table.insert(r, "`" .. classkey .. "`")
                        table.insert(r, e)

                        return table.concat(r)
                    end
                })
                table.insert(typelinks, {
                    excludes = classData,
                    pattern = "(.?)(" .. name .. ")(.?)",
                    replacement = function (b, n, e)
                        if #b > 0 and not b:match("^[`<%s]") then
                            return
                        end

                        if #e > 0 and not e:match("^[>&*%s]") then
                            return
                        end

                        local r = {}
                        table.insert(r, b)
                        table.insert(r, link)
                        table.insert(r, e)

                        return table.concat(r)
                    end
                })
            end
        end
    end

    table.sort(classFiles, function (a, b)
        return a.fullname < b.fullname
    end)

    local function type(typeStr, key)
        for _, link in pairs(typelinks) do
            if link.key then
                if link.key == key then
                    typeStr = typeStr:gsub(link.pattern, link.replacement)
                end
            elseif link.excludes ~= key then
                typeStr = typeStr:gsub(link.pattern, link.replacement)
            end
        end
        if typeStr:startswith("``") then
            typeStr = typeStr:sub(3)
        end
        if typeStr:endswith("``") then
            typeStr = typeStr:sub(1, -3)
        end
        return typeStr
    end

    for _, classData in pairs(classFiles) do
        os.mkdir(path.directory(classData.filename))
        print("generating " .. classData.fullname .. "...")
        local file = assert(io.open(classData.filename, "w+"))
        file:print("# %s", classData.fullname)
        file:print("")
        file:print("Class description")
        file:print("")
        file:print("## Constructors")
        file:print("")
        for _, constructor in pairs(classData.class.constructors) do
            local params = {}
            for _, param in pairs(constructor.parameters) do
                if #param.name > 0 then
                    table.insert(params, type(param.type, classData) .. " " .. param.name)
                else
                    table.insert(params, type(param.type, classData))
                end
            end
            file:print("- `%s(%s)`", classData.classname, table.concat(params, ", "))
        end
        file:print("")
        file:print("## Methods")
        file:print("")
        file:print("| Return type | Signature |")
        file:print("| ----------- | --------- |")
        for _, method in pairs(classData.class.methods) do
            local params = {}
            for _, param in pairs(method.parameters) do
                if #param.name > 0 then
                    table.insert(params, type(param.type, classData) .. " " .. param.name)
                else
                    table.insert(params, type(param.type, classData))
                end
            end
            file:print("| %s | `%s(%s)` |", type("`" .. method.returnType .. "`", classData), method.name, table.concat(params, ", "))
        end
    end
end)