// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STATICMESH_HPP
#define NAZARA_STATICMESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/SubMesh.hpp>

class NAZARA_API NzStaticMesh final : public NzSubMesh
{
	public:
		NzStaticMesh(const NzMesh* parent);
		NzStaticMesh(const NzMesh* parent, const NzVertexDeclaration* vertexDeclaration, NzVertexBuffer* vertexBuffer, NzIndexBuffer* indexBuffer = nullptr);
		virtual ~NzStaticMesh();

		bool Create(const NzVertexDeclaration* vertexDeclaration, NzVertexBuffer* vertexBuffer, NzIndexBuffer* indexBuffer = nullptr);
		void Destroy();

		bool GenerateAABB();

		const NzAxisAlignedBox& GetAABB() const;
		nzAnimationType GetAnimationType() const;
		unsigned int GetFrameCount() const;
		const NzIndexBuffer* GetIndexBuffer() const;
		nzPrimitiveType GetPrimitiveType() const;
		const NzVertexBuffer* GetVertexBuffer() const;
		const NzVertexDeclaration* GetVertexDeclaration() const;

		bool IsAnimated() const;
		bool IsValid() const;

		void SetAABB(const NzAxisAlignedBox& aabb);
		void SetPrimitiveType(nzPrimitiveType primitiveType);

	private:
		void AnimateImpl(unsigned int frameA, unsigned int frameB, float interpolation);

		nzPrimitiveType m_primitiveType = nzPrimitiveType_TriangleList;
		NzAxisAlignedBox m_aabb;
		NzIndexBuffer* m_indexBuffer = nullptr;
		NzVertexBuffer* m_vertexBuffer = nullptr;
		const NzVertexDeclaration* m_vertexDeclaration = nullptr;
};

#endif // NAZARA_STATICMESH_HPP
