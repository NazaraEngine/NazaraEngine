// Copyright (C) 2014 Jérôme Leclercq
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

class NzSubMesh;

class NAZARA_API NzVertexMapper
{
	public:
		NzVertexMapper(NzVertexBuffer* vertexBuffer, unsigned int vertexCount);
		NzVertexMapper(NzSubMesh* subMesh);
		~NzVertexMapper();

		template<typename T> NzSparsePtr<T> GetComponentPtr(nzVertexComponent component);
		unsigned int GetVertexCount() const;

		void Unmap();

	private:
		NzBufferMapper<NzVertexBuffer> m_mapper;
		unsigned int m_vertexCount;
};

#include <Nazara/Utility/VertexMapper.inl>

#endif // NAZARA_VERTEXMAPPER_HPP
