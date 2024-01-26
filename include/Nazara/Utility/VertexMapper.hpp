// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_VERTEXMAPPER_HPP
#define NAZARA_UTILITY_VERTEXMAPPER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <NazaraUtils/SparsePtr.hpp>

namespace Nz
{
	class SubMesh;

	class NAZARA_UTILITY_API VertexMapper
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

#include <Nazara/Utility/VertexMapper.inl>

#endif // NAZARA_UTILITY_VERTEXMAPPER_HPP
