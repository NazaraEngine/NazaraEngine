// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STRINGSTREAM_HPP
#define NAZARA_STRINGSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API StringStream
	{
		public:
			StringStream() = default;
			StringStream(String str);
			StringStream(const StringStream&) = default;
			StringStream(StringStream&&) noexcept = default;

			void Clear();

			std::size_t GetBufferSize() const;

			String ToString() const;

			StringStream& operator=(const StringStream&) = default;
			StringStream& operator=(StringStream&&) noexcept = default;

			StringStream& operator<<(bool boolean);
			StringStream& operator<<(short number);
			StringStream& operator<<(unsigned short number);
			StringStream& operator<<(int number);
			StringStream& operator<<(unsigned int number);
			StringStream& operator<<(long number);
			StringStream& operator<<(unsigned long number);
			StringStream& operator<<(long long number);
			StringStream& operator<<(unsigned long long number);
			StringStream& operator<<(float number);
			StringStream& operator<<(double number);
			StringStream& operator<<(long double number);
			StringStream& operator<<(char character);
			StringStream& operator<<(unsigned char character);
			StringStream& operator<<(const char* string);
			StringStream& operator<<(const std::string& string);
			StringStream& operator<<(const String& string);
			StringStream& operator<<(const void* ptr);

			operator String() const;

		private:
			String m_result;
	};
}

#endif // NAZARA_STRINGSTREAM_HPP
