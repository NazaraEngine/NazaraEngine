local spirvGrammarURI = "https://raw.githubusercontent.com/KhronosGroup/SPIRV-Headers/master/include/spirv/unified1/spirv.core.grammar.json"

task("update-spirv")

set_menu({
	-- Settings menu usage
	usage = "xmake update-spirv [options]",
	description = "Download and parse the SpirV grammar and updates the shader module files with it"
})

on_run(function()
	import("core.base.json")
	import("net.http")

	io.write("Downloading Spir-V grammar... ")
	io.flush()
	
	local tempGrammar = os.tmpfile() .. ".spirv.core.grammar.json"

	http.download(spirvGrammarURI, tempGrammar)
	
	print("Done")
	io.flush()

	io.write("Parsing... ")
	io.flush()

	local content = io.readfile(tempGrammar)

	local result, err = json.decode(content)
	assert(result, err)

	local instructions = {}
	local instructionById = {}
	for _, instruction in pairs(result.instructions) do
		local duplicateId = instructionById[instruction.opcode]
		if (duplicateId == nil) then
			table.insert(instructions, instruction)
			instructionById[instruction.opcode] = #instructions
		else
			instructions[duplicateId] = instruction
		end
	end

	local operands = {}
	local operandByInstruction = {}
	for _, instruction in pairs(instructions) do
		if (instruction.operands) then
			local resultId
			local firstId = #operands
			local operandCount = #instruction.operands
			for i, operand in pairs(instruction.operands) do
				table.insert(operands, operand)

				if (operand.kind == "IdResult") then
					assert(not resultId, "unexpected operand with two IdResult")
					resultId = i - 1
				end
			end

			operandByInstruction[instruction.opcode] = { firstId = firstId, count = operandCount, resultId = resultId }
		end
	end

	print("Done")
	io.flush()

	io.write("Generating... ")
	io.flush()

	local headerFile = io.open("include/Nazara/Shader/SpirvData.hpp", "w+")
	assert(headerFile, "failed to open Spir-V header")

	headerFile:write([[
// Copyright (C) ]] .. os.date("%Y") .. [[ Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// this file was automatically generated and should not be edited

#pragma once

#ifndef NAZARA_SHADER_SPIRVDATA_HPP
#define NAZARA_SHADER_SPIRVDATA_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Flags.hpp>
#include <Nazara/Shader/Config.hpp>

namespace Nz
{
]])

	local magicNumber = result.magic_number
	local majorVersion = assert(math.tointeger(result.major_version), "expected integer major version number")
	local minorVersion = assert(math.tointeger(result.minor_version), "expected integer minor version number")
	local revision = assert(math.tointeger(result.revision), "expected integer revision number")

	headerFile:write([[
	constexpr UInt32 SpirvMagicNumber = ]] .. magicNumber .. [[;
	constexpr UInt32 SpirvMajorVersion = ]] .. majorVersion .. [[;
	constexpr UInt32 SpirvMinorVersion = ]] .. minorVersion .. [[;
	constexpr UInt32 SpirvRevision = ]] .. revision .. [[;
	constexpr UInt32 SpirvVersion = (SpirvMajorVersion << 16) | (SpirvMinorVersion << 8);

]])

	-- SpirV operations
	headerFile:write([[
	enum class SpirvOp
	{
]])

	for _, instruction in pairs(result.instructions) do
		local value = assert(math.tointeger(instruction.opcode), "unexpected non-integer in opcode")
		headerFile:write("\t\t" .. instruction.opname .. " = " .. value .. ",\n")
	end

headerFile:write([[
	};

]])

	-- SpirV operands
	headerFile:write([[
	enum class SpirvOperandKind
	{
]])
	
	for _, operand in pairs(result.operand_kinds) do
		headerFile:write("\t\t" .. operand.kind .. ",\n")
	end

	headerFile:write([[
	};

]])

	-- SpirV enums
	for _, operand in pairs(result.operand_kinds) do
		if (operand.category == "ValueEnum" or operand.category == "BitEnum") then
			local enumName = "Spirv" .. operand.kind
			headerFile:write([[
	enum class ]] .. enumName .. [[

	{
]])

			local maxName
			local maxValue
			for _, enumerant in pairs(operand.enumerants) do
				local value = enumerant.value
				if type(enumerant.value) ~= "string" then -- power of two are written as strings
					value = assert(math.tointeger(value), "unexpected non-integer in enums")
				end

				local eName = enumerant.enumerant:match("^%d") and operand.kind .. enumerant.enumerant or enumerant.enumerant
				headerFile:write([[
		]] .. eName .. [[ = ]] .. value .. [[,
]])

				if (not maxValue or value > maxValue) then
					maxName = eName
				end
			end

			headerFile:write([[
	};

]])
			if (operand.category == "BitEnum") then
				headerFile:write([[
	template<>
	struct EnumAsFlags<]] .. enumName .. [[>
	{
		static constexpr ]] .. enumName .. [[ max = ]] .. enumName .. "::" .. maxName .. [[;

		static constexpr bool AutoFlag = false;
	};


]])
			end
		end
	end

	-- Struct
	headerFile:write([[
	struct SpirvInstruction
	{
		struct Operand
		{
			SpirvOperandKind kind;
			const char* name;
		};

		SpirvOp op;
		const char* name;
		const Operand* operands;
		const Operand* resultOperand;
		std::size_t minOperandCount;
	};

]])

	-- Functions signatures
	headerFile:write([[
	NAZARA_SHADER_API const SpirvInstruction* GetInstructionData(UInt16 op);
]])

headerFile:write([[
}

#endif // NAZARA_SHADER_SPIRVDATA_HPP
]])

	headerFile:close()

	local sourceFile = io.open("src/Nazara/Shader/SpirvData.cpp", "w+")
	assert(sourceFile, "failed to open Spir-V source")

	sourceFile:write([[
// Copyright (C) ]] .. os.date("%Y") .. [[ Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// this file was automatically generated and should not be edited

#include <Nazara/Shader/SpirvData.hpp>
#include <algorithm>
#include <array>
#include <cassert>

namespace Nz
{
	static constexpr std::array<SpirvInstruction::Operand, ]] .. #operands .. [[> s_operands = {
		{
]])
	for _, operand in pairs(operands) do
		sourceFile:write([[
			{
				SpirvOperandKind::]] .. operand.kind .. [[,
				R"(]] .. (operand.name or operand.kind) .. [[)"
			},
]])
	end

	sourceFile:write([[
		}
	};

	static std::array<SpirvInstruction, ]] .. #instructions .. [[> s_instructions = {
		{
]])

	for _, instruction in pairs(instructions) do
		local opByInstruction = operandByInstruction[instruction.opcode]
		local resultId = opByInstruction and opByInstruction.resultId or nil

		sourceFile:write([[
			{
				SpirvOp::]] .. instruction.opname .. [[,
				R"(]] .. instruction.opname .. [[)",
				]] .. (opByInstruction and "&s_operands[" .. opByInstruction.firstId .. "]" or "nullptr") .. [[,
				]] .. (resultId and "&s_operands[" .. opByInstruction.firstId + resultId .. "]" or "nullptr") .. [[,
				]] .. (opByInstruction and opByInstruction.count or "0") .. [[,
			},
]])
	end

	sourceFile:write([[
		}
	};

]])

	-- Operand to string
	sourceFile:write([[
	const SpirvInstruction* GetInstructionData(UInt16 op)
	{
		auto it = std::lower_bound(std::begin(s_instructions), std::end(s_instructions), op, [](const SpirvInstruction& inst, UInt16 op) { return UInt16(inst.op) < op; });
		if (it != std::end(s_instructions) && UInt16(it->op) == op)
			return &*it;
		else
			return nullptr;
	}
]])

	sourceFile:write([[
}
]])

	sourceFile:close()

	print("Done")
	io.flush()
end)
