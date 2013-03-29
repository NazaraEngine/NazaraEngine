// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXMAPPER_HPP
#define NAZARA_VERTEXMAPPER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Mesh.hpp>

class NzSubMesh;

class NAZARA_API NzVertexMapper
{
	public:
		NzVertexMapper(NzSubMesh* subMesh);
		~NzVertexMapper();

		NzVector3f GetNormal(unsigned int i) const;
		NzVector3f GetPosition(unsigned int i) const;
		NzVector3f GetTangent(unsigned int i) const;
		NzVector2f GetTexCoord(unsigned int i) const;
		unsigned int GetVertexCount();

		void SetNormal(unsigned int i, const NzVector3f& normal);
		void SetPosition(unsigned int i, const NzVector3f& position);
		void SetTangent(unsigned int i, const NzVector3f& tangent);
		void SetTexCoord(unsigned int i, const NzVector2f& texCoord);

		void Unmap();

	private:
		NzBufferMapper<NzVertexBuffer> m_mapper;
		NzMeshVertex* m_vertices;
		unsigned int m_vertexCount;
};

#endif // NAZARA_VERTEXMAPPER_HPP
