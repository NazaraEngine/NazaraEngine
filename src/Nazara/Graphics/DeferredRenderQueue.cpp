// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Rendre les billboards via Deferred Shading si possible

namespace
{
	enum ObjectType
	{
		ObjectType_IndexBuffer,
		ObjectType_Material,
		ObjectType_VertexBuffer
	};
}

NzDeferredRenderQueue::NzDeferredRenderQueue(NzForwardRenderQueue* forwardQueue) :
m_forwardQueue(forwardQueue)
{
}

void NzDeferredRenderQueue::AddBillboard(const NzMaterial* material, const NzVector3f& position, const NzVector2f& size, const NzVector2f& sinCos, const NzColor& color)
{
	m_forwardQueue->AddBillboard(material, position, size, sinCos, color);
}

void NzDeferredRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const NzColor> colorPtr)
{
	m_forwardQueue->AddBillboards(material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
}

void NzDeferredRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr)
{
	m_forwardQueue->AddBillboards(material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
}

void NzDeferredRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr)
{
	m_forwardQueue->AddBillboards(material, count, positionPtr, sizePtr, anglePtr, colorPtr);
}

void NzDeferredRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr)
{
	m_forwardQueue->AddBillboards(material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
}

void NzDeferredRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const NzColor> colorPtr)
{
	m_forwardQueue->AddBillboards(material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
}

void NzDeferredRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr)
{
	m_forwardQueue->AddBillboards(material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
}

void NzDeferredRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr)
{
	m_forwardQueue->AddBillboards(material, count, positionPtr, sizePtr, anglePtr, colorPtr);
}

void NzDeferredRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr)
{
	m_forwardQueue->AddBillboards(material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
}

void NzDeferredRenderQueue::AddDrawable(const NzDrawable* drawable)
{
	m_forwardQueue->AddDrawable(drawable);
}

void NzDeferredRenderQueue::AddLight(const NzLight* light)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!light)
	{
		NazaraError("Invalid light");
		return;
	}
	#endif

	// On trie la lumière (elles sont traitées différement selon leur type)
	switch (light->GetLightType())
	{
		case nzLightType_Directional:
			directionalLights.push_back(light);
			break;

		case nzLightType_Point:
			pointLights.push_back(light);
			break;

		case nzLightType_Spot:
			spotLights.push_back(light);
			break;
	}

	// On envoie également la lumière au forward-shading
	///TODO: Possibilité pour une lumière de se réserver au Deferred Shading
	m_forwardQueue->AddLight(light);
}

void NzDeferredRenderQueue::AddMesh(const NzMaterial* material, const NzMeshData& meshData, const NzBoxf& meshAABB, const NzMatrix4f& transformMatrix)
{
	if (material->IsEnabled(nzRendererParameter_Blend))
		// Un matériau transparent ? J'aime pas, va voir dans la forward queue si j'y suis
		m_forwardQueue->AddMesh(material, meshData, meshAABB, transformMatrix);
	else
	{
		auto it = opaqueModels.find(material);
		if (it == opaqueModels.end())
		{
			BatchedModelEntry entry(this, ObjectType_Material);
			entry.materialListener = material;

			it = opaqueModels.insert(std::make_pair(material, std::move(entry))).first;
		}

		BatchedModelEntry& entry = it->second;
		entry.enabled = true;

		auto& meshMap = entry.meshMap;

		auto it2 = meshMap.find(meshData);
		if (it2 == meshMap.end())
		{
			MeshInstanceEntry instanceEntry(this, ObjectType_IndexBuffer, ObjectType_VertexBuffer);
			instanceEntry.indexBufferListener = meshData.indexBuffer;
			instanceEntry.vertexBufferListener = meshData.vertexBuffer;

			it2 = meshMap.insert(std::make_pair(meshData, std::move(instanceEntry))).first;
		}

		// On ajoute la matrice à la liste des instances de cet objet
		std::vector<NzMatrix4f>& instances = it2->second.instances;
		instances.push_back(transformMatrix);

		// Avons-nous suffisamment d'instances pour que le coût d'utilisation de l'instancing soit payé ?
		if (instances.size() >= NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT)
			entry.instancingEnabled = true; // Apparemment oui, activons l'instancing avec ce matériau
	}
}

void NzDeferredRenderQueue::AddSprites(const NzMaterial* material, const NzVertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const NzTexture* overlay)
{
	m_forwardQueue->AddSprites(material, vertices, spriteCount, overlay);
}

void NzDeferredRenderQueue::Clear(bool fully)
{
	directionalLights.clear();
	pointLights.clear();
	spotLights.clear();

	if (fully)
		opaqueModels.clear();

	m_forwardQueue->Clear(fully);
}

bool NzDeferredRenderQueue::OnObjectDestroy(const NzRefCounted* object, int index)
{
	switch (index)
	{
		case ObjectType_IndexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = modelPair.second.meshMap;
				for (auto it = meshes.begin(); it != meshes.end();)
				{
					const NzMeshData& renderData = it->first;
					if (renderData.indexBuffer == object)
						it = meshes.erase(it);
					else
						++it;
				}
			}
			break;
		}

		case ObjectType_Material:
		{
			const NzMaterial* material = static_cast<const NzMaterial*>(object);

			opaqueModels.erase(material);
			break;
		}

		case ObjectType_VertexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = modelPair.second.meshMap;
				for (auto it = meshes.begin(); it != meshes.end();)
				{
					const NzMeshData& renderData = it->first;
					if (renderData.vertexBuffer == object)
						it = meshes.erase(it);
					else
						++it;
				}
			}
			break;
		}
	}

	return false; // Nous ne voulons plus recevoir d'évènement de cette ressource
}

void NzDeferredRenderQueue::OnObjectReleased(const NzRefCounted* object, int index)
{
	// La ressource vient d'être libérée, nous ne pouvons donc plus utiliser la méthode traditionnelle de recherche
	// des pointeurs stockés (À cause de la fonction de triage utilisant des spécificités des ressources)

	switch (index)
	{
		case ObjectType_IndexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = modelPair.second.meshMap;
				for (auto it = meshes.begin(); it != meshes.end();)
				{
					const NzMeshData& renderData = it->first;
					if (renderData.indexBuffer == object)
						it = meshes.erase(it);
					else
						++it;
				}
			}
			break;
		}

		case ObjectType_Material:
		{
			for (auto it = opaqueModels.begin(); it != opaqueModels.end(); ++it)
			{
				if (it->first == object)
				{
					opaqueModels.erase(it);
					break;
				}
			}
			break;
		}

		case ObjectType_VertexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = modelPair.second.meshMap;
				for (auto it = meshes.begin(); it != meshes.end();)
				{
					const NzMeshData& renderData = it->first;
					if (renderData.vertexBuffer == object)
						it = meshes.erase(it);
					else
						++it;
				}
			}
			break;
		}
	}
}

bool NzDeferredRenderQueue::BatchedModelMaterialComparator::operator()(const NzMaterial* mat1, const NzMaterial* mat2)
{
	const NzUberShader* uberShader1 = mat1->GetShader();
	const NzUberShader* uberShader2 = mat2->GetShader();
	if (uberShader1 != uberShader2)
		return uberShader1 < uberShader2;

	const NzShader* shader1 = mat1->GetShaderInstance(nzShaderFlags_Deferred)->GetShader();
	const NzShader* shader2 = mat2->GetShaderInstance(nzShaderFlags_Deferred)->GetShader();
	if (shader1 != shader2)
		return shader1 < shader2;

	const NzTexture* diffuseMap1 = mat1->GetDiffuseMap();
	const NzTexture* diffuseMap2 = mat2->GetDiffuseMap();
	if (diffuseMap1 != diffuseMap2)
		return diffuseMap1 < diffuseMap2;

	return mat1 < mat2;
}

bool NzDeferredRenderQueue::MeshDataComparator::operator()(const NzMeshData& data1, const NzMeshData& data2)
{
	const NzBuffer* buffer1;
	const NzBuffer* buffer2;

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
