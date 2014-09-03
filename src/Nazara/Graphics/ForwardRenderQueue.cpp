// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
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

NzForwardRenderQueue::~NzForwardRenderQueue()
{
	Clear(true);
}

void NzForwardRenderQueue::AddBillboard(const NzMaterial* material, const NzVector3f& position, const NzVector2f& size, const NzVector2f& sinCos, const NzColor& color)
{
	billboards[material].push_back(BillboardData{color, position, size, sinCos});
}

void NzForwardRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const NzColor> colorPtr)
{
	///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seont remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	NzVector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

	if (!sinCosPtr)
		sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	if (!colorPtr)
		colorPtr.Reset(&NzColor::White, 0); // Pareil

	std::vector<BillboardData>& billboardVec = billboards[material];
	unsigned int prevSize = billboardVec.size();
	billboardVec.resize(prevSize + count);

	BillboardData* billboardData = &billboardVec[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		billboardData->center = *positionPtr++;
		billboardData->color = *colorPtr++;
		billboardData->sinCos = *sinCosPtr++;
		billboardData->size = *sizePtr++;
		billboardData++;
	}
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
		// Le matériau est transparent, nous devons rendre ce mesh d'une autre façon (après le rendu des objets opaques et en les triant)
		unsigned int index = transparentModelData.size();
		transparentModelData.resize(index+1);

		TransparentModelData& data = transparentModelData.back();
		data.boundingSphere = NzSpheref(transformMatrix.GetTranslation() + meshAABB.GetCenter(), meshAABB.GetSquaredRadius());
		data.material = material;
		data.meshData = meshData;
		data.transformMatrix = transformMatrix;

		transparentModels.push_back(index);
	}
	else
	{
		ModelBatches::iterator it = opaqueModels.find(material);
		if (it == opaqueModels.end())
		{
			// Première utilisation du matériau, ajoutons-nous comme listener
			it = opaqueModels.insert(std::make_pair(material, ModelBatches::mapped_type())).first;
			material->AddResourceListener(this, ResourceType_Material);
		}

		bool& used = std::get<0>(it->second);
		bool& enableInstancing = std::get<1>(it->second);
		MeshInstanceContainer& meshMap = std::get<2>(it->second);

		used = true;

		MeshInstanceContainer::iterator it2 = meshMap.find(meshData);
		if (it2 == meshMap.end())
		{
			it2 = meshMap.insert(std::make_pair(meshData, MeshInstanceContainer::mapped_type())).first;

			NzSpheref& squaredBoundingSphere = it2->second.first;
			squaredBoundingSphere.Set(meshAABB.GetSquaredBoundingSphere());

			if (meshData.indexBuffer)
				meshData.indexBuffer->AddResourceListener(this, ResourceType_IndexBuffer);

			meshData.vertexBuffer->AddResourceListener(this, ResourceType_VertexBuffer);
		}

		std::vector<NzMatrix4f>& instances = it2->second.second;
		instances.push_back(transformMatrix);

		// Avons-nous suffisamment d'instances pour que le coût d'utilisation de l'instancing soit payé ?
		if (instances.size() >= NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT)
			enableInstancing = true; // Apparemment oui, activons l'instancing avec ce matériau
	}
}

void NzForwardRenderQueue::AddSprite(const NzSprite* sprite)
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

	sprites[sprite->GetMaterial()].push_back(sprite);
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

		billboards.clear();
		opaqueModels.clear();
		sprites.clear();
	}
}

void NzForwardRenderQueue::Sort(const NzAbstractViewer* viewer)
{
	NzPlanef nearPlane = viewer->GetFrustum().GetPlane(nzFrustumPlane_Near);
	NzVector3f viewerPos = viewer->GetEyePosition();
	NzVector3f viewerNormal = viewer->GetForward();

	std::sort(transparentModels.begin(), transparentModels.end(), [this, &nearPlane, &viewerNormal](unsigned int index1, unsigned int index2)
	{
		const NzSpheref& sphere1 = transparentModelData[index1].boundingSphere;
		const NzSpheref& sphere2 = transparentModelData[index2].boundingSphere;

		NzVector3f position1 = sphere1.GetNegativeVertex(viewerNormal);
		NzVector3f position2 = sphere2.GetNegativeVertex(viewerNormal);

		return nearPlane.Distance(position1) > nearPlane.Distance(position2);
	});

	for (auto& pair : billboards)
	{
		const NzMaterial* mat = pair.first;
		auto& container = pair.second;

		if (mat->IsEnabled(nzRendererParameter_Blend))
		{
			std::sort(container.begin(), container.end(), [&viewerPos](const BillboardData& data1, const BillboardData& data2)
			{
				return viewerPos.SquaredDistance(data1.center) > viewerPos.SquaredDistance(data2.center);
			});
		}
	}
}

bool NzForwardRenderQueue::OnResourceDestroy(const NzResource* resource, int index)
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
		{
			const NzMaterial* material = static_cast<const NzMaterial*>(resource);

			billboards.erase(material);
			opaqueModels.erase(material);
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
