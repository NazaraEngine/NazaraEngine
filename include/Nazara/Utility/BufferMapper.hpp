// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BUFFERMAPPER_HPP
#define NAZARA_BUFFERMAPPER_HPP

#include <Nazara/Utility/Enums.hpp>

template<class T>
class NzBufferMapper
{
	public:
		NzBufferMapper();
		NzBufferMapper(T* buffer, nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);
		NzBufferMapper(T& buffer, nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);
		NzBufferMapper(const T* buffer, nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);
		NzBufferMapper(const T& buffer, nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);
		~NzBufferMapper();

		bool Map(T* buffer, nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);
		bool Map(T& buffer, nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);
		bool Map(const T* buffer, nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);
		bool Map(const T& buffer, nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);

		const T* GetBuffer() const;
		void* GetPointer() const;

		void Unmap();

	private:
		const T* m_buffer;
		void* m_ptr;
};

#include <Nazara/Utility/BufferMapper.inl>

#endif // NAZARA_BUFFERMAPPER_HPP
