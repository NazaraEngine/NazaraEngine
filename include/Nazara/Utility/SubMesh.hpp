// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SUBMESH_HPP
#define NAZARA_SUBMESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/Resource.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>

class NzMesh;

class NAZARA_API NzSubMesh : public NzResource
{
	friend class NzMesh;

	public:
		NzSubMesh(const NzMesh* parent);
		virtual ~NzSubMesh();

		void Animate(unsigned int frameA, unsigned int frameB, float interpolation);

		virtual const NzIndexBuffer* GetIndexBuffer() const = 0;
		const NzMesh* GetParent() const;
		virtual nzPrimitiveType GetPrimitiveType() const = 0;
		virtual const NzVertexBuffer* GetVertexBuffer() const = 0;
		virtual const NzVertexDeclaration* GetVertexDeclaration() const = 0;
		unsigned int GetVertexCount() const;

	protected:
		virtual void AnimateImpl(unsigned int frameA, unsigned int frameB, float interpolation) = 0;

		const NzMesh* m_parent;
};

#endif // NAZARA_SUBMESH_HPP
