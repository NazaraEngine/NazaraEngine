// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BUFFERMAPPER_HPP
#define NAZARA_BUFFERMAPPER_HPP

#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	template<class T>
	class BufferMapper
	{
		public:
			BufferMapper();
			BufferMapper(T* buffer, BufferAccess access, unsigned int offset = 0, unsigned int length = 0);
			BufferMapper(T& buffer, BufferAccess access, unsigned int offset = 0, unsigned int length = 0);
			BufferMapper(const T* buffer, BufferAccess access, unsigned int offset = 0, unsigned int length = 0);
			BufferMapper(const T& buffer, BufferAccess access, unsigned int offset = 0, unsigned int length = 0);
			~BufferMapper();

			bool Map(T* buffer, BufferAccess access, unsigned int offset = 0, unsigned int length = 0);
			bool Map(T& buffer, BufferAccess access, unsigned int offset = 0, unsigned int length = 0);
			bool Map(const T* buffer, BufferAccess access, unsigned int offset = 0, unsigned int length = 0);
			bool Map(const T& buffer, BufferAccess access, unsigned int offset = 0, unsigned int length = 0);

			const T* GetBuffer() const;
			void* GetPointer() const;

			void Unmap();

		private:
			const T* m_buffer;
			void* m_ptr;
	};
}

#include <Nazara/Utility/BufferMapper.inl>

#endif // NAZARA_BUFFERMAPPER_HPP
