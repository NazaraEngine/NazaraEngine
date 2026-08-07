-- defined at the end of the file
local headerTemplate, classHeaderTemplate, fieldTemplate, classFooterTemplate, footerTemplate

local primitiveToType = {
	f32 = "float",
	i32 = "Nz::Int32",
	u32 = "Nz::UInt32"
}

local typeDecoder

local function DecodeType(typeData)
	local decoder = typeDecoder[typeData.kind]
	if not decoder then
		print("unknown kind " .. typeData.kind)
		os.raise("unknown kind " .. typeData.kind)
	end
	return decoder(typeData)
end

typeDecoder = {
	array = function (typeData)
		return string.format("std::array<%s, %u>", DecodeType(typeData.innerType), typeData.length)
	end,
	dynArray = function (typeData)
		return DecodeType(typeData.innerType) .. "*"
	end,
	matrix = function (typeData)
		--assert(typeData.cellType == "f32")
		return string.format("nzsl::Matrix%ux%u%s", typeData.columnCount, typeData.rowCount, typeData.cellType)
	end,
	primitive = function (typeData)
		local typeStr = primitiveToType[typeData.primitiveType]
		if not typeStr then
			os.raise("unknown type")
		end
		return typeStr
	end,
	struct = function (typeData)
	end,
	vector = function (typeData)
		return string.format("nzsl::Vector%u%s", typeData.dims, typeData.baseType)
	end
}

task("reflect-shaders")
	set_menu({
		-- Settings menu usage
		usage = "xmake reflect-shaders [options]",
		description = "Reflect engine shaders",
		options = {
			{'v', "verbose",   "k",  nil,   "Print lots of verbose information for users."              },
			{'D', "diagnosis", "k",  nil,   "Print lots of diagnosis information (backtrace, check info ..) only for developers."}
		}
	})

	on_run(function ()
		import("core.base.option")
		import("core.base.json")
		import("core.base.task")
		import("core.project.project")

		task.run("config", {}, {disable_dump = true})

		local nzsl = project.required_package("nzsl")
		local nzsla = path.join(nzsl:installdir(), "bin", "nzsla")
		local nzslc = path.join(nzsl:installdir(), "bin", "nzslc")

		local envs = nzsl:get("envs")
		if is_plat("mingw") then
			local mingw = toolchain.load("mingw")
			if mingw and mingw:check() then
				for name, value in pairs(mingw:runenvs()) do
					envs[name] = table.join(envs[name] or {}, table.wrap(value))
				end
			end
		end

		print("Reflecting shaders...")

		local reflect = {
			{moduleName = "Renderer", folder = "Shaders", fileName = "DebugDraw", structs = {"ViewerData"}},
			{moduleName = "Graphics", folder = "ShaderArchives/Shaders/Modules/Engine", fileName = "IndirectData", structs = {"DrawIndirectCommand", "DrawIndexedIndirectCommand", }},
			{moduleName = "Graphics", folder = "ShaderArchives/Shaders/Modules/Engine", fileName = "InstanceData", structs = {"InstanceData"}},
			{moduleName = "Graphics", folder = "ShaderArchives/Shaders/Modules/Engine", fileName = "SkeletalData", structs = {"SkeletalData"}},
			{moduleName = "Graphics", folder = "ShaderArchives/Shaders/Modules/Engine", fileName = "ViewerData", structs = {"ViewerData"}},
			{moduleName = "Graphics", folder = "ShaderArchives/Shaders/Modules/Lighting", fileName = "LightData", structs = {"DirectionalLight", "DirectionalLights"}},
		}

		for _, reflectData in pairs(reflect) do
			local args = {"--module=" .. "src/Nazara/" .. reflectData.moduleName .. "/" .. reflectData.folder}
			for _, structName in ipairs(reflectData.structs) do
				table.insert(args, "--reflect=" .. structName)
			end
			local filePathWithoutExt = "src/Nazara/" .. reflectData.moduleName .. "/" .. reflectData.folder .. "/" .. reflectData.fileName
			table.insert(args, "src/Nazara/" .. reflectData.moduleName .. "/" .. reflectData.folder .. "/" .. reflectData.fileName .. ".nzsl")

			os.vrunv(nzslc, args, { envs = envs })
			local reflectedShader = json.loadfile(filePathWithoutExt .. ".nzsl.json")

			local header, err = io.open(filePathWithoutExt .. ".hpp", "w+")
			if (not header) then
				error("Failed to create header file (" .. v.Target .. "): " .. err)
			end

			local fileReplacements = {
				FILENAME = reflectData.fileName,
				UPPER_FILENAME = reflectData.fileName:upper()
			}

			local fileReplacer = function (kw)
				local r = fileReplacements[kw]
				if not r then
					os.raise("missing replacement for " .. kw)
				end

				return r
			end

			header:write((headerTemplate:gsub("%%([%u_]+)%%", fileReplacer)))

			for _, structData in pairs(reflectedShader.structs) do
				local structReplacements = debug.setmetatable({
					STRUCT_NAME = structData.name,
					STRUCT_SIZE = string.format("%u", structData.size),
					STRUCT_ALIGNMENT = string.format("%u", structData.alignment)
				}, { __index = fileReplacements })

				local structReplacer = function (kw)
					local r = structReplacements[kw]
					if not r then
						os.raise("missing replacement for " .. kw)
					end

					return r
				end

				header:write((classHeaderTemplate:gsub("%%([%u_]+)%%", structReplacer)))

				for _, fieldData in pairs(structData.members) do
					local fieldReplacements = debug.setmetatable({
						FIELD_NAME = fieldData.name,
						FIELD_OFFSET = string.format("%u", fieldData.offset),
						FIELD_TYPE = DecodeType(fieldData.type)
					}, { __index = structReplacements })

					local fieldReplacer = function (kw)
						local r = fieldReplacements[kw]
						if not r then
							os.raise("missing replacement for " .. kw)
						end

						return r
					end

					header:write((fieldTemplate:gsub("%%([%u_]+)%%", fieldReplacer)))
				end


				header:write((classFooterTemplate:gsub("%%([%u_]+)%%", structReplacer)))
			end

		header:write((footerTemplate:gsub("%%([%u_]+)%%", fileReplacer)))
		end
	end)

headerTemplate = [[
// this file was automatically generated and should not be edited

#pragma once

#ifndef NAZARA_RENDERER_%UPPER_FILENAME%_REFLECTION_HPP
#define NAZARA_RENDERER_%UPPER_FILENAME%_REFLECTION_HPP

#include <NZSL/Math/Matrix.hpp>
#include <NZSL/Math/Vector.hpp>

namespace Nz::ShaderReflect
{
]]

classHeaderTemplate = [[
	class %STRUCT_NAME%View
	{
		public:
			static constexpr std::size_t Size = %STRUCT_SIZE%;
			static constexpr std::size_t Alignment = %STRUCT_ALIGNMENT%;

			%STRUCT_NAME%View(void* ptr) :
			m_basePtr(static_cast<std::uint8_t*>(ptr))
			{
			}

]]

fieldTemplate = [[
			%FIELD_TYPE%& %FIELD_NAME%() { return *std::launder(reinterpret_cast<%FIELD_TYPE%*>(&m_ptr[%FIELD_OFFSET%])); }
			const %FIELD_TYPE%& %FIELD_NAME%() const { return *std::launder(reinterpret_cast<const %FIELD_TYPE%*>(&m_ptr[%FIELD_OFFSET%])); }

]]

classFooterTemplate = [[
		private:
			std::uint8_t* m_basePtr;
	};

	class %STRUCT_NAME%Storage : public %STRUCT_NAME%View
	{
		public:
			%STRUCT_NAME%Storage() :
			%STRUCT_NAME%View(&m_storage[0])
			{
			}

		private:
			alignas(%STRUCT_NAME%View::Alignment) std::uint8_t m_storage[%STRUCT_NAME%View::Size];
	};
]]

footerTemplate = [[
}

#endif // NAZARA_RENDERER_%UPPER_FILENAME%_REFLECTION_HPP
]]