// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvPrinter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Shader/SpirvData.hpp>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	struct SpirvPrinter::State
	{
		State(const Settings& s) :
		settings(s)
		{
		}

		std::size_t resultOffset;
		std::ostringstream stream;
		const Settings& settings;
	};

	std::string SpirvPrinter::Print(const UInt32* codepoints, std::size_t count, const Settings& settings)
	{
		State state(settings);

		m_currentState = &state;
		CallOnExit resetOnExit([&] { m_currentState = nullptr; });

		Decode(codepoints, count);

		return m_currentState->stream.str();
	}

	bool SpirvPrinter::HandleHeader(const SpirvHeader& header)
	{
		UInt8 majorVersion = ((header.versionNumber) >> 16) & 0xFF;
		UInt8 minorVersion = ((header.versionNumber) >> 8) & 0xFF;

		m_currentState->resultOffset = std::snprintf(nullptr, 0, "%%%u = ", header.bound);

		if (m_currentState->settings.printHeader)
		{
			m_currentState->stream << "Version " + std::to_string(+majorVersion) << "." << std::to_string(+minorVersion) << "\n";
			m_currentState->stream << "Generator: " << std::to_string(header.generatorId) << "\n";
			m_currentState->stream << "Bound: " << std::to_string(header.bound) << "\n";
			m_currentState->stream << "Schema: " << std::to_string(header.schema) << "\n";
		}

		return true;
	}

	bool SpirvPrinter::HandleOpcode(const SpirvInstruction& instruction, UInt32 wordCount)
	{
		const UInt32* startPtr = GetCurrentPtr();

		if (m_currentState->settings.printParameters)
		{
			std::ostringstream instructionStream;
			instructionStream << instruction.name;

			UInt32 resultId = 0;

			std::size_t currentOperand = 0;
			const UInt32* endPtr = startPtr + wordCount - 1;
			while (GetCurrentPtr() < endPtr)
			{
				const SpirvInstruction::Operand* operand = &instruction.operands[currentOperand];

				if (operand->kind != SpirvOperandKind::IdResult)
				{
					switch (operand->kind)
					{
						case SpirvOperandKind::IdRef:
						case SpirvOperandKind::IdResultType:
						case SpirvOperandKind::IdMemorySemantics:
						case SpirvOperandKind::IdScope:
						{
							UInt32 value = ReadWord();
							instructionStream << " %" << value;
							break;
						}

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
						case SpirvOperandKind::LiteralExtInstInteger:
						case SpirvOperandKind::LiteralSpecConstantOpInteger:
						case SpirvOperandKind::LiteralContextDependentNumber: //< FIXME
						{
							UInt32 value = ReadWord();
							instructionStream << " " << operand->name << "(" << value << ")";
							break;
						}

						case SpirvOperandKind::LiteralInteger:
						{
							UInt32 value = ReadWord();
							instructionStream << " " << value;
							break;
						}

						case SpirvOperandKind::LiteralString:
						{
							std::string str = ReadString();
							instructionStream << " \"" << str << "\"";

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
				}
				else
					resultId = ReadWord();

				if (currentOperand < instruction.minOperandCount - 1)
					currentOperand++;
			}

			if (resultId != 0)
			{
				std::string resultInfo = "%" + std::to_string(resultId) + " = ";
				m_currentState->stream << std::setw(m_currentState->resultOffset) << resultInfo;
			}
			else
				m_currentState->stream << std::string(m_currentState->resultOffset, ' ');

			m_currentState->stream << instructionStream.str();

			assert(GetCurrentPtr() == startPtr + wordCount - 1);
		}
		else
			m_currentState->stream << instruction.name;

		m_currentState->stream << "\n";

		return true;
	}
}
