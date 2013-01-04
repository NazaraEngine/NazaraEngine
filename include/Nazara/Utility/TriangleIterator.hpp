// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TRIANGLEITERATOR_HPP
#define NAZARA_TRIANGLEITERATOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/VertexMapper.hpp>

class NzSubMesh;

class NAZARA_API NzTriangleIterator
{
	public:
		NzTriangleIterator(NzSubMesh* subMesh, nzBufferAccess access = nzBufferAccess_ReadWrite);
		~NzTriangleIterator() = default;

		bool Advance();

		NzVector3f GetNormal(unsigned int i) const;
		NzVector3f GetPosition(unsigned int i) const;
		NzVector3f GetTangent(unsigned int i) const;
		NzVector2f GetTexCoords(unsigned int i) const;

		void SetNormal(unsigned int i, const NzVector3f& normal);
		void SetPosition(unsigned int i, const NzVector3f& position);
		void SetTangent(unsigned int i, const NzVector3f& tangent);
		void SetTexCoords(unsigned int i, const NzVector2f& texCoords);

	private:
		nzPrimitiveType m_primitiveType;
		nzUInt32 m_triangleIndices[3];
		NzIndexMapper m_indexMapper;
		NzVertexMapper m_vertexMapper;
		unsigned int m_currentIndex;
		unsigned int m_indexCount;
};

#endif // NAZARA_TRIANGLEITERATOR_HPP
