// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKELETALMESH_HPP
#define NAZARA_SKELETALMESH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>

namespace Nz
{
	class SkeletalMesh;

	using SkeletalMeshConstRef = ObjectRef<const SkeletalMesh>;
	using SkeletalMeshRef = ObjectRef<SkeletalMesh>;

	class NAZARA_UTILITY_API SkeletalMesh final : public SubMesh
	{
		public:
			SkeletalMesh(VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer);

			NAZARA_DEPRECATED("SkeletalMesh constructor taking a mesh is deprecated, submeshes no longer require to be part of a single mesh")
			SkeletalMesh(const Mesh* parent);

			~SkeletalMesh();

			NAZARA_DEPRECATED("SkeletalMesh create/destroy functions are deprecated, please use constructor")
			bool Create(VertexBuffer* vertexBuffer);
			void Destroy();

			const Boxf& GetAABB() const override;
			AnimationType GetAnimationType() const final override;
			const IndexBuffer* GetIndexBuffer() const override;
			VertexBuffer* GetVertexBuffer();
			const VertexBuffer* GetVertexBuffer() const;
			unsigned int GetVertexCount() const override;

			bool IsAnimated() const final override;
			bool IsValid() const;

			void SetAABB(const Boxf& aabb);
			void SetIndexBuffer(const IndexBuffer* indexBuffer);

			template<typename... Args> static SkeletalMeshRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnSkeletalMeshDestroy, const SkeletalMesh* /*skeletalMesh*/);
			NazaraSignal(OnSkeletalMeshRelease, const SkeletalMesh* /*skeletalMesh*/);

		private:
			Boxf m_aabb;
			IndexBufferConstRef m_indexBuffer;
			VertexBufferRef m_vertexBuffer;
	};
}

#include <Nazara/Utility/SkeletalMesh.inl>

#endif // NAZARA_SKELETALMESH_HPP
