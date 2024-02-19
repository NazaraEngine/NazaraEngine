// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_INDEXMAPPER_HPP
#define NAZARA_CORE_INDEXMAPPER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/BufferMapper.hpp>
#include <Nazara/Core/IndexBuffer.hpp>

namespace Nz
{
	class IndexIterator;
	class SubMesh;

	class NAZARA_CORE_API IndexMapper
	{
		public:
			IndexMapper(IndexBuffer& indexBuffer, UInt32 indexCount = 0);
			IndexMapper(SubMesh& subMesh);
			~IndexMapper() = default;

			UInt32 Get(std::size_t i) const;
			const IndexBuffer* GetBuffer() const;
			UInt32 GetIndexCount() const;

			void Set(std::size_t i, UInt32 value);

			void Unmap();

			// Méthodes STD
			IndexIterator begin();
			//IndexConstIterator begin() const;
			IndexIterator end();
			//IndexIterator end() const;
			// Méthodes STD

		private:
			using Getter = UInt32(*)(const void* buffer, std::size_t i);
			using Setter = void(*)(void* buffer, std::size_t i, UInt32 value);

			BufferMapper<IndexBuffer> m_mapper;
			Getter m_getter;
			Setter m_setter;
			UInt32 m_indexCount;
};
}

#endif // NAZARA_CORE_INDEXMAPPER_HPP
