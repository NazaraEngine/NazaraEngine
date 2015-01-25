// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKELETALMESH_HPP
#define NAZARA_SKELETALMESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Utility/SubMesh.hpp>

class NzSkeletalMesh;

using NzSkeletalMeshConstListener = NzObjectListenerWrapper<const NzSkeletalMesh>;
using NzSkeletalMeshConstRef = NzObjectRef<const NzSkeletalMesh>;
using NzSkeletalMeshListener = NzObjectListenerWrapper<NzSkeletalMesh>;
using NzSkeletalMeshRef = NzObjectRef<NzSkeletalMesh>;

class NAZARA_API NzSkeletalMesh final : public NzSubMesh
{
	public:
		NzSkeletalMesh(const NzMesh* parent);
		~NzSkeletalMesh();

		bool Create(NzVertexBuffer* vertexBuffer);
		void Destroy();

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

		template<typename... Args> static NzSkeletalMeshRef New(Args&&... args);

	private:
		NzBoxf m_aabb;
		NzIndexBufferConstRef m_indexBuffer = nullptr;
		NzVertexBufferRef m_vertexBuffer = nullptr;
};

#include <Nazara/Utility/SkeletalMesh.inl>

#endif // NAZARA_SKELETALMESH_HPP
