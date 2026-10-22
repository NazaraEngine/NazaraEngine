// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_GRAPHICALMESH_HPP
#define NAZARA_GRAPHICS_GRAPHICALMESH_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Mesh.hpp>
#include <Nazara/Core/VertexDeclaration.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Renderer/GpuBuffer.hpp>
#include <NazaraUtils/Signal.hpp>
#include <memory>

namespace Nz
{
	class GpuAsyncCommands;

	class NAZARA_GRAPHICS_API GraphicalMesh
	{
		public:
			struct Submesh;

			GraphicalMesh() = default;
			GraphicalMesh(const GraphicalMesh&) = delete;
			GraphicalMesh(GraphicalMesh&&) noexcept = default;
			~GraphicalMesh() = default;

			inline std::size_t AddSubmesh(Submesh submesh);

			inline void Clear();

			inline const Boxf& GetAABB(std::size_t submeshIndex) const;
			inline const std::shared_ptr<GpuBuffer>& GetIndexBuffer(std::size_t submeshIndex) const;
			inline UInt32 GetIndexCount(std::size_t submeshIndex) const;
			inline IndexType GetIndexType(std::size_t submeshIndex) const;
			inline const std::shared_ptr<GpuBuffer>& GetVertexBuffer(std::size_t submeshIndex) const;
			inline const std::shared_ptr<const VertexDeclaration>& GetVertexDeclaration(std::size_t submeshIndex) const;
			inline std::size_t GetSubmeshCount() const;

			inline void UpdateSubmeshIndexCount(std::size_t submeshIndex, UInt32 indexCount);

			GraphicalMesh& operator=(const GraphicalMesh&) = delete;
			GraphicalMesh& operator=(GraphicalMesh&&) = delete;

			struct Submesh
			{
				std::shared_ptr<GpuBuffer> indexBuffer;
				std::shared_ptr<GpuBuffer> vertexBuffer;
				std::shared_ptr<const VertexDeclaration> vertexDeclaration;
				Boxf aabb;
				IndexType indexType;
				UInt32 indexCount;
			};

			static inline std::shared_ptr<GraphicalMesh> Build(const Primitive& primitive, const MeshParams& params = MeshParams());
			static inline std::shared_ptr<GraphicalMesh> Build(const PrimitiveList& primitiveList, const MeshParams& params = MeshParams());
			static std::shared_ptr<GraphicalMesh> BuildFromMesh(const Mesh& mesh);
			static std::shared_ptr<GraphicalMesh> BuildFromMesh(GpuAsyncCommands& asyncTransfer, const Mesh& mesh);

			NazaraSignal(OnInvalidated, GraphicalMesh* /*gfxMesh*/);

		private:
			std::vector<Submesh> m_submeshes;
	};
}

#include <Nazara/Graphics/GraphicalMesh.inl>

#endif // NAZARA_GRAPHICS_GRAPHICALMESH_HPP
