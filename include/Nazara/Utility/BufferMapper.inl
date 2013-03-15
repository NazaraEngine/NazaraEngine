// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

template<typename T>
NzBufferMapper<T>::NzBufferMapper(T* buffer, nzBufferAccess access, unsigned int offset, unsigned int length) :
m_buffer(buffer)
{
	if (m_buffer)
	{
		m_ptr = buffer->Map(access, offset, length);
		if (!m_ptr)
			NazaraError("Failed to map buffer"); ///TODO: Unexpected
	}
	else
		m_ptr = nullptr;
}

template<typename T>
NzBufferMapper<T>::NzBufferMapper(T& buffer, nzBufferAccess access, unsigned int offset, unsigned int length) :
NzBufferMapper(&buffer, access, offset, length)
{
}

template<typename T>
NzBufferMapper<T>::NzBufferMapper(const T* buffer, nzBufferAccess access, unsigned int offset, unsigned int length) :
m_buffer(buffer)
{
	if (m_buffer)
	{
		m_ptr = buffer->Map(access, offset, length);
		if (!m_ptr)
			NazaraError("Failed to map buffer"); ///TODO: Unexpected
	}
	else
		m_ptr = nullptr;
}

template<typename T>
NzBufferMapper<T>::NzBufferMapper(const T& buffer, nzBufferAccess access, unsigned int offset, unsigned int length) :
NzBufferMapper(&buffer, access, offset, length)
{
}

template<typename T>
NzBufferMapper<T>::~NzBufferMapper()
{
	if (m_buffer)
		m_buffer->Unmap();
}

template<typename T>
const T* NzBufferMapper<T>::GetBuffer() const
{
	return m_buffer;
}

template<typename T>
void* NzBufferMapper<T>::GetPointer() const
{
	return m_ptr;
}

template<typename T>
void NzBufferMapper<T>::Unmap()
{
	if (m_buffer)
	{
		m_buffer->Unmap();
		m_buffer = nullptr;
	}
}

#include <Nazara/Core/DebugOff.hpp>
