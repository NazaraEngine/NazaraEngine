// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvSectionBase.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	std::size_t SpirvSectionBase::AppendRaw(const Raw& raw)
	{
		std::size_t offset = GetOutputOffset();

		const UInt8* ptr = static_cast<const UInt8*>(raw.ptr);

		std::size_t size4 = CountWord(raw);
		for (std::size_t i = 0; i < size4; ++i)
		{
			UInt32 codepoint = 0;
			for (std::size_t j = 0; j < 4; ++j)
			{
#ifdef NAZARA_BIG_ENDIAN
				std::size_t pos = i * 4 + (3 - j);
#else
				std::size_t pos = i * 4 + j;
#endif

				if (pos < raw.size)
					codepoint |= UInt32(ptr[pos]) << (j * 8);
			}

			AppendRaw(codepoint);
		}

		return offset;
	}
}
