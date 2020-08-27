// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvPrinter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Shader/SpirvData.hpp>
#include <SpirV/spirv.h>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	struct SpirvPrinter::State
	{
		State(const Settings& Settings) :
		settings(settings)
		{
		}

		const UInt32* codepoints;
		std::size_t index = 0;
		std::size_t count;
		std::ostringstream stream;
		const Settings& settings;
	};

	std::string SpirvPrinter::Print(const UInt32* codepoints, std::size_t count, const Settings& settings)
	{
		State state(settings);
		state.codepoints = codepoints;
		state.count = count;

		m_currentState = &state;
		CallOnExit resetOnExit([&] { m_currentState = nullptr; });

		UInt32 magicNumber = ReadWord();
		if (magicNumber != SpvMagicNumber)
			throw std::runtime_error("invalid Spir-V: magic number didn't match");

		if (m_currentState->settings.printHeader)
			m_currentState->stream << "Spir-V module\n";

		UInt32 versionNumber = ReadWord();
		if (versionNumber > SpvVersion)
			throw std::runtime_error("Spir-V is more recent than printer, dismissing");

		UInt8 majorVersion = ((versionNumber) >> 16) & 0xFF;
		UInt8 minorVersion = ((versionNumber) >>  8) & 0xFF;

		UInt32 generatorId = ReadWord();
		UInt32 bound = ReadWord();
		UInt32 schema = ReadWord();

		if (m_currentState->settings.printHeader)
		{
			m_currentState->stream << "Version " + std::to_string(+majorVersion) << "." << std::to_string(+minorVersion) << "\n";
			m_currentState->stream << "Generator: " << std::to_string(generatorId) << "\n";
			m_currentState->stream << "Bound: " << std::to_string(bound) << "\n";
			m_currentState->stream << "Schema: " << std::to_string(schema) << "\n";
		}

		while (m_currentState->index < m_currentState->count)
			AppendInstruction();

		return m_currentState->stream.str();
	}

	void SpirvPrinter::AppendInstruction()
	{
		std::size_t startIndex = m_currentState->index;

		UInt32 firstWord = ReadWord();

		UInt16 wordCount = static_cast<UInt16>((firstWord >> 16) & 0xFFFF);
		UInt16 opcode    = static_cast<UInt16>(firstWord & 0xFFFF);

		const SpirvInstruction* inst = GetInstructionData(opcode);
		if (!inst)
			throw std::runtime_error("invalid instruction");

		m_currentState->stream << inst->name;

		if (m_currentState->settings.printParameters)
		{
			std::size_t currentOperand = 0;
			std::size_t instructionEnd = startIndex + wordCount;
			while (m_currentState->index < instructionEnd)
			{
				const SpirvInstruction::Operand* operand = &inst->operands[currentOperand];

				m_currentState->stream << " " << operand->name << "(";

				switch (operand->kind)
				{
					case SpirvOperandKind::ImageOperands:
					case SpirvOperandKind::FPFastMathMode:
					case SpirvOperandKind::SelectionControl:
					case SpirvOperandKind::LoopControl:
					case SpirvOperandKind::FunctionControl:
					case SpirvOperandKind::MemorySemantics:
					case SpirvOperandKind::MemoryAccess:
					case SpirvOperandKind::KernelProfilingInfo:
					case SpirvOperandKind::RayFlags:
					case SpirvOperandKind::SourceLanguage:
					case SpirvOperandKind::ExecutionModel:
					case SpirvOperandKind::AddressingModel:
					case SpirvOperandKind::MemoryModel:
					case SpirvOperandKind::ExecutionMode:
					case SpirvOperandKind::StorageClass:
					case SpirvOperandKind::Dim:
					case SpirvOperandKind::SamplerAddressingMode:
					case SpirvOperandKind::SamplerFilterMode:
					case SpirvOperandKind::ImageFormat:
					case SpirvOperandKind::ImageChannelOrder:
					case SpirvOperandKind::ImageChannelDataType:
					case SpirvOperandKind::FPRoundingMode:
					case SpirvOperandKind::LinkageType:
					case SpirvOperandKind::AccessQualifier:
					case SpirvOperandKind::FunctionParameterAttribute:
					case SpirvOperandKind::Decoration:
					case SpirvOperandKind::BuiltIn:
					case SpirvOperandKind::Scope:
					case SpirvOperandKind::GroupOperation:
					case SpirvOperandKind::KernelEnqueueFlags:
					case SpirvOperandKind::Capability:
					case SpirvOperandKind::RayQueryIntersection:
					case SpirvOperandKind::RayQueryCommittedIntersectionType:
					case SpirvOperandKind::RayQueryCandidateIntersectionType:
					case SpirvOperandKind::IdResultType:
					case SpirvOperandKind::IdResult:
					case SpirvOperandKind::IdMemorySemantics:
					case SpirvOperandKind::IdScope:
					case SpirvOperandKind::IdRef:
					case SpirvOperandKind::LiteralInteger:
					case SpirvOperandKind::LiteralExtInstInteger:
					case SpirvOperandKind::LiteralSpecConstantOpInteger:
					case SpirvOperandKind::LiteralContextDependentNumber: //< FIXME
					{
						UInt32 value = ReadWord();
						m_currentState->stream << value;
						break;
					}

					case SpirvOperandKind::LiteralString:
					{
						std::string str = ReadString();
						m_currentState->stream << "\"" << str << "\"";

						/*
						std::size_t offset = GetOutputOffset();

						std::size_t size4 = CountWord(str);
						for (std::size_t i = 0; i < size4; ++i)
						{
							UInt32 codepoint = 0;
							for (std::size_t j = 0; j < 4; ++j)
							{
								std::size_t pos = i * 4 + j;
								if (pos < str.size())
									codepoint |= UInt32(str[pos]) << (j * 8);
							}

							Append(codepoint);
						}
						*/
						break;
					}

				
					case SpirvOperandKind::PairLiteralIntegerIdRef:
					{
						ReadWord();
						ReadWord();
						break;
					}

					case SpirvOperandKind::PairIdRefLiteralInteger:
					{
						ReadWord();
						ReadWord();
						break;
					}

					case SpirvOperandKind::PairIdRefIdRef:
					{
						ReadWord();
						ReadWord();
						break;
					}

					/*case SpirvOperandKind::LiteralContextDependentNumber:
					{
						throw std::runtime_error("not yet implemented");
					}*/

					default:
						break;

				}

				m_currentState->stream << ")";

				if (currentOperand < inst->minOperandCount - 1)
					currentOperand++;
			}
		}
		else
		{
			m_currentState->index += wordCount - 1;
			if (m_currentState->index > m_currentState->count)
				throw std::runtime_error("unexpected end of stream");
		}

		m_currentState->stream << "\n";

		assert(m_currentState->index == startIndex + wordCount);
	}

	std::string SpirvPrinter::ReadString()
	{
		std::string str;

		for (;;)
		{
			UInt32 value = ReadWord();
			for (std::size_t j = 0; j < 4; ++j)
			{
				char c = static_cast<char>((value >> (j * 8)) & 0xFF);
				if (c == '\0')
					return str;

				str.push_back(c);
			}
		}
	}

	UInt32 SpirvPrinter::ReadWord()
	{
		if (m_currentState->index >= m_currentState->count)
			throw std::runtime_error("unexpected end of stream");

		return m_currentState->codepoints[m_currentState->index++];
	}
}
