// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	template<typename T>
	BufferMapper<T>::BufferMapper() :
	m_buffer(nullptr),
	m_ptr(nullptr)
	{
	}

	template<typename T>
	BufferMapper<T>::BufferMapper(T* buffer, BufferAccess access, unsigned int offset, unsigned int length) :
	m_buffer(nullptr)
	{
		if (!Map(buffer, access, offset, length))
			NazaraError("Failed to map buffer"); ///TODO: Unexpected
	}

	template<typename T>
	BufferMapper<T>::BufferMapper(T& buffer, BufferAccess access, unsigned int offset, unsigned int length) :
	BufferMapper(&buffer, access, offset, length)
	{
	}

	template<typename T>
	BufferMapper<T>::BufferMapper(const T* buffer, BufferAccess access, unsigned int offset, unsigned int length) :
	m_buffer(nullptr)
	{
		if (!Map(buffer, access, offset, length))
			NazaraError("Failed to map buffer"); ///TODO: Unexpected
	}

	template<typename T>
	BufferMapper<T>::BufferMapper(const T& buffer, BufferAccess access, unsigned int offset, unsigned int length) :
	BufferMapper(&buffer, access, offset, length)
	{
	}

	template<typename T>
	BufferMapper<T>::~BufferMapper()
	{
		if (m_buffer)
			m_buffer->Unmap();
	}

	template<typename T>
	const T* BufferMapper<T>::GetBuffer() const
	{
		return m_buffer;
	}

	template<typename T>
	void* BufferMapper<T>::GetPointer() const
	{
		return m_ptr;
	}

	template<typename T>
	bool BufferMapper<T>::Map(T* buffer, BufferAccess access, unsigned int offset, unsigned int length)
	{
		Unmap();

		#if NAZARA_UTILITY_SAFE
		if (!buffer)
		{
			NazaraError("Buffer must be valid");
			m_ptr = nullptr;

			return false;
		}
		#endif

		m_buffer = buffer;
		m_ptr = buffer->Map(access, offset, length);
		if (!m_ptr)
			NazaraError("Failed to map buffer"); ///TODO: Unexpected

		return true;
	}

	template<typename T>
	bool BufferMapper<T>::Map(T& buffer, BufferAccess access, unsigned int offset, unsigned int length)
	{
		return Map(&buffer, access, offset, length);
	}

	template<typename T>
	bool BufferMapper<T>::Map(const T* buffer, BufferAccess access, unsigned int offset, unsigned int length)
	{
		Unmap();

		#if NAZARA_UTILITY_SAFE
		if (!buffer)
		{
			NazaraError("Buffer must be valid");
			m_ptr = nullptr;

			return false;
		}
		#endif

		m_buffer = buffer;
		m_ptr = buffer->Map(access, offset, length);
		if (!m_ptr)
			NazaraError("Failed to map buffer"); ///TODO: Unexpected

		return true;
	}

	template<typename T>
	bool BufferMapper<T>::Map(const T& buffer, BufferAccess access, unsigned int offset, unsigned int length)
	{
		return Map(&buffer, access, offset, length);
	}

	template<typename T>
	void BufferMapper<T>::Unmap()
	{
		if (m_buffer)
		{
			m_buffer->Unmap();
			m_buffer = nullptr;
		}
	}
}

#include <Nazara/Utility/DebugOff.hpp>
