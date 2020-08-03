// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/SpirvWriter.hpp>
#include <cassert>
#include <type_traits>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline std::size_t SpirvWriter::Section::Append(const char* str)
	{
		return Append(std::string_view(str));
	}

	inline std::size_t SpirvWriter::Section::Append(const std::string_view& str)
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

	inline std::size_t SpirvWriter::Section::Append(const std::string& str)
	{
		return Append(std::string_view(str));
	}

	inline std::size_t SpirvWriter::Section::Append(UInt32 value)
	{
		std::size_t offset = GetOutputOffset();
		data.push_back(value);

		return offset;
	}

	inline std::size_t SpirvWriter::Section::Append(std::initializer_list<UInt32> codepoints)
	{
		std::size_t offset = GetOutputOffset();

		for (UInt32 cp : codepoints)
			Append(cp);

		return offset;
	}

	template<typename ...Args>
	std::size_t SpirvWriter::Section::Append(Opcode opcode, const Args&... args)
	{
		unsigned int wordCount = 1 + (CountWord(args) + ... + 0);
		std::size_t offset = Append(opcode, WordCount{ wordCount });
		if constexpr (sizeof...(args) > 0)
			(Append(args), ...);

		return offset;
	}

	template<typename T>
	std::size_t SpirvWriter::Section::Append(T value)
	{
		return Append(static_cast<UInt32>(value));
	}

	template<typename T>
	unsigned int SpirvWriter::Section::CountWord(const T& value)
	{
		return 1;
	}

	template<typename T1, typename T2, typename ...Args>
	unsigned int SpirvWriter::Section::CountWord(const T1& value, const T2& value2, const Args&... rest)
	{
		return CountWord(value) + CountWord(value2) + (CountWord(rest) + ...);
	}

	inline unsigned int SpirvWriter::Section::CountWord(const char* str)
	{
		return CountWord(std::string_view(str));
	}

	inline unsigned int Nz::SpirvWriter::Section::CountWord(const std::string& str)
	{
		return CountWord(std::string_view(str));
	}

	inline unsigned int SpirvWriter::Section::CountWord(const std::string_view& str)
	{
		return (static_cast<unsigned int>(str.size() + 1) + sizeof(UInt32) - 1) / sizeof(UInt32); //< + 1 for null character
	}

	std::size_t SpirvWriter::Section::GetOutputOffset() const
	{
		return data.size();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
