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
		NzStaticMesh(const NzMesh* parent, const NzVertexBuffer* vertexBuffer, const NzVertexDeclaration* vertexDeclaration, const NzIndexBuffer* indexBuffer = nullptr);
		virtual ~NzStaticMesh();

		bool Create(const NzVertexBuffer* vertexBuffer, const NzVertexDeclaration* vertexDeclaration, const NzIndexBuffer* indexBuffer = nullptr);
		void Destroy();

		nzAnimationType GetAnimationType() const;
		unsigned int GetFrameCount() const;
		const NzIndexBuffer* GetIndexBuffer() const;
		nzPrimitiveType GetPrimitiveType() const;
		const NzVertexBuffer* GetVertexBuffer() const;
		const NzVertexDeclaration* GetVertexDeclaration() const;

		bool IsAnimated() const;
		bool IsValid() const;

		void SetPrimitiveType(nzPrimitiveType primitiveType);

	private:
		void AnimateImpl(unsigned int frameA, unsigned int frameB, float interpolation);

		nzPrimitiveType m_primitiveType = nzPrimitiveType_TriangleList;
		const NzIndexBuffer* m_indexBuffer = nullptr;
		const NzVertexBuffer* m_vertexBuffer = nullptr;
		const NzVertexDeclaration* m_vertexDeclaration = nullptr;
};

#endif // NAZARA_STATICMESH_HPP
