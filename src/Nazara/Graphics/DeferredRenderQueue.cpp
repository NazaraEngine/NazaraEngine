// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredRenderQueue.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	enum ResourceType
	{
		ResourceType_IndexBuffer,
		ResourceType_Material,
		ResourceType_VertexBuffer
	};
}

NzDeferredRenderQueue::NzDeferredRenderQueue(NzForwardRenderQueue* forwardQueue) :
m_forwardQueue(forwardQueue)
{
}

NzDeferredRenderQueue::~NzDeferredRenderQueue()
{
	Clear(true);
}

void NzDeferredRenderQueue::AddBillboard(const NzMaterial* material, const NzVector3f& position, const NzVector2f& size, const NzVector2f& sinCos, const NzColor& color)
{
	///TODO: Rendre les billboards via Deferred Shading si possible
	m_forwardQueue->AddBillboard(material, position, size, sinCos, color);
}

void NzDeferredRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const NzColor> colorPtr)
{
	///TODO: Rendre les billboards via Deferred Shading si possible
	m_forwardQueue->AddBillboards(material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
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

	m_forwardQueue->AddLight(light);
}

void NzDeferredRenderQueue::AddMesh(const NzMaterial* material, const NzMeshData& meshData, const NzBoxf& meshAABB, const NzMatrix4f& transformMatrix)
{
	if (material->IsEnabled(nzRendererParameter_Blend))
		m_forwardQueue->AddMesh(material, meshData, meshAABB, transformMatrix);
	else
	{
		ModelBatches::iterator it = opaqueModels.find(material);
		if (it == opaqueModels.end())
		{
			it = opaqueModels.insert(std::make_pair(material, ModelBatches::mapped_type())).first;
			material->AddResourceListener(this, ResourceType_Material);
		}

		bool& used = std::get<0>(it->second);
		bool& enableInstancing = std::get<1>(it->second);
		MeshInstanceContainer& meshMap = std::get<2>(it->second);

		// On indique la présence de modèles dans cette partie de la map
		used = true;

		// Si nous insérons ce mesh pour la première fois, nous ajoutons des listeners sur ses buffers
		MeshInstanceContainer::iterator it2 = meshMap.find(meshData);
		if (it2 == meshMap.end())
		{
			it2 = meshMap.insert(std::make_pair(meshData, MeshInstanceContainer::mapped_type())).first;

			if (meshData.indexBuffer)
				meshData.indexBuffer->AddResourceListener(this, ResourceType_IndexBuffer);

			meshData.vertexBuffer->AddResourceListener(this, ResourceType_VertexBuffer);
		}

		// On ajoute la matrice à la liste des instances de cet objet
		std::vector<NzMatrix4f>& instances = it2->second;
		instances.push_back(transformMatrix);

		// Avons-nous suffisamment d'instances pour que le coût d'utilisation de l'instancing soit payé ?
		if (instances.size() >= NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT)
			enableInstancing = true; // Apparemment oui, activons l'instancing avec ce matériau
	}
}

void NzDeferredRenderQueue::AddSprite(const NzSprite* sprite)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!sprite)
	{
		NazaraError("Invalid sprite");
		return;
	}

	if (!sprite->IsDrawable())
	{
		NazaraError("Sprite is not drawable");
		return;
	}
	#endif

	/*NzMaterial* material = sprite->GetMaterial();
	if (!material->IsLightingEnabled() || material->IsEnabled(nzRendererParameter_Blend))
		m_forwardQueue->AddSprite(sprite);
	else
		sprites[material].push_back(sprite);*/

	m_forwardQueue->AddSprite(sprite);
}

void NzDeferredRenderQueue::Clear(bool fully)
{
	directionalLights.clear();
	pointLights.clear();
	spotLights.clear();

	if (fully)
	{
		for (auto& matIt : opaqueModels)
		{
			const NzMaterial* material = matIt.first;
			material->RemoveResourceListener(this);

			MeshInstanceContainer& instances = std::get<2>(matIt.second);
			for (auto& instanceIt : instances)
			{
				const NzMeshData& renderData = instanceIt.first;

				if (renderData.indexBuffer)
					renderData.indexBuffer->RemoveResourceListener(this);

				renderData.vertexBuffer->RemoveResourceListener(this);
			}
		}

		opaqueModels.clear();
		sprites.clear();
	}

	m_forwardQueue->Clear(fully);
}

bool NzDeferredRenderQueue::OnResourceDestroy(const NzResource* resource, int index)
{
	switch (index)
	{
		case ResourceType_IndexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = std::get<2>(modelPair.second);
				for (auto it = meshes.begin(); it != meshes.end();)
				{
					const NzMeshData& renderData = it->first;
					if (renderData.indexBuffer == resource)
						it = meshes.erase(it);
					else
						++it;
				}
			}
			break;
		}

		case ResourceType_Material:
			opaqueModels.erase(static_cast<const NzMaterial*>(resource));
			break;

		case ResourceType_VertexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = std::get<2>(modelPair.second);
				for (auto it = meshes.begin(); it != meshes.end();)
				{
					const NzMeshData& renderData = it->first;
					if (renderData.vertexBuffer == resource)
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

void NzDeferredRenderQueue::OnResourceReleased(const NzResource* resource, int index)
{
	// La ressource vient d'être libérée, nous ne pouvons donc plus utiliser la méthode traditionnelle de recherche
	// des pointeurs stockés (À cause de la fonction de triage utilisant des spécificités des ressources)

	switch (index)
	{
		case ResourceType_IndexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = std::get<2>(modelPair.second);
				for (auto it = meshes.begin(); it != meshes.end();)
				{
					const NzMeshData& renderData = it->first;
					if (renderData.indexBuffer == resource)
						it = meshes.erase(it);
					else
						++it;
				}
			}
			break;
		}

		case ResourceType_Material:
		{
			for (auto it = opaqueModels.begin(); it != opaqueModels.end(); ++it)
			{
				if (it->first == resource)
				{
					opaqueModels.erase(it);
					break;
				}
			}
			break;
		}

		case ResourceType_VertexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = std::get<2>(modelPair.second);
				for (auto it = meshes.begin(); it != meshes.end();)
				{
					const NzMeshData& renderData = it->first;
					if (renderData.vertexBuffer == resource)
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

bool NzDeferredRenderQueue::BatchedSpriteMaterialComparator::operator()(const NzMaterial* mat1, const NzMaterial* mat2)
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
