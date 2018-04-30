// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STATICMESH_HPP
#define NAZARA_STATICMESH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/SubMesh.hpp>

namespace Nz
{
	class StaticMesh;

	using StaticMeshConstRef = ObjectRef<const StaticMesh>;
	using StaticMeshRef = ObjectRef<StaticMesh>;

	class NAZARA_UTILITY_API StaticMesh final : public SubMesh
	{
		public:
			StaticMesh(const Mesh* parent);
			~StaticMesh();

			void Center();

			bool Create(VertexBuffer* vertexBuffer);
			void Destroy();

			bool GenerateAABB();

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

			template<typename... Args> static StaticMeshRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnStaticMeshDestroy, const StaticMesh* /*staticMesh*/);
			NazaraSignal(OnStaticMeshRelease, const StaticMesh* /*staticMesh*/);

		private:
			Boxf m_aabb;
			IndexBufferConstRef m_indexBuffer = nullptr;
			VertexBufferRef m_vertexBuffer = nullptr;
	};
}

#include <Nazara/Utility/StaticMesh.inl>

#endif // NAZARA_STATICMESH_HPP
