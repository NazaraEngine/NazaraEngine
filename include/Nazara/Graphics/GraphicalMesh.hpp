// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_GRAPHICALMESH_HPP
#define NAZARA_GRAPHICS_GRAPHICALMESH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utils/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <memory>

namespace Nz
{
	class NAZARA_GRAPHICS_API GraphicalMesh
	{
		public:
			struct SubMesh;

			GraphicalMesh() = default;
			GraphicalMesh(const GraphicalMesh&) = delete;
			GraphicalMesh(GraphicalMesh&&) noexcept = default;
			~GraphicalMesh() = default;

			inline std::size_t AddSubMesh(SubMesh subMesh);

			inline void Clear();

			inline const std::shared_ptr<RenderBuffer>& GetIndexBuffer(std::size_t subMesh) const;
			inline UInt32 GetIndexCount(std::size_t subMesh) const;
			inline IndexType GetIndexType(std::size_t subMesh) const;
			inline const std::shared_ptr<RenderBuffer>& GetVertexBuffer(std::size_t subMesh) const;
			inline const std::shared_ptr<const VertexDeclaration>& GetVertexDeclaration(std::size_t subMesh) const;
			inline std::size_t GetSubMeshCount() const;

			inline void UpdateSubMeshIndexCount(std::size_t subMeshIndex, UInt32 indexCount);

			GraphicalMesh& operator=(const GraphicalMesh&) = delete;
			GraphicalMesh& operator=(GraphicalMesh&&) = delete;

			struct SubMesh
			{
				std::shared_ptr<RenderBuffer> indexBuffer;
				std::shared_ptr<RenderBuffer> vertexBuffer;
				std::shared_ptr<const VertexDeclaration> vertexDeclaration;
				IndexType indexType;
				UInt32 indexCount;
			};

			static std::shared_ptr<GraphicalMesh> BuildFromMesh(const Mesh& mesh);

			NazaraSignal(OnInvalidated, GraphicalMesh* /*gfxMesh*/);

		private:
			std::vector<SubMesh> m_subMeshes;
	};
}

#include <Nazara/Graphics/GraphicalMesh.inl>

#endif // NAZARA_GRAPHICS_GRAPHICALMESH_HPP
