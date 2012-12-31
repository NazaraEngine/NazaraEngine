// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SUBMESH_HPP
#define NAZARA_SUBMESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Utility/AxisAlignedBox.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>

class NzMesh;

class NAZARA_API NzSubMesh : public NzResource
{
	friend NzMesh;

	public:
		NzSubMesh(const NzMesh* parent);
		virtual ~NzSubMesh();

		virtual void Finish() = 0; ///DOC: Mets le mesh dans sa position d'origine et calcule son AABB

		virtual const NzAxisAlignedBox& GetAABB() const = 0;
		virtual nzAnimationType GetAnimationType() const = 0;
		virtual const NzIndexBuffer* GetIndexBuffer() const = 0;
		unsigned int GetMaterialIndex() const;
		const NzMesh* GetParent() const;
		nzPrimitiveType GetPrimitiveType() const;
		virtual NzVertexBuffer* GetVertexBuffer() = 0;
		virtual const NzVertexBuffer* GetVertexBuffer() const = 0;
		virtual unsigned int GetVertexCount() const;

		virtual bool IsAnimated() const = 0;

		void SetMaterialIndex(unsigned int matIndex);
		void SetPrimitiveType(nzPrimitiveType primitiveType);

	protected:
		nzPrimitiveType m_primitiveType = nzPrimitiveType_TriangleList;
		const NzMesh* m_parent;
		unsigned int m_matIndex;
};

#endif // NAZARA_SUBMESH_HPP
