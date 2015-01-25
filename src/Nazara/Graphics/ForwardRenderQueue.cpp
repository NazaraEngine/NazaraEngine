// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Remplacer les sinus/cosinus par une lookup table (va booster les perfs d'un bon x10)

namespace
{
	enum ObjectType
	{
		ObjectType_IndexBuffer,
		ObjectType_Material,
		ObjectType_Texture,
		ObjectType_VertexBuffer
	};
}

void NzForwardRenderQueue::AddBillboard(const NzMaterial* material, const NzVector3f& position, const NzVector2f& size, const NzVector2f& sinCos, const NzColor& color)
{
	auto it = billboards.find(material);
	if (it == billboards.end())
	{
		BatchedBillboardEntry entry(this, ObjectType_Material);
		entry.materialListener = material;

		it = billboards.insert(std::make_pair(material, std::move(entry))).first;
	}

	BatchedBillboardEntry& entry = it->second;

	auto& billboardVector = entry.billboards;
	billboardVector.push_back(BillboardData{color, position, size, sinCos});
}

void NzForwardRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const NzColor> colorPtr)
{
	///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seont remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	NzVector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

	if (!sinCosPtr)
		sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	if (!colorPtr)
		colorPtr.Reset(&NzColor::White, 0); // Pareil

	auto it = billboards.find(material);
	if (it == billboards.end())
	{
		BatchedBillboardEntry entry(this, ObjectType_Material);
		entry.materialListener = material;

		it = billboards.insert(std::make_pair(material, std::move(entry))).first;
	}

	BatchedBillboardEntry& entry = it->second;

	auto& billboardVector = entry.billboards;
	unsigned int prevSize = billboardVector.size();
	billboardVector.resize(prevSize + count);

	BillboardData* billboardData = &billboardVector[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		billboardData->center = *positionPtr++;
		billboardData->color = *colorPtr++;
		billboardData->sinCos = *sinCosPtr++;
		billboardData->size = *sizePtr++;
		billboardData++;
	}
}

void NzForwardRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr)
{
	///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seont remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	NzVector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

	if (!sinCosPtr)
		sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	float defaultAlpha = 1.f;

	if (!alphaPtr)
		alphaPtr.Reset(&defaultAlpha, 0); // Pareil

	auto it = billboards.find(material);
	if (it == billboards.end())
	{
		BatchedBillboardEntry entry(this, ObjectType_Material);
		entry.materialListener = material;

		it = billboards.insert(std::make_pair(material, std::move(entry))).first;
	}

	BatchedBillboardEntry& entry = it->second;

	auto& billboardVector = entry.billboards;
	unsigned int prevSize = billboardVector.size();
	billboardVector.resize(prevSize + count);

	BillboardData* billboardData = &billboardVector[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		billboardData->center = *positionPtr++;
		billboardData->color = NzColor(255, 255, 255, static_cast<nzUInt8>(255.f * (*alphaPtr++)));
		billboardData->sinCos = *sinCosPtr++;
		billboardData->size = *sizePtr++;
		billboardData++;
	}
}

void NzForwardRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr)
{
	///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seont remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	float defaultRotation = 0.f;

	if (!anglePtr)
		anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	if (!colorPtr)
		colorPtr.Reset(&NzColor::White, 0); // Pareil

	auto it = billboards.find(material);
	if (it == billboards.end())
	{
		BatchedBillboardEntry entry(this, ObjectType_Material);
		entry.materialListener = material;

		it = billboards.insert(std::make_pair(material, std::move(entry))).first;
	}

	BatchedBillboardEntry& entry = it->second;

	auto& billboardVector = entry.billboards;
	unsigned int prevSize = billboardVector.size();
	billboardVector.resize(prevSize + count);

	BillboardData* billboardData = &billboardVector[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		float sin = std::sin(NzToRadians(*anglePtr));
		float cos = std::cos(NzToRadians(*anglePtr));
		anglePtr++;

		billboardData->center = *positionPtr++;
		billboardData->color = *colorPtr++;
		billboardData->sinCos.Set(sin, cos);
		billboardData->size = *sizePtr++;
		billboardData++;
	}
}

void NzForwardRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr)
{
	///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seont remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	float defaultRotation = 0.f;

	if (!anglePtr)
		anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	float defaultAlpha = 1.f;

	if (!alphaPtr)
		alphaPtr.Reset(&defaultAlpha, 0); // Pareil

	auto it = billboards.find(material);
	if (it == billboards.end())
	{
		BatchedBillboardEntry entry(this, ObjectType_Material);
		entry.materialListener = material;

		it = billboards.insert(std::make_pair(material, std::move(entry))).first;
	}

	BatchedBillboardEntry& entry = it->second;

	auto& billboardVector = entry.billboards;
	unsigned int prevSize = billboardVector.size();
	billboardVector.resize(prevSize + count);

	BillboardData* billboardData = &billboardVector[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		float sin = std::sin(NzToRadians(*anglePtr));
		float cos = std::cos(NzToRadians(*anglePtr));
		anglePtr++;

		billboardData->center = *positionPtr++;
		billboardData->color = NzColor(255, 255, 255, static_cast<nzUInt8>(255.f * (*alphaPtr++)));
		billboardData->sinCos.Set(sin, cos);
		billboardData->size = *sizePtr++;
		billboardData++;
	}
}

void NzForwardRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const NzColor> colorPtr)
{
	///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seont remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	NzVector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

	if (!sinCosPtr)
		sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	if (!colorPtr)
		colorPtr.Reset(&NzColor::White, 0); // Pareil

	auto it = billboards.find(material);
	if (it == billboards.end())
	{
		BatchedBillboardEntry entry(this, ObjectType_Material);
		entry.materialListener = material;

		it = billboards.insert(std::make_pair(material, std::move(entry))).first;
	}

	BatchedBillboardEntry& entry = it->second;

	auto& billboardVector = entry.billboards;
	unsigned int prevSize = billboardVector.size();
	billboardVector.resize(prevSize + count);

	BillboardData* billboardData = &billboardVector[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		billboardData->center = *positionPtr++;
		billboardData->color = *colorPtr++;
		billboardData->sinCos = *sinCosPtr++;
		billboardData->size.Set(*sizePtr++);
		billboardData++;
	}
}

void NzForwardRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr)
{
	///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seont remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	NzVector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

	if (!sinCosPtr)
		sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	float defaultAlpha = 1.f;

	if (!alphaPtr)
		alphaPtr.Reset(&defaultAlpha, 0); // Pareil

	auto it = billboards.find(material);
	if (it == billboards.end())
	{
		BatchedBillboardEntry entry(this, ObjectType_Material);
		entry.materialListener = material;

		it = billboards.insert(std::make_pair(material, std::move(entry))).first;
	}

	BatchedBillboardEntry& entry = it->second;

	auto& billboardVector = entry.billboards;
	unsigned int prevSize = billboardVector.size();
	billboardVector.resize(prevSize + count);

	BillboardData* billboardData = &billboardVector[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		billboardData->center = *positionPtr++;
		billboardData->color = NzColor(255, 255, 255, static_cast<nzUInt8>(255.f * (*alphaPtr++)));
		billboardData->sinCos = *sinCosPtr++;
		billboardData->size.Set(*sizePtr++);
		billboardData++;
	}
}

void NzForwardRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr)
{
	///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seont remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	float defaultRotation = 0.f;

	if (!anglePtr)
		anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	if (!colorPtr)
		colorPtr.Reset(&NzColor::White, 0); // Pareil

	auto it = billboards.find(material);
	if (it == billboards.end())
	{
		BatchedBillboardEntry entry(this, ObjectType_Material);
		entry.materialListener = material;

		it = billboards.insert(std::make_pair(material, std::move(entry))).first;
	}

	BatchedBillboardEntry& entry = it->second;

	auto& billboardVector = entry.billboards;
	unsigned int prevSize = billboardVector.size();
	billboardVector.resize(prevSize + count);

	BillboardData* billboardData = &billboardVector[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		float sin = std::sin(NzToRadians(*anglePtr));
		float cos = std::cos(NzToRadians(*anglePtr));
		anglePtr++;

		billboardData->center = *positionPtr++;
		billboardData->color = *colorPtr++;
		billboardData->sinCos.Set(sin, cos);
		billboardData->size.Set(*sizePtr++);
		billboardData++;
	}
}

void NzForwardRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr)
{
	///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seont remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	float defaultRotation = 0.f;

	if (!anglePtr)
		anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	float defaultAlpha = 1.f;

	if (!alphaPtr)
		alphaPtr.Reset(&defaultAlpha, 0); // Pareil

	auto it = billboards.find(material);
	if (it == billboards.end())
	{
		BatchedBillboardEntry entry(this, ObjectType_Material);
		entry.materialListener = material;

		it = billboards.insert(std::make_pair(material, std::move(entry))).first;
	}

	BatchedBillboardEntry& entry = it->second;

	auto& billboardVector = entry.billboards;
	unsigned int prevSize = billboardVector.size();
	billboardVector.resize(prevSize + count);

	BillboardData* billboardData = &billboardVector[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		float sin = std::sin(NzToRadians(*anglePtr));
		float cos = std::cos(NzToRadians(*anglePtr));
		anglePtr++;

		billboardData->center = *positionPtr++;
		billboardData->color = NzColor(255, 255, 255, static_cast<nzUInt8>(255.f * (*alphaPtr++)));
		billboardData->sinCos.Set(sin, cos);
		billboardData->size.Set(*sizePtr++);
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
		data.material = material;
		data.meshData = meshData;
		data.squaredBoundingSphere = NzSpheref(transformMatrix.GetTranslation() + meshAABB.GetCenter(), meshAABB.GetSquaredRadius());
		data.transformMatrix = transformMatrix;

		transparentModels.push_back(index);
	}
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
		BatchedBasicSpriteEntry entry(this, ObjectType_Material);
		entry.materialListener = material;

		matIt = basicSprites.insert(std::make_pair(material, std::move(entry))).first;
	}

	BatchedBasicSpriteEntry& entry = matIt->second;
	entry.enabled = true;

	auto& overlayMap = entry.overlayMap;

	auto overlayIt = overlayMap.find(overlay);
	if (overlayIt == overlayMap.end())
	{
		BatchedSpriteEntry overlayEntry(this, ObjectType_Texture);
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
		billboards.clear();
		opaqueModels.clear();
	}
}

void NzForwardRenderQueue::Sort(const NzAbstractViewer* viewer)
{
	NzPlanef nearPlane = viewer->GetFrustum().GetPlane(nzFrustumPlane_Near);
	NzVector3f viewerPos = viewer->GetEyePosition();
	NzVector3f viewerNormal = viewer->GetForward();

	std::sort(transparentModels.begin(), transparentModels.end(), [this, &nearPlane, &viewerNormal](unsigned int index1, unsigned int index2)
	{
		const NzSpheref& sphere1 = transparentModelData[index1].squaredBoundingSphere;
		const NzSpheref& sphere2 = transparentModelData[index2].squaredBoundingSphere;

		NzVector3f position1 = sphere1.GetNegativeVertex(viewerNormal);
		NzVector3f position2 = sphere2.GetNegativeVertex(viewerNormal);

		return nearPlane.Distance(position1) > nearPlane.Distance(position2);
	});

	for (auto& pair : billboards)
	{
		const NzMaterial* mat = pair.first;

		if (mat->IsDepthSortingEnabled())
		{
			BatchedBillboardEntry& entry = pair.second;
			auto& billboardVector = entry.billboards;

			std::sort(billboardVector.begin(), billboardVector.end(), [&viewerPos](const BillboardData& data1, const BillboardData& data2)
			{
				return viewerPos.SquaredDistance(data1.center) > viewerPos.SquaredDistance(data2.center);
			});
		}
	}
}

bool NzForwardRenderQueue::OnObjectDestroy(const NzRefCounted* object, int index)
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

			basicSprites.erase(material);
			billboards.erase(material);
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

void NzForwardRenderQueue::OnObjectReleased(const NzRefCounted* object, int index)
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
			for (auto it = basicSprites.begin(); it != basicSprites.end(); ++it)
			{
				if (it->first == object)
				{
					basicSprites.erase(it);
					break;
				}
			}

			for (auto it = billboards.begin(); it != billboards.end(); ++it)
			{
				if (it->first == object)
				{
					billboards.erase(it);
					break;
				}
			}

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

		case ObjectType_Texture:
		{
			for (auto matIt = basicSprites.begin(); matIt != basicSprites.end(); ++matIt)
			{
				auto& overlayMap = matIt->second.overlayMap;
				for (auto overlayIt = overlayMap.begin(); overlayIt != overlayMap.end(); ++overlayIt)
				{
					if (overlayIt->first == object)
					{
						overlayMap.erase(overlayIt);
						break;
					}
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

bool NzForwardRenderQueue::BatchedBillboardComparator::operator()(const NzMaterial* mat1, const NzMaterial* mat2)
{
	const NzUberShader* uberShader1 = mat1->GetShader();
	const NzUberShader* uberShader2 = mat2->GetShader();
	if (uberShader1 != uberShader2)
		return uberShader1 < uberShader2;

	const NzShader* shader1 = mat1->GetShaderInstance(nzShaderFlags_Billboard | nzShaderFlags_VertexColor)->GetShader();
	const NzShader* shader2 = mat2->GetShaderInstance(nzShaderFlags_Billboard | nzShaderFlags_VertexColor)->GetShader();
	if (shader1 != shader2)
		return shader1 < shader2;

	const NzTexture* diffuseMap1 = mat1->GetDiffuseMap();
	const NzTexture* diffuseMap2 = mat2->GetDiffuseMap();
	if (diffuseMap1 != diffuseMap2)
		return diffuseMap1 < diffuseMap2;

	return mat1 < mat2;
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
