// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <Nazara/Core/SoftwareBuffer.hpp>
#include <Nazara/Core/StaticMesh.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <cassert>

namespace Nz
{
	std::shared_ptr<GraphicalMesh> GraphicalMesh::BuildFromMesh(const Mesh& mesh)
	{
		const std::shared_ptr<RenderDevice>& renderDevice = Graphics::Instance()->GetRenderDevice();

		std::shared_ptr<GraphicalMesh> gfxMesh = std::make_shared<GraphicalMesh>();

		for (std::size_t i = 0; i < mesh.GetSubMeshCount(); ++i)
		{
			const Nz::SubMesh& subMesh = *mesh.GetSubMesh(i);

			const StaticMesh& staticMesh = static_cast<const StaticMesh&>(subMesh);

			const std::shared_ptr<VertexBuffer>& vertexBuffer = staticMesh.GetVertexBuffer();
			assert(vertexBuffer->GetBuffer()->GetStorage() == DataStorage::Software);
			const SoftwareBuffer* vertexBufferContent = static_cast<const SoftwareBuffer*>(vertexBuffer->GetBuffer().get());

			GraphicalMesh::SubMesh submeshData;

			const std::shared_ptr<const IndexBuffer>& indexBuffer = staticMesh.GetIndexBuffer();
			if (indexBuffer)
			{
				assert(indexBuffer->GetBuffer()->GetStorage() == DataStorage::Software);
				const SoftwareBuffer* indexBufferContent = static_cast<const SoftwareBuffer*>(indexBuffer->GetBuffer().get());

				submeshData.indexBuffer = renderDevice->InstantiateBuffer(BufferType::Index, indexBuffer->GetStride() * indexBuffer->GetIndexCount(), BufferUsage::DeviceLocal | BufferUsage::Write);
				if (!submeshData.indexBuffer->Fill(indexBufferContent->GetData() + indexBuffer->GetStartOffset(), 0, indexBuffer->GetEndOffset() - indexBuffer->GetStartOffset()))
					throw std::runtime_error("failed to fill index buffer");

				submeshData.indexCount = indexBuffer->GetIndexCount();
				submeshData.indexType = indexBuffer->GetIndexType();
			}
			else
				submeshData.indexCount = vertexBuffer->GetVertexCount();

			submeshData.vertexBuffer = renderDevice->InstantiateBuffer(BufferType::Vertex, vertexBuffer->GetStride() * vertexBuffer->GetVertexCount(), BufferUsage::DeviceLocal | BufferUsage::Write);
			if (!submeshData.vertexBuffer->Fill(vertexBufferContent->GetData() + vertexBuffer->GetStartOffset(), 0, vertexBuffer->GetEndOffset() - vertexBuffer->GetStartOffset()))
				throw std::runtime_error("failed to fill vertex buffer");

			submeshData.vertexDeclaration = vertexBuffer->GetVertexDeclaration();

			gfxMesh->AddSubMesh(std::move(submeshData));
		}

		gfxMesh->UpdateAABB(mesh.GetAABB());

		return gfxMesh;
	}
}
