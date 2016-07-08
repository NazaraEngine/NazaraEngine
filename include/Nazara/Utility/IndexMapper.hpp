// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INDEXMAPPER_HPP
#define NAZARA_INDEXMAPPER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>

namespace Nz
{
	class IndexBuffer;
	class IndexIterator;
	class SubMesh;

	using IndexMapperGetter = UInt32 (*)(const void* buffer, unsigned int i);
	using IndexMapperSetter = void (*)(void* buffer, unsigned int i, UInt32 value);

	class NAZARA_UTILITY_API IndexMapper
	{
		public:
			IndexMapper(IndexBuffer* indexBuffer, BufferAccess access = BufferAccess_ReadWrite, std::size_t indexCount = 0);
			IndexMapper(SubMesh* subMesh, BufferAccess access = BufferAccess_ReadWrite);
			IndexMapper(const IndexBuffer* indexBuffer, BufferAccess access = BufferAccess_ReadOnly, std::size_t indexCount = 0);
			IndexMapper(const SubMesh* subMesh, BufferAccess access = BufferAccess_ReadOnly);
			~IndexMapper() = default;

			UInt32 Get(unsigned int i) const;
			const IndexBuffer* GetBuffer() const;
			unsigned int GetIndexCount() const;

			void Set(unsigned int i, UInt32 value);

			void Unmap();

			// Méthodes STD
			IndexIterator begin();
			//IndexConstIterator begin() const;
			IndexIterator end();
			//IndexIterator end() const;
			// Méthodes STD

		private:
			BufferMapper<IndexBuffer> m_mapper;
			IndexMapperGetter m_getter;
			IndexMapperSetter m_setter;
			unsigned int m_indexCount;
};
}

#endif // NAZARA_INDEXMAPPER_HPP
