// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	enum ResourceType
	{
		ResourceType_IndexBuffer,
		ResourceType_Material,
		ResourceType_Texture,
		ResourceType_VertexBuffer
	};
}

void NzForwardRenderQueue::AddDrawable(const NzDrawable* drawable)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!drawable)
	{
		NazaraError("Invalid drawable");
		return;
	}
	#endif

	otherDrawables.push_back(drawable);
}

void NzForwardRenderQueue::AddLight(const NzLight* light)
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
		case nzLightType_Spot:
			lights.push_back(light);
			break;

		#ifdef NAZARA_DEBUG
		default:
			NazaraError("Light type not handled (0x" + NzString::Number(light->GetLightType(), 16) + ')');
		#endif
	}
}

void NzForwardRenderQueue::AddMesh(const NzMaterial* material, const NzMeshData& meshData, const NzBoxf& meshAABB, const NzMatrix4f& transformMatrix)
{
	if (material->IsEnabled(nzRendererParameter_Blend))
	{
		unsigned int index = transparentModelData.size();
		transparentModelData.resize(index+1);

		TransparentModelData& data = transparentModelData.back();
		data.material = material;
		data.meshData = meshData;
		data.squaredBoundingSphere = NzSpheref(transformMatrix.GetTranslation() + meshAABB.GetCenter(), meshAABB.GetSquaredRadius());
		data.transformMatrix = transformMatrix;

		transparentModels.push_back(index);
	}
	else
	{
		ModelBatches::iterator it = opaqueModels.find(material);
		if (it == opaqueModels.end())
		{
			BatchedModelEntry entry(this, ResourceType_Material);
			entry.materialListener = material;

			it = opaqueModels.insert(std::make_pair(material, std::move(entry))).first;
		}

		BatchedModelEntry& entry = it->second;
		entry.enabled = true;

		auto& meshMap = entry.meshMap;

		auto it2 = meshMap.find(meshData);
		if (it2 == meshMap.end())
		{
			MeshInstanceEntry instanceEntry(this, ResourceType_IndexBuffer, ResourceType_VertexBuffer);
			instanceEntry.indexBufferListener = meshData.indexBuffer;
			instanceEntry.squaredBoundingSphere = meshAABB.GetSquaredBoundingSphere();
			instanceEntry.vertexBufferListener = meshData.vertexBuffer;

			it2 = meshMap.insert(std::make_pair(meshData, std::move(instanceEntry))).first;
		}

		std::vector<NzMatrix4f>& instances = it2->second.instances;
		instances.push_back(transformMatrix);

		// Avons-nous suffisamment d'instances pour que le coût d'utilisation de l'instancing soit payé ?
		if (instances.size() >= NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT)
			entry.instancingEnabled = true; // Apparemment oui, activons l'instancing avec ce matériau
	}
}

void NzForwardRenderQueue::AddSprites(const NzMaterial* material, const NzVertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const NzTexture* overlay)
{
	auto matIt = basicSprites.find(material);
	if (matIt == basicSprites.end())
	{
		BatchedBasicSpriteEntry entry(this, ResourceType_Material);
		entry.materialListener = material;

		matIt = basicSprites.insert(std::make_pair(material, std::move(entry))).first;
	}

	BatchedBasicSpriteEntry& entry = matIt->second;
	entry.enabled = true;

	auto& overlayMap = entry.overlayMap;

	auto overlayIt = overlayMap.find(overlay);
	if (overlayIt == overlayMap.end())
	{
		BatchedSpriteEntry overlayEntry(this, ResourceType_Texture);
		overlayEntry.textureListener = overlay;

		overlayIt = overlayMap.insert(std::make_pair(overlay, std::move(overlayEntry))).first;
	}

	auto& spriteVector = overlayIt->second.spriteChains;
	spriteVector.push_back(SpriteChain_XYZ_Color_UV({vertices, spriteCount}));
}

void NzForwardRenderQueue::Clear(bool fully)
{
	directionalLights.clear();
	lights.clear();
	otherDrawables.clear();
	transparentModels.clear();
	transparentModelData.clear();

	if (fully)
	{
		basicSprites.clear();
		opaqueModels.clear();
	}
}

void NzForwardRenderQueue::Sort(const NzAbstractViewer* viewer)
{
	NzPlanef nearPlane = viewer->GetFrustum().GetPlane(nzFrustumPlane_Near);
	NzVector3f viewerNormal = viewer->GetForward();

	std::sort(transparentModels.begin(), transparentModels.end(), [this, &nearPlane, &viewerNormal](unsigned int index1, unsigned int index2)
	{
		const NzSpheref& sphere1 = transparentModelData[index1].squaredBoundingSphere;
		const NzSpheref& sphere2 = transparentModelData[index2].squaredBoundingSphere;

		NzVector3f position1 = sphere1.GetNegativeVertex(viewerNormal);
		NzVector3f position2 = sphere2.GetNegativeVertex(viewerNormal);

		return nearPlane.Distance(position1) > nearPlane.Distance(position2);
	});
}

bool NzForwardRenderQueue::OnResourceDestroy(const NzResource* resource, int index)
{
	switch (index)
	{
		case ResourceType_IndexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = modelPair.second.meshMap;
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
			const NzMaterial* material = static_cast<const NzMaterial*>(resource);

			basicSprites.erase(material);
			opaqueModels.erase(material);
			break;
		}

		case ResourceType_VertexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = modelPair.second.meshMap;
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

void NzForwardRenderQueue::OnResourceReleased(const NzResource* resource, int index)
{
	// La ressource vient d'être libérée, nous ne pouvons donc plus utiliser la méthode traditionnelle de recherche
	// des pointeurs stockés (À cause de la fonction de triage utilisant des spécificités des ressources)

	switch (index)
	{
		case ResourceType_IndexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = modelPair.second.meshMap;
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
			for (auto it = basicSprites.begin(); it != basicSprites.end(); ++it)
			{
				if (it->first == resource)
				{
					basicSprites.erase(it);
					break;
				}
			}

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

		case ResourceType_Texture:
		{
			for (auto matIt = basicSprites.begin(); matIt != basicSprites.end(); ++matIt)
			{
				auto& overlayMap = matIt->second.overlayMap;
				for (auto overlayIt = overlayMap.begin(); overlayIt != overlayMap.end(); ++overlayIt)
				{
					if (overlayIt->first == resource)
					{
						overlayMap.erase(overlayIt);
						break;
					}
				}
			}

			break;
		}

		case ResourceType_VertexBuffer:
		{
			for (auto& modelPair : opaqueModels)
			{
				MeshInstanceContainer& meshes = modelPair.second.meshMap;
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

bool NzForwardRenderQueue::BatchedModelMaterialComparator::operator()(const NzMaterial* mat1, const NzMaterial* mat2)
{
	const NzUberShader* uberShader1 = mat1->GetShader();
	const NzUberShader* uberShader2 = mat2->GetShader();
	if (uberShader1 != uberShader2)
		return uberShader1 < uberShader2;

	const NzShader* shader1 = mat1->GetShaderInstance()->GetShader();
	const NzShader* shader2 = mat2->GetShaderInstance()->GetShader();
	if (shader1 != shader2)
		return shader1 < shader2;

	const NzTexture* diffuseMap1 = mat1->GetDiffuseMap();
	const NzTexture* diffuseMap2 = mat2->GetDiffuseMap();
	if (diffuseMap1 != diffuseMap2)
		return diffuseMap1 < diffuseMap2;

	return mat1 < mat2;
}

bool NzForwardRenderQueue::BatchedSpriteMaterialComparator::operator()(const NzMaterial* mat1, const NzMaterial* mat2)
{
	const NzUberShader* uberShader1 = mat1->GetShader();
	const NzUberShader* uberShader2 = mat2->GetShader();
	if (uberShader1 != uberShader2)
		return uberShader1 < uberShader2;

	const NzShader* shader1 = mat1->GetShaderInstance()->GetShader();
	const NzShader* shader2 = mat2->GetShaderInstance()->GetShader();
	if (shader1 != shader2)
		return shader1 < shader2;

	const NzTexture* diffuseMap1 = mat1->GetDiffuseMap();
	const NzTexture* diffuseMap2 = mat2->GetDiffuseMap();
	if (diffuseMap1 != diffuseMap2)
		return diffuseMap1 < diffuseMap2;

	return mat1 < mat2;
}

bool NzForwardRenderQueue::MeshDataComparator::operator()(const NzMeshData& data1, const NzMeshData& data2)
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
