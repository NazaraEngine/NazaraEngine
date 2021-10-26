// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvSectionBase.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline std::size_t SpirvSectionBase::Append(SpirvOp opcode, const OpSize& wordCount)
	{
		return AppendRaw(BuildOpcode(opcode, wordCount.wc));
	}

	template<typename... Args>
	std::size_t SpirvSectionBase::Append(SpirvOp opcode, const Args&... args)
	{
		unsigned int wordCount = 1 + (CountWord(args) + ... + 0);
		std::size_t offset = Append(opcode, OpSize{ wordCount });
		if constexpr (sizeof...(args) > 0)
			(AppendRaw(args), ...);

		return offset;
	}

	template<typename F> std::size_t SpirvSectionBase::AppendVariadic(SpirvOp opcode, F&& callback)
	{
		std::size_t offset = AppendRaw(0); //< Will be filled later

		unsigned int wordCount = 1;
		auto appendFunctor = [&](const auto& value)
		{
			wordCount += CountWord(value);
			AppendRaw(value);
		};
		callback(appendFunctor);

		m_bytecode[offset] = BuildOpcode(opcode, wordCount);

		return offset;
	}

	inline std::size_t SpirvSectionBase::AppendRaw(const char* str)
	{
		return AppendRaw(std::string_view(str));
	}

	inline std::size_t SpirvSectionBase::AppendRaw(const std::string_view& str)
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

			AppendRaw(codepoint);
		}

		return offset;
	}

	inline std::size_t SpirvSectionBase::AppendRaw(const std::string& str)
	{
		return AppendRaw(std::string_view(str));
	}

	inline std::size_t SpirvSectionBase::AppendRaw(UInt32 value)
	{
		std::size_t offset = GetOutputOffset();
		m_bytecode.push_back(value);

		return offset;
	}

	inline std::size_t SpirvSectionBase::AppendRaw(std::initializer_list<UInt32> codepoints)
	{
		std::size_t offset = GetOutputOffset();

		for (UInt32 cp : codepoints)
			AppendRaw(cp);

		return offset;
	}

	inline std::size_t SpirvSectionBase::AppendSection(const SpirvSectionBase& section)
	{
		const std::vector<UInt32>& bytecode = section.GetBytecode();

		std::size_t offset = GetOutputOffset();
		m_bytecode.resize(offset + bytecode.size());
		std::copy(bytecode.begin(), bytecode.end(), m_bytecode.begin() + offset);

		return offset;
	}

	template<typename T, typename>
	std::size_t SpirvSectionBase::AppendRaw(T value)
	{
		return AppendRaw(static_cast<UInt32>(value));
	}

	template<typename T, typename>
	unsigned int SpirvSectionBase::CountWord(const T& /*value*/)
	{
		return 1;
	}

	template<typename T1, typename T2, typename ...Args>
	unsigned int SpirvSectionBase::CountWord(const T1& value, const T2& value2, const Args&... rest)
	{
		return CountWord(value) + CountWord(value2) + (CountWord(rest) + ...);
	}

	inline unsigned int SpirvSectionBase::CountWord(const char* str)
	{
		return CountWord(std::string_view(str));
	}

	inline unsigned int Nz::SpirvSectionBase::CountWord(const std::string& str)
	{
		return CountWord(std::string_view(str));
	}

	inline unsigned int SpirvSectionBase::CountWord(const Raw& raw)
	{
		return static_cast<unsigned int>((raw.size + sizeof(UInt32) - 1) / sizeof(UInt32));
	}

	inline unsigned int SpirvSectionBase::CountWord(const std::string_view& str)
	{
		return (static_cast<unsigned int>(str.size() + 1) + sizeof(UInt32) - 1) / sizeof(UInt32); //< + 1 for null character
	}

	inline const std::vector<UInt32>& SpirvSectionBase::GetBytecode() const
	{
		return m_bytecode;
	}

	inline std::size_t SpirvSectionBase::GetOutputOffset() const
	{
		return m_bytecode.size();
	}

	inline UInt32 SpirvSectionBase::BuildOpcode(SpirvOp opcode, unsigned int wordCount)
	{
		return UInt32(opcode) | UInt32(wordCount) << 16;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
