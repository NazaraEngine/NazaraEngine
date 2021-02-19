ACTION.Name = "UpdateSpirV"
ACTION.Description = "Download and parse the SpirV grammar and generate a .cpp file for it"

local spirvGrammarURI = "https://raw.githubusercontent.com/KhronosGroup/SPIRV-Headers/master/include/spirv/unified1/spirv.core.grammar.json"

ACTION.Function = function()
	io.write("Downloading Spir-V grammar... ")
	local content, resultStr, resultCode = http.get(spirvGrammarURI, {
		headers = { "From: Premake", "Referer: Premake" }
	})

	if (resultCode ~= 200) then
		error("Failed to download SpirV grammar: " .. resultStr)
	end

	print("Done")

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
			local firstId = #operands
			local operandCount = #instruction.operands
			for _, operand in pairs(instruction.operands) do
				table.insert(operands, operand)
			end

			operandByInstruction[instruction.opcode] = { firstId = firstId, count = operandCount }
		end
	end

	local headerFile = io.open("../include/Nazara/Shader/SpirvData.hpp", "w+")
	assert(headerFile, "failed to open Spir-V header")

	headerFile:write([[
// Copyright (C) ]] .. os.date("%Y") .. [[ Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp"

// This file was generated automatically, please do not edit

#pragma once

#ifndef NAZARA_SPIRVDATA_HPP
#define NAZARA_SPIRVDATA_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Flags.hpp>
#include <Nazara/Shader/Config.hpp>

namespace Nz
{
]])

	-- SpirV operations
	headerFile:write([[
	enum class SpirvOp
	{
]])

	for _, instruction in pairs(result.instructions) do
		headerFile:write("\t\t" .. instruction.opname .. " = " .. instruction.opcode .. ",\n")
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
		std::size_t minOperandCount;
	};

]])

	-- Functions signatures
	headerFile:write([[
	NAZARA_SHADER_API const SpirvInstruction* GetInstructionData(UInt16 op);
]])

headerFile:write([[
}

#endif
]])

	local sourceFile = io.open("../src/Nazara/Shader/SpirvData.cpp", "w+")
	assert(sourceFile, "failed to open Spir-V source")

	sourceFile:write([[
// Copyright (C) ]] .. os.date("%Y") .. [[ Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp"

// This file was generated automatically, please do not edit

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

		sourceFile:write([[
			{
				SpirvOp::]] .. instruction.opname .. [[,
				R"(]] .. instruction.opname .. [[)",
				]] .. (opByInstruction and "&s_operands[" .. opByInstruction.firstId .. "]" or "nullptr") .. [[,
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

end
