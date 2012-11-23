// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKELETALMESH_HPP
#define NAZARA_SKELETALMESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/SubMesh.hpp>

class NzSkeleton;

struct NzVertexWeight
{
	std::vector<unsigned int> weights;
};

struct NzWeight
{
	float weight;
	unsigned int jointIndex;
};

struct NzSkeletalMeshImpl;

class NAZARA_API NzSkeletalMesh final : public NzSubMesh
{
	public:
		NzSkeletalMesh(const NzMesh* parent);
		virtual ~NzSkeletalMesh();

		bool Create(NzVertexBuffer* vertexBuffer, unsigned int weightCount);
		void Destroy();

		const NzAxisAlignedBox& GetAABB() const;
		nzAnimationType GetAnimationType() const final;
		void* GetBindPoseBuffer();
		const void* GetBindPoseBuffer() const;
		const NzIndexBuffer* GetIndexBuffer() const override;
		const NzVertexBuffer* GetVertexBuffer() const override;
		NzVertexWeight* GetVertexWeight(unsigned int vertexIndex = 0);
		const NzVertexWeight* GetVertexWeight(unsigned int vertexIndex = 0) const;
		NzWeight* GetWeight(unsigned int weightIndex = 0);
		const NzWeight* GetWeight(unsigned int weightIndex = 0) const;
		unsigned int GetWeightCount() const;

		bool IsAnimated() const final;
		bool IsValid() const;

		void Skin() const;
		void Skin(const NzSkeleton* skeleton) const;

		void SetIndexBuffer(const NzIndexBuffer* indexBuffer);

	private:
		NzSkeletalMeshImpl* m_impl = nullptr;
};

#endif // NAZARA_SKELETALMESH_HPP
