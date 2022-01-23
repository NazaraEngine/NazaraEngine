// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_GRAPHICALMESH_HPP
#define NAZARA_GRAPHICS_GRAPHICALMESH_HPP

#include <Nazara/Prerequisites.hpp>
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
			GraphicalMesh(const Mesh& mesh);
			GraphicalMesh(const GraphicalMesh&) = delete;
			GraphicalMesh(GraphicalMesh&&) noexcept = default;
			~GraphicalMesh() = default;

			inline const std::shared_ptr<RenderBuffer>& GetIndexBuffer(std::size_t subMesh) const;
			inline std::size_t GetIndexCount(std::size_t subMesh) const;
			inline const std::shared_ptr<RenderBuffer>& GetVertexBuffer(std::size_t subMesh) const;
			inline const std::shared_ptr<const VertexDeclaration>& GetVertexDeclaration(std::size_t subMesh) const;
			inline std::size_t GetSubMeshCount() const;

			GraphicalMesh& operator=(const GraphicalMesh&) = delete;
			GraphicalMesh& operator=(GraphicalMesh&&) noexcept = default;

		private:
			struct GraphicalSubMesh
			{
				std::shared_ptr<RenderBuffer> indexBuffer;
				std::shared_ptr<RenderBuffer> vertexBuffer;
				std::size_t indexCount;
				std::shared_ptr<const VertexDeclaration> vertexDeclaration;
			};

			std::vector<GraphicalSubMesh> m_subMeshes;
	};
}

#include <Nazara/Graphics/GraphicalMesh.inl>

#endif // NAZARA_GRAPHICS_GRAPHICALMESH_HPP
