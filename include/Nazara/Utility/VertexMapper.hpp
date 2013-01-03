// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXMAPPER_HPP
#define NAZARA_VERTEXMAPPER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Enums.hpp>

class NzSubMesh;

class NzVertexMapperImpl;

class NAZARA_API NzVertexMapper
{
	public:
		NzVertexMapper(NzSubMesh* subMesh);
		~NzVertexMapper();

		NzVector3f GetNormal(unsigned int i) const;
		NzVector3f GetPosition(unsigned int i) const;
		NzVector3f GetTangent(unsigned int i) const;
		NzVector2f GetTexCoords(unsigned int i) const;
		unsigned int GetTotalVertexCount();

		void SetNormal(unsigned int i, const NzVector3f& normal);
		void SetPosition(unsigned int i, const NzVector3f& position);
		void SetTangent(unsigned int i, const NzVector3f& tangent);
		void SetTexCoords(unsigned int i, const NzVector2f& texCoords);

		void Unmap();

	private:
		NzVertexMapperImpl* m_impl;
};

#endif // NAZARA_VERTEXMAPPER_HPP
