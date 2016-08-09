// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXMAPPER_HPP
#define NAZARA_VERTEXMAPPER_HPP

#include <Nazara/Prerequesites.hpp>
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
			VertexMapper(SubMesh* subMesh, BufferAccess access = BufferAccess_ReadWrite);
			VertexMapper(VertexBuffer* vertexBuffer, BufferAccess access = BufferAccess_ReadWrite);
			VertexMapper(const SubMesh* subMesh, BufferAccess access = BufferAccess_ReadOnly);
			VertexMapper(const VertexBuffer* vertexBuffer, BufferAccess access = BufferAccess_ReadOnly);
			~VertexMapper();

			template<typename T> SparsePtr<T> GetComponentPtr(VertexComponent component);

			void Unmap();

		private:
			BufferMapper<VertexBuffer> m_mapper;
	};
}

#include <Nazara/Utility/VertexMapper.inl>

#endif // NAZARA_VERTEXMAPPER_HPP
