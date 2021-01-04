// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvSection.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline std::size_t SpirvSection::Append(const char* str)
	{
		return Append(std::string_view(str));
	}

	inline std::size_t SpirvSection::Append(const std::string_view& str)
	{
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

		return offset;
	}

	inline std::size_t SpirvSection::Append(const std::string& str)
	{
		return Append(std::string_view(str));
	}

	inline std::size_t SpirvSection::Append(UInt32 value)
	{
		std::size_t offset = GetOutputOffset();
		m_bytecode.push_back(value);

		return offset;
	}

	inline std::size_t SpirvSection::Append(SpirvOp opcode, const OpSize& wordCount)
	{
		return Append(BuildOpcode(opcode, wordCount.wc));
	}

	inline std::size_t SpirvSection::Append(std::initializer_list<UInt32> codepoints)
	{
		std::size_t offset = GetOutputOffset();

		for (UInt32 cp : codepoints)
			Append(cp);

		return offset;
	}

	inline std::size_t SpirvSection::Append(const SpirvSection& section)
	{
		const std::vector<UInt32>& bytecode = section.GetBytecode();

		std::size_t offset = GetOutputOffset();
		m_bytecode.resize(offset + bytecode.size());
		std::copy(bytecode.begin(), bytecode.end(), m_bytecode.begin() + offset);

		return offset;
	}

	template<typename ...Args>
	std::size_t SpirvSection::Append(SpirvOp opcode, const Args&... args)
	{
		unsigned int wordCount = 1 + (CountWord(args) + ... + 0);
		std::size_t offset = Append(opcode, OpSize{ wordCount });
		if constexpr (sizeof...(args) > 0)
			(Append(args), ...);

		return offset;
	}

	template<typename F> std::size_t SpirvSection::AppendVariadic(SpirvOp opcode, F&& callback)
	{
		std::size_t offset = Append(0); //< Will be filled later

		unsigned int wordCount = 1;
		auto appendFunctor = [&](const auto& value)
		{
			wordCount += CountWord(value);
			Append(value);
		};
		callback(appendFunctor);

		m_bytecode[offset] = BuildOpcode(opcode, wordCount);

		return offset;
	}

	template<typename T, typename>
	std::size_t SpirvSection::Append(T value)
	{
		return Append(static_cast<UInt32>(value));
	}

	template<typename T, typename>
	unsigned int SpirvSection::CountWord(const T& /*value*/)
	{
		return 1;
	}

	template<typename T1, typename T2, typename ...Args>
	unsigned int SpirvSection::CountWord(const T1& value, const T2& value2, const Args&... rest)
	{
		return CountWord(value) + CountWord(value2) + (CountWord(rest) + ...);
	}

	inline unsigned int SpirvSection::CountWord(const char* str)
	{
		return CountWord(std::string_view(str));
	}

	inline unsigned int Nz::SpirvSection::CountWord(const std::string& str)
	{
		return CountWord(std::string_view(str));
	}

	inline unsigned int SpirvSection::CountWord(const Raw& raw)
	{
		return static_cast<unsigned int>((raw.size + sizeof(UInt32) - 1) / sizeof(UInt32));
	}

	inline unsigned int SpirvSection::CountWord(const std::string_view& str)
	{
		return (static_cast<unsigned int>(str.size() + 1) + sizeof(UInt32) - 1) / sizeof(UInt32); //< + 1 for null character
	}

	inline const std::vector<UInt32>& SpirvSection::GetBytecode() const
	{
		return m_bytecode;
	}

	inline std::size_t SpirvSection::GetOutputOffset() const
	{
		return m_bytecode.size();
	}

	inline UInt32 SpirvSection::BuildOpcode(SpirvOp opcode, unsigned int wordCount)
	{
		return UInt32(opcode) | UInt32(wordCount) << 16;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
