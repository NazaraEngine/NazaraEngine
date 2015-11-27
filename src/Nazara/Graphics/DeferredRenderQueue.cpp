// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Rendre les billboards via Deferred Shading si possible

namespace Nz
{
	DeferredRenderQueue::DeferredRenderQueue(ForwardRenderQueue* forwardQueue) :
	m_forwardQueue(forwardQueue)
	{
	}

	void DeferredRenderQueue::AddBillboard(int renderOrder, const Material* material, const Vector3f& position, const Vector2f& size, const Vector2f& sinCos, const Color& color)
	{
		m_forwardQueue->AddBillboard(renderOrder, material, position, size, sinCos, color);
	}

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
	}

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
	}

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, anglePtr, colorPtr);
	}

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
	}

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
	}

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
	}

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, anglePtr, colorPtr);
	}

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
	}

	void DeferredRenderQueue::AddDrawable(int renderOrder, const Drawable* drawable)
	{
		m_forwardQueue->AddDrawable(renderOrder, drawable);
	}

	void DeferredRenderQueue::AddMesh(int renderOrder, const Material* material, const MeshData& meshData, const Boxf& meshAABB, const Matrix4f& transformMatrix)
	{
		if (material->IsEnabled(RendererParameter_Blend))
			// Un matériau transparent ? J'aime pas, va voir dans la forward queue si j'y suis
			m_forwardQueue->AddMesh(renderOrder, material, meshData, meshAABB, transformMatrix);
		else
		{
			Layer& currentLayer = GetLayer(renderOrder);
			auto& opaqueModels = currentLayer.opaqueModels;

			auto it = opaqueModels.find(material);
			if (it == opaqueModels.end())
			{
				BatchedModelEntry entry;
				entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &DeferredRenderQueue::OnMaterialInvalidation);

				it = opaqueModels.insert(std::make_pair(material, std::move(entry))).first;
			}

			BatchedModelEntry& entry = it->second;
			entry.enabled = true;

			auto& meshMap = entry.meshMap;

			auto it2 = meshMap.find(meshData);
			if (it2 == meshMap.end())
			{
				MeshInstanceEntry instanceEntry;
				if (meshData.indexBuffer)
					instanceEntry.indexBufferReleaseSlot.Connect(meshData.indexBuffer->OnIndexBufferRelease, this, &DeferredRenderQueue::OnIndexBufferInvalidation);

				instanceEntry.vertexBufferReleaseSlot.Connect(meshData.vertexBuffer->OnVertexBufferRelease, this, &DeferredRenderQueue::OnVertexBufferInvalidation);

				it2 = meshMap.insert(std::make_pair(meshData, std::move(instanceEntry))).first;
			}

			// On ajoute la matrice à la liste des instances de cet objet
			std::vector<Matrix4f>& instances = it2->second.instances;
			instances.push_back(transformMatrix);

			// Avons-nous suffisamment d'instances pour que le coût d'utilisation de l'instancing soit payé ?
			if (instances.size() >= NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT)
				entry.instancingEnabled = true; // Apparemment oui, activons l'instancing avec ce matériau
		}
	}

	void DeferredRenderQueue::AddSprites(int renderOrder, const Material* material, const VertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const Texture* overlay)
	{
		m_forwardQueue->AddSprites(renderOrder, material, vertices, spriteCount, overlay);
	}

	void DeferredRenderQueue::Clear(bool fully)
	{
		AbstractRenderQueue::Clear(fully);

		if (fully)
			layers.clear();
		else
		{
			for (auto it = layers.begin(); it != layers.end(); ++it)
			{
				Layer& layer = it->second;
				if (layer.clearCount++ >= 100)
					it = layers.erase(it);
			}
		}

		m_forwardQueue->Clear(fully);
	}

	DeferredRenderQueue::Layer& DeferredRenderQueue::GetLayer(unsigned int i)
	{
		auto it = layers.find(i);
		if (it == layers.end())
			it = layers.insert(std::make_pair(i, Layer())).first;

		Layer& layer = it->second;
		layer.clearCount = 0;

		return layer;
	}

	void DeferredRenderQueue::OnIndexBufferInvalidation(const IndexBuffer* indexBuffer)
	{
		for (auto& pair : layers)
		{
			Layer& layer = pair.second;

			for (auto& modelPair : layer.opaqueModels)
			{
				MeshInstanceContainer& meshes = modelPair.second.meshMap;
				for (auto it = meshes.begin(); it != meshes.end();)
				{
					const MeshData& renderData = it->first;
					if (renderData.indexBuffer == indexBuffer)
						it = meshes.erase(it);
					else
						++it;
				}
			}
		}
	}

	void DeferredRenderQueue::OnMaterialInvalidation(const Material* material)
	{
		for (auto& pair : layers)
		{
			Layer& layer = pair.second;

			layer.opaqueModels.erase(material);
		}
	}

	void DeferredRenderQueue::OnVertexBufferInvalidation(const VertexBuffer* vertexBuffer)
	{
		for (auto& pair : layers)
		{
			Layer& layer = pair.second;

			for (auto& modelPair : layer.opaqueModels)
			{
				MeshInstanceContainer& meshes = modelPair.second.meshMap;
				for (auto it = meshes.begin(); it != meshes.end();)
				{
					const MeshData& renderData = it->first;
					if (renderData.vertexBuffer == vertexBuffer)
						it = meshes.erase(it);
					else
						++it;
				}
			}
		}
	}

	bool DeferredRenderQueue::BatchedModelMaterialComparator::operator()(const Material* mat1, const Material* mat2) const
	{
		const UberShader* uberShader1 = mat1->GetShader();
		const UberShader* uberShader2 = mat2->GetShader();
		if (uberShader1 != uberShader2)
			return uberShader1 < uberShader2;

		const Shader* shader1 = mat1->GetShaderInstance(ShaderFlags_Deferred)->GetShader();
		const Shader* shader2 = mat2->GetShaderInstance(ShaderFlags_Deferred)->GetShader();
		if (shader1 != shader2)
			return shader1 < shader2;

		const Texture* diffuseMap1 = mat1->GetDiffuseMap();
		const Texture* diffuseMap2 = mat2->GetDiffuseMap();
		if (diffuseMap1 != diffuseMap2)
			return diffuseMap1 < diffuseMap2;

		return mat1 < mat2;
	}

	bool DeferredRenderQueue::MeshDataComparator::operator()(const MeshData& data1, const MeshData& data2) const
	{
		const Buffer* buffer1;
		const Buffer* buffer2;

		buffer1 = (data1.indexBuffer) ? data1.indexBuffer->GetBuffer() : nullptr;
		buffer2 = (data2.indexBuffer) ? data2.indexBuffer->GetBuffer() : nullptr;
		if (buffer1 != buffer2)
			return buffer1 < buffer2;

		buffer1 = data1.vertexBuffer->GetBuffer();
		buffer2 = data2.vertexBuffer->GetBuffer();
		if (buffer1 != buffer2)
			return buffer1 < buffer2;

		return data1.primitiveMode < data2.primitiveMode;
	}
}
