// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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
	BufferMapper<T>::BufferMapper(T& buffer, UInt64 offset, UInt64 length) :
	m_buffer(nullptr)
	{
		if (!Map(buffer, offset, length))
			NazaraError("failed to map buffer"); ///TODO: Unexpected
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
	bool BufferMapper<T>::Map(T& buffer, UInt64 offset, UInt64 length)
	{
		Unmap();

		m_buffer = &buffer;
		m_ptr = buffer.Map(offset, length);
		if (!m_ptr)
		{
			NazaraError("failed to map buffer"); ///TODO: Unexpected
			return false;
		}

		return true;
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
