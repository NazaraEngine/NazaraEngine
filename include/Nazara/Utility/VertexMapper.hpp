// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_VERTEXMAPPER_HPP
#define NAZARA_UTILITY_VERTEXMAPPER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>

namespace Nz
{
	class SubMesh;

	class NAZARA_UTILITY_API VertexMapper
	{
		public:
			VertexMapper(SubMesh& subMesh, BufferAccess access = BufferAccess::ReadWrite);
			VertexMapper(VertexBuffer& vertexBuffer, BufferAccess access = BufferAccess::ReadWrite);
			VertexMapper(const SubMesh& subMesh, BufferAccess access = BufferAccess::ReadOnly);
			VertexMapper(const VertexBuffer& vertexBuffer, BufferAccess access = BufferAccess::ReadOnly);
			~VertexMapper();

			template<typename T> SparsePtr<T> GetComponentPtr(VertexComponent component, std::size_t componentIndex = 0);
			inline const VertexBuffer* GetVertexBuffer() const;
			inline std::size_t GetVertexCount() const;
			
			template<typename T> bool HasComponentOfType(VertexComponent component) const;

			void Unmap();

		private:
			BufferMapper<VertexBuffer> m_mapper;
	};
}

#include <Nazara/Utility/VertexMapper.inl>

#endif // NAZARA_UTILITY_VERTEXMAPPER_HPP
