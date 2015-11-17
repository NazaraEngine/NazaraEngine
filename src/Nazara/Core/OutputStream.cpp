// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/OutputStream.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	OutputStream::~OutputStream() = default;

	bool OutputStream::Write(const ByteArray& byteArray)
	{
		ByteArray::size_type size = byteArray.GetSize();
		return Write(byteArray.GetConstBuffer(), size) == size;
	}

	bool OutputStream::Write(const String& string)
	{
		String temp(string);

		if (m_streamOptions & StreamOption_Text)
		{
			#if defined(NAZARA_PLATFORM_WINDOWS)
			temp.Replace("\n", "\r\n");
			#elif defined(NAZARA_PLATFORM_LINUX)
			// Nothing to do
			#elif defined(NAZARA_PLATFORM_MACOS)
			temp.Replace('\n', '\r');
			#endif
		}

		std::size_t size = temp.GetSize();
		return Write(temp.GetConstBuffer(), size) == size;
	}
}
