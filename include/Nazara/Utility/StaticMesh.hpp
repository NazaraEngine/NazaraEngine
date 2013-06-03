// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STATICMESH_HPP
#define NAZARA_STATICMESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Utility/SubMesh.hpp>

class NzStaticMesh;

using NzStaticMeshConstRef = NzResourceRef<const NzStaticMesh>;
using NzStaticMeshRef = NzResourceRef<NzStaticMesh>;

class NAZARA_API NzStaticMesh final : public NzSubMesh, NzResourceListener
{
	public:
		NzStaticMesh(const NzMesh* parent);
		virtual ~NzStaticMesh();

		bool Create(NzVertexBuffer* vertexBuffer);
		void Destroy();

		bool GenerateAABB();

		const NzBoxf& GetAABB() const override;
		nzAnimationType GetAnimationType() const final;
		const NzIndexBuffer* GetIndexBuffer() const override;
		NzVertexBuffer* GetVertexBuffer();
		const NzVertexBuffer* GetVertexBuffer() const;
		unsigned int GetVertexCount() const override;

		bool IsAnimated() const final;
		bool IsValid() const;

		void SetAABB(const NzBoxf& aabb);
		void SetIndexBuffer(const NzIndexBuffer* indexBuffer);

	private:
		void OnResourceReleased(const NzResource* resource, int index) override;

		NzBoxf m_aabb;
		const NzIndexBuffer* m_indexBuffer = nullptr;
		NzVertexBuffer* m_vertexBuffer = nullptr;
};

#endif // NAZARA_STATICMESH_HPP
