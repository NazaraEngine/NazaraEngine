// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_STATICMESH_HPP
#define NAZARA_CORE_STATICMESH_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/SubMesh.hpp>

namespace Nz
{
	class NAZARA_CORE_API StaticMesh final : public SubMesh
	{
		public:
			StaticMesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer);
			~StaticMesh() = default;

			void Center();

			bool GenerateAABB();

			const Boxf& GetAABB() const override;
			AnimationType GetAnimationType() const final;
			const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override;
			const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const;
			UInt32 GetVertexCount() const override;

			bool IsAnimated() const final;
			bool IsValid() const;

			void SetAABB(const Boxf& aabb);
			void SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer);

		private:
			Boxf m_aabb;
			std::shared_ptr<IndexBuffer> m_indexBuffer;
			std::shared_ptr<VertexBuffer> m_vertexBuffer;
	};
}

#include <Nazara/Core/StaticMesh.inl>

#endif // NAZARA_CORE_STATICMESH_HPP
