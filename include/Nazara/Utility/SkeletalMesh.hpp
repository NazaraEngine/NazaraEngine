// Copyright (C) 2013 Jérôme Leclercq
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
	std::vector<unsigned int> weights; ///FIXME: Niveau fragmentation mémoire ça doit pas être génial
};

struct NzWeight
{
	float weight;
	unsigned int jointIndex;
};

class NzSkeletalMesh;

using NzSkeletalMeshConstRef = NzResourceRef<const NzSkeletalMesh>;
using NzSkeletalMeshRef = NzResourceRef<NzSkeletalMesh>;

struct NzSkeletalMeshImpl;

class NAZARA_API NzSkeletalMesh final : public NzSubMesh
{
	public:
		NzSkeletalMesh(const NzMesh* parent);
		virtual ~NzSkeletalMesh();

		bool Create(unsigned int vertexCount, unsigned int weightCount);
		void Destroy();

		const NzBoxf& GetAABB() const;
		nzAnimationType GetAnimationType() const final;
		NzMeshVertex* GetBindPoseBuffer();
		const NzMeshVertex* GetBindPoseBuffer() const;
		const NzIndexBuffer* GetIndexBuffer() const override;
		unsigned int GetVertexCount() const override;
		NzVertexWeight* GetVertexWeight(unsigned int vertexIndex = 0);
		const NzVertexWeight* GetVertexWeight(unsigned int vertexIndex = 0) const;
		NzWeight* GetWeight(unsigned int weightIndex = 0);
		const NzWeight* GetWeight(unsigned int weightIndex = 0) const;
		unsigned int GetWeightCount() const;

		bool IsAnimated() const final;
		bool IsValid() const;

		void Skin(NzMeshVertex* outputBuffer) const;
		void Skin(NzMeshVertex* outputBuffer, const NzSkeleton* skeleton) const;

		void SetIndexBuffer(const NzIndexBuffer* indexBuffer);

	private:
		NzSkeletalMeshImpl* m_impl = nullptr;
};

#endif // NAZARA_SKELETALMESH_HPP
