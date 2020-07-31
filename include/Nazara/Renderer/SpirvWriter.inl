// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/SpirvWriter.hpp>
#include <cassert>
#include <type_traits>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline void SpirvWriter::Append(const char* str)
	{
		return Append(std::string_view(str));
	}

	template<typename T>
	void SpirvWriter::Append(T value)
	{
		assert(m_currentState);
		m_currentState->output.push_back(static_cast<UInt32>(value));
	}

	template<typename ...Args>
	inline void SpirvWriter::Append(Opcode opcode, const Args&... args)
	{
		unsigned int wordCount = 1 + (CountWord(args) + ... + 0);
		Append(opcode, wordCount);
		if constexpr (sizeof...(args) > 0)
			(Append(args), ...);
	}

	template<typename T>
	inline unsigned int SpirvWriter::CountWord(const T& value)
	{
		return 1;
	}

	template<typename T1, typename T2, typename ...Args>
	unsigned int SpirvWriter::CountWord(const T1& value, const T2& value2, const Args&... rest)
	{
		return CountWord(value) + CountWord(value2) + (CountWord(rest) + ...);
	}

	inline unsigned int SpirvWriter::CountWord(const char* str)
	{
		return CountWord(std::string_view(str));
	}

	inline unsigned int SpirvWriter::CountWord(const std::string_view& str)
	{
		return (static_cast<unsigned int>(str.size() + 1) + sizeof(UInt32) - 1) / sizeof(UInt32); //< + 1 for null character
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
