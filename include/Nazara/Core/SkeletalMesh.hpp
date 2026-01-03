// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_SKELETALMESH_HPP
#define NAZARA_CORE_SKELETALMESH_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/IndexBuffer.hpp>
#include <Nazara/Core/SubMesh.hpp>
#include <Nazara/Core/VertexBuffer.hpp>

namespace Nz
{
	class NAZARA_CORE_API SkeletalMesh final : public SubMesh
	{
		public:
			SkeletalMesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer);
			~SkeletalMesh() = default;

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

#include <Nazara/Core/SkeletalMesh.inl>

#endif // NAZARA_CORE_SKELETALMESH_HPP
