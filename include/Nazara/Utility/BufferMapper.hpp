// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_BUFFERMAPPER_HPP
#define NAZARA_UTILITY_BUFFERMAPPER_HPP

#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	template<class T>
	class BufferMapper
	{
		public:
			BufferMapper();
			BufferMapper(T& buffer, UInt64 offset, UInt64 length);
			~BufferMapper();

			bool Map(T& buffer, UInt64 offset, UInt64 length);

			const T* GetBuffer() const;
			void* GetPointer() const;

			void Unmap();

		private:
			T* m_buffer;
			void* m_ptr;
	};
}

#include <Nazara/Utility/BufferMapper.inl>

#endif // NAZARA_UTILITY_BUFFERMAPPER_HPP
