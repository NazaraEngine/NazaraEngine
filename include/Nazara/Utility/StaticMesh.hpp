// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STATICMESH_HPP
#define NAZARA_STATICMESH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/SubMesh.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API StaticMesh final : public SubMesh
	{
		public:
			StaticMesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<const IndexBuffer> indexBuffer);
			~StaticMesh() = default;

			void Center();

			bool GenerateAABB();

			const Boxf& GetAABB() const override;
			AnimationType GetAnimationType() const final;
			const std::shared_ptr<const IndexBuffer>& GetIndexBuffer() const override;
			const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const;
			std::size_t GetVertexCount() const override;

			bool IsAnimated() const final;
			bool IsValid() const;

			void SetAABB(const Boxf& aabb);
			void SetIndexBuffer(std::shared_ptr<const IndexBuffer> indexBuffer);

		private:
			Boxf m_aabb;
			std::shared_ptr<const IndexBuffer> m_indexBuffer;
			std::shared_ptr<VertexBuffer> m_vertexBuffer;
	};
}

#include <Nazara/Utility/StaticMesh.inl>

#endif // NAZARA_STATICMESH_HPP
