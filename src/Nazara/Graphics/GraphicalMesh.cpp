// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Utility/SoftwareBuffer.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	GraphicalMesh::GraphicalMesh(const Mesh& mesh)
	{
		assert(mesh.GetAnimationType() == AnimationType::Static);

		const std::shared_ptr<RenderDevice>& renderDevice = Graphics::Instance()->GetRenderDevice();

		m_subMeshes.reserve(mesh.GetSubMeshCount());
		for (std::size_t i = 0; i < mesh.GetSubMeshCount(); ++i)
		{
			const SubMesh& subMesh = *mesh.GetSubMesh(i);

			const StaticMesh& staticMesh = static_cast<const StaticMesh&>(subMesh);

			const std::shared_ptr<const IndexBuffer>& indexBuffer = staticMesh.GetIndexBuffer();
			const std::shared_ptr<VertexBuffer>& vertexBuffer = staticMesh.GetVertexBuffer();

			assert(indexBuffer->GetBuffer()->GetStorage() == DataStorage::Software);
			const SoftwareBuffer* indexBufferContent = static_cast<const SoftwareBuffer*>(indexBuffer->GetBuffer()->GetImpl());

			assert(vertexBuffer->GetBuffer()->GetStorage() == DataStorage::Software);
			const SoftwareBuffer* vertexBufferContent = static_cast<const SoftwareBuffer*>(vertexBuffer->GetBuffer()->GetImpl());

			auto& submeshData = m_subMeshes.emplace_back();
			submeshData.indexBuffer = renderDevice->InstantiateBuffer(BufferType::Index);
			if (!submeshData.indexBuffer->Initialize(indexBuffer->GetStride() * indexBuffer->GetIndexCount(), BufferUsage::DeviceLocal))
				throw std::runtime_error("failed to create index buffer");

			if (!submeshData.indexBuffer->Fill(indexBufferContent->GetData() + indexBuffer->GetStartOffset(), 0, indexBuffer->GetEndOffset() - indexBuffer->GetStartOffset()))
				throw std::runtime_error("failed to fill index buffer");

			submeshData.indexCount = indexBuffer->GetIndexCount();

			submeshData.vertexBuffer = renderDevice->InstantiateBuffer(BufferType::Vertex);
			if (!submeshData.vertexBuffer->Initialize(vertexBuffer->GetStride() * vertexBuffer->GetVertexCount(), BufferUsage::DeviceLocal))
				throw std::runtime_error("failed to create vertex buffer");

			if (!submeshData.vertexBuffer->Fill(vertexBufferContent->GetData() + vertexBuffer->GetStartOffset(), 0, vertexBuffer->GetEndOffset() - vertexBuffer->GetStartOffset()))
				throw std::runtime_error("failed to fill vertex buffer");

			submeshData.vertexDeclaration = vertexBuffer->GetVertexDeclaration();
		}
	}
}
