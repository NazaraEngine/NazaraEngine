// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvDecoder.hpp>
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
	void SpirvDecoder::Decode(const UInt32* codepoints, std::size_t count)
	{
		m_currentCodepoint = codepoints;
		m_codepointEnd = codepoints + count;

		UInt32 magicNumber = ReadWord();
		if (magicNumber != SpirvMagicNumber)
			throw std::runtime_error("invalid Spir-V: magic number didn't match");

		UInt32 versionNumber = ReadWord();
		if (versionNumber > SpirvVersion)
			throw std::runtime_error("Spir-V is more recent than decoder, dismissing");

		SpirvHeader header;
		header.generatorId = ReadWord();
		header.bound = ReadWord();
		header.schema = ReadWord();
		header.versionNumber = versionNumber;

		if (!HandleHeader(header))
			return;

		while (m_currentCodepoint < m_codepointEnd)
		{
			const UInt32* instructionBegin = m_currentCodepoint;

			UInt32 firstWord = ReadWord();

			auto wordCount = static_cast<UInt16>((firstWord >> 16) & 0xFFFF);
			auto opcode = static_cast<UInt16>(firstWord & 0xFFFF);

			const SpirvInstruction* inst = GetInstructionData(opcode);
			if (!inst)
				throw std::runtime_error("invalid instruction");

			if (!HandleOpcode(*inst, wordCount))
				break;

			m_currentCodepoint = instructionBegin + wordCount;
		}
	}

	bool SpirvDecoder::HandleHeader(const SpirvHeader& /*header*/)
	{
		return true;
	}

	std::string SpirvDecoder::ReadString()
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

	UInt32 SpirvDecoder::ReadWord()
	{
		if (m_currentCodepoint >= m_codepointEnd)
			throw std::runtime_error("unexpected end of stream");

		return *m_currentCodepoint++;
	}
}
