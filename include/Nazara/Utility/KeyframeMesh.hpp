// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_KEYFRAMEMESH_HPP
#define NAZARA_KEYFRAMEMESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/SubMesh.hpp>

struct NzKeyframeMeshImpl;

class NAZARA_API NzKeyframeMesh final : public NzSubMesh
{
	friend NzMesh;

	public:
		NzKeyframeMesh(const NzMesh* parent);
		virtual ~NzKeyframeMesh();

		bool Create(NzVertexBuffer* vertexBuffer, unsigned int frameCount, bool lock = true);
		void Destroy();

		void Finish();

		const NzAxisAlignedBox& GetAABB() const override;
		nzAnimationType GetAnimationType() const override;
		unsigned int GetFrameCount() const;
		const NzIndexBuffer* GetIndexBuffer() const override;
		bool GetVertex(NzMeshVertex* dest, unsigned int frameIndex, unsigned int vertexIndex, bool queryUV = true) const;
		const NzVertexBuffer* GetVertexBuffer() const override;

		void Interpolate(unsigned int frameA, unsigned int frameB, float interpolation);

		bool IsAnimated() const override;
		bool IsValid();

		bool Lock(nzBufferAccess access) const;

		void SetAABB(unsigned int frameIndex, const NzAxisAlignedBox& aabb);
		void SetIndexBuffer(const NzIndexBuffer* indexBuffer);
		bool SetVertex(const NzMeshVertex& source, unsigned int frameIndex, unsigned int vertexIndex, bool setUV = true);

		void Unlock() const;

	private:
		void InterpolateImpl(unsigned int frameA, unsigned int frameB, float interpolation);

		NzKeyframeMeshImpl* m_impl = nullptr;
};

#endif // NAZARA_KEYFRAMEMESH_HPP
