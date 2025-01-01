// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_VERTEXMAPPER_HPP
#define NAZARA_CORE_VERTEXMAPPER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/BufferMapper.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/VertexBuffer.hpp>
#include <NazaraUtils/SparsePtr.hpp>

namespace Nz
{
	class SubMesh;

	class NAZARA_CORE_API VertexMapper
	{
		public:
			VertexMapper(SubMesh& subMesh);
			VertexMapper(VertexBuffer& vertexBuffer);
			~VertexMapper();

			template<typename T> SparsePtr<T> GetComponentPtr(VertexComponent component, std::size_t componentIndex = 0);
			inline const VertexBuffer* GetVertexBuffer() const;
			inline UInt32 GetVertexCount() const;

			template<typename T> bool HasComponentOfType(VertexComponent component) const;

			void Unmap();

		private:
			BufferMapper<VertexBuffer> m_mapper;
	};
}

#include <Nazara/Core/VertexMapper.inl>

#endif // NAZARA_CORE_VERTEXMAPPER_HPP
