// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Remplacer les sinus/cosinus par une lookup table (va booster les perfs d'un bon x10)

namespace Nz
{
	void ForwardRenderQueue::AddBillboard(int renderOrder, const Material* material, const Vector3f& position, const Vector2f& size, const Vector2f& sinCos, const Color& color)
	{
		NazaraAssert(material, "Invalid material");

		auto& billboards = GetLayer(renderOrder).billboards;

		auto it = billboards.find(material);
		if (it == billboards.end())
		{
			BatchedBillboardEntry entry;
			entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

			it = billboards.insert(std::make_pair(material, std::move(entry))).first;
		}

		BatchedBillboardEntry& entry = it->second;

		auto& billboardVector = entry.billboards;
		billboardVector.push_back(BillboardData{color, position, size, sinCos});
	}

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seont remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
		Vector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

		if (!sinCosPtr)
			sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

		if (!colorPtr)
			colorPtr.Reset(&Color::White, 0); // Pareil

		auto& billboards = GetLayer(renderOrder).billboards;

		auto it = billboards.find(material);
		if (it == billboards.end())
		{
			BatchedBillboardEntry entry;
			entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

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

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
		Vector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

		if (!sinCosPtr)
			sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

		float defaultAlpha = 1.f;

		if (!alphaPtr)
			alphaPtr.Reset(&defaultAlpha, 0); // Pareil

		auto& billboards = GetLayer(renderOrder).billboards;

		auto it = billboards.find(material);
		if (it == billboards.end())
		{
			BatchedBillboardEntry entry;
			entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

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
			billboardData->color = Color(255, 255, 255, static_cast<UInt8>(255.f * (*alphaPtr++)));
			billboardData->sinCos = *sinCosPtr++;
			billboardData->size = *sizePtr++;
			billboardData++;
		}
	}

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
		float defaultRotation = 0.f;

		if (!anglePtr)
			anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

		if (!colorPtr)
			colorPtr.Reset(&Color::White, 0); // Pareil

		auto& billboards = GetLayer(renderOrder).billboards;

		auto it = billboards.find(material);
		if (it == billboards.end())
		{
			BatchedBillboardEntry entry;
			entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

			it = billboards.insert(std::make_pair(material, std::move(entry))).first;
		}

		BatchedBillboardEntry& entry = it->second;

		auto& billboardVector = entry.billboards;
		unsigned int prevSize = billboardVector.size();
		billboardVector.resize(prevSize + count);

		BillboardData* billboardData = &billboardVector[prevSize];
		for (unsigned int i = 0; i < count; ++i)
		{
			float sin = std::sin(ToRadians(*anglePtr));
			float cos = std::cos(ToRadians(*anglePtr));
			anglePtr++;

			billboardData->center = *positionPtr++;
			billboardData->color = *colorPtr++;
			billboardData->sinCos.Set(sin, cos);
			billboardData->size = *sizePtr++;
			billboardData++;
		}
	}

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
		float defaultRotation = 0.f;

		if (!anglePtr)
			anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

		float defaultAlpha = 1.f;

		if (!alphaPtr)
			alphaPtr.Reset(&defaultAlpha, 0); // Pareil

		auto& billboards = GetLayer(renderOrder).billboards;

		auto it = billboards.find(material);
		if (it == billboards.end())
		{
			BatchedBillboardEntry entry;
			entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

			it = billboards.insert(std::make_pair(material, std::move(entry))).first;
		}

		BatchedBillboardEntry& entry = it->second;

		auto& billboardVector = entry.billboards;
		unsigned int prevSize = billboardVector.size();
		billboardVector.resize(prevSize + count);

		BillboardData* billboardData = &billboardVector[prevSize];
		for (unsigned int i = 0; i < count; ++i)
		{
			float sin = std::sin(ToRadians(*anglePtr));
			float cos = std::cos(ToRadians(*anglePtr));
			anglePtr++;

			billboardData->center = *positionPtr++;
			billboardData->color = Color(255, 255, 255, static_cast<UInt8>(255.f * (*alphaPtr++)));
			billboardData->sinCos.Set(sin, cos);
			billboardData->size = *sizePtr++;
			billboardData++;
		}
	}

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
		Vector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

		if (!sinCosPtr)
			sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

		if (!colorPtr)
			colorPtr.Reset(&Color::White, 0); // Pareil

		auto& billboards = GetLayer(renderOrder).billboards;

		auto it = billboards.find(material);
		if (it == billboards.end())
		{
			BatchedBillboardEntry entry;
			entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

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

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
		Vector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

		if (!sinCosPtr)
			sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

		float defaultAlpha = 1.f;

		if (!alphaPtr)
			alphaPtr.Reset(&defaultAlpha, 0); // Pareil

		auto& billboards = GetLayer(renderOrder).billboards;

		auto it = billboards.find(material);
		if (it == billboards.end())
		{
			BatchedBillboardEntry entry;
			entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

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
			billboardData->color = Color(255, 255, 255, static_cast<UInt8>(255.f * (*alphaPtr++)));
			billboardData->sinCos = *sinCosPtr++;
			billboardData->size.Set(*sizePtr++);
			billboardData++;
		}
	}

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
		float defaultRotation = 0.f;

		if (!anglePtr)
			anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

		if (!colorPtr)
			colorPtr.Reset(&Color::White, 0); // Pareil

		auto& billboards = GetLayer(renderOrder).billboards;

		auto it = billboards.find(material);
		if (it == billboards.end())
		{
			BatchedBillboardEntry entry;
			entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

			it = billboards.insert(std::make_pair(material, std::move(entry))).first;
		}

		BatchedBillboardEntry& entry = it->second;

		auto& billboardVector = entry.billboards;
		unsigned int prevSize = billboardVector.size();
		billboardVector.resize(prevSize + count);

		BillboardData* billboardData = &billboardVector[prevSize];
		for (unsigned int i = 0; i < count; ++i)
		{
			float sin = std::sin(ToRadians(*anglePtr));
			float cos = std::cos(ToRadians(*anglePtr));
			anglePtr++;

			billboardData->center = *positionPtr++;
			billboardData->color = *colorPtr++;
			billboardData->sinCos.Set(sin, cos);
			billboardData->size.Set(*sizePtr++);
			billboardData++;
		}
	}

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
		float defaultRotation = 0.f;

		if (!anglePtr)
			anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

		float defaultAlpha = 1.f;

		if (!alphaPtr)
			alphaPtr.Reset(&defaultAlpha, 0); // Pareil

		auto& billboards = GetLayer(renderOrder).billboards;

		auto it = billboards.find(material);
		if (it == billboards.end())
		{
			BatchedBillboardEntry entry;
			entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

			it = billboards.insert(std::make_pair(material, std::move(entry))).first;
		}

		BatchedBillboardEntry& entry = it->second;

		auto& billboardVector = entry.billboards;
		unsigned int prevSize = billboardVector.size();
		billboardVector.resize(prevSize + count);

		BillboardData* billboardData = &billboardVector[prevSize];
		for (unsigned int i = 0; i < count; ++i)
		{
			float sin = std::sin(ToRadians(*anglePtr));
			float cos = std::cos(ToRadians(*anglePtr));
			anglePtr++;

			billboardData->center = *positionPtr++;
			billboardData->color = Color(255, 255, 255, static_cast<UInt8>(255.f * (*alphaPtr++)));
			billboardData->sinCos.Set(sin, cos);
			billboardData->size.Set(*sizePtr++);
			billboardData++;
		}
	}

	void ForwardRenderQueue::AddDrawable(int renderOrder, const Drawable* drawable)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (!drawable)
		{
			NazaraError("Invalid drawable");
			return;
		}
		#endif

		auto& otherDrawables = GetLayer(renderOrder).otherDrawables;

		otherDrawables.push_back(drawable);
	}

	void ForwardRenderQueue::AddMesh(int renderOrder, const Material* material, const MeshData& meshData, const Boxf& meshAABB, const Matrix4f& transformMatrix)
	{
		if (material->IsEnabled(RendererParameter_Blend))
		{
			Layer& currentLayer = GetLayer(renderOrder);
			auto& transparentModels = currentLayer.transparentModels;
			auto& transparentModelData = currentLayer.transparentModelData;

			// Le matériau est transparent, nous devons rendre ce mesh d'une autre façon (après le rendu des objets opaques et en les triant)
			unsigned int index = transparentModelData.size();
			transparentModelData.resize(index+1);

			TransparentModelData& data = transparentModelData.back();
			data.material = material;
			data.meshData = meshData;
			data.squaredBoundingSphere = Spheref(transformMatrix.GetTranslation() + meshAABB.GetCenter(), meshAABB.GetSquaredRadius());
			data.transformMatrix = transformMatrix;

			transparentModels.push_back(index);
		}
		else
		{
			Layer& currentLayer = GetLayer(renderOrder);
			auto& opaqueModels = currentLayer.opaqueModels;

			auto it = opaqueModels.find(material);
			if (it == opaqueModels.end())
			{
				BatchedModelEntry entry;
				entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

				it = opaqueModels.insert(std::make_pair(material, std::move(entry))).first;
			}

			BatchedModelEntry& entry = it->second;
			entry.enabled = true;

			auto& meshMap = entry.meshMap;

			auto it2 = meshMap.find(meshData);
			if (it2 == meshMap.end())
			{
				MeshInstanceEntry instanceEntry;
				instanceEntry.squaredBoundingSphere = meshAABB.GetSquaredBoundingSphere();

				if (meshData.indexBuffer)
					instanceEntry.indexBufferReleaseSlot.Connect(meshData.indexBuffer->OnIndexBufferRelease, this, &ForwardRenderQueue::OnIndexBufferInvalidation);

				instanceEntry.vertexBufferReleaseSlot.Connect(meshData.vertexBuffer->OnVertexBufferRelease, this, &ForwardRenderQueue::OnVertexBufferInvalidation);

				it2 = meshMap.insert(std::make_pair(meshData, std::move(instanceEntry))).first;
			}

			std::vector<Matrix4f>& instances = it2->second.instances;
			instances.push_back(transformMatrix);

			// Avons-nous suffisamment d'instances pour que le coût d'utilisation de l'instancing soit payé ?
			if (instances.size() >= NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT)
				entry.instancingEnabled = true; // Apparemment oui, activons l'instancing avec ce matériau
		}
	}

	void ForwardRenderQueue::AddSprites(int renderOrder, const Material* material, const VertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const Texture* overlay)
	{
		Layer& currentLayer = GetLayer(renderOrder);
		auto& basicSprites = currentLayer.basicSprites;

		auto matIt = basicSprites.find(material);
		if (matIt == basicSprites.end())
		{
			BatchedBasicSpriteEntry entry;
			entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

			matIt = basicSprites.insert(std::make_pair(material, std::move(entry))).first;
		}

		BatchedBasicSpriteEntry& entry = matIt->second;
		entry.enabled = true;

		auto& overlayMap = entry.overlayMap;

		auto overlayIt = overlayMap.find(overlay);
		if (overlayIt == overlayMap.end())
		{
			BatchedSpriteEntry overlayEntry;
			if (overlay)
				overlayEntry.textureReleaseSlot.Connect(overlay->OnTextureRelease, this, &ForwardRenderQueue::OnTextureInvalidation);

			overlayIt = overlayMap.insert(std::make_pair(overlay, std::move(overlayEntry))).first;
		}

		auto& spriteVector = overlayIt->second.spriteChains;
		spriteVector.push_back(SpriteChain_XYZ_Color_UV({vertices, spriteCount}));
	}

	void ForwardRenderQueue::Clear(bool fully)
	{
		AbstractRenderQueue::Clear(fully);

		if (fully)
			layers.clear();
		else
		{
			for (auto it = layers.begin(); it != layers.end();)
			{
				Layer& layer = it->second;
				if (layer.clearCount++ >= 100)
					layers.erase(it++);
				else
				{
					layer.otherDrawables.clear();
					layer.transparentModels.clear();
					layer.transparentModelData.clear();
					++it;
				}
			}
		}
	}

	void ForwardRenderQueue::Sort(const AbstractViewer* viewer)
	{
		Planef nearPlane = viewer->GetFrustum().GetPlane(FrustumPlane_Near);
		Vector3f viewerPos = viewer->GetEyePosition();
		Vector3f viewerNormal = viewer->GetForward();

		for (auto& pair : layers)
		{
			Layer& layer = pair.second;

			std::sort(layer.transparentModels.begin(), layer.transparentModels.end(), [&layer, &nearPlane, &viewerNormal] (unsigned int index1, unsigned int index2)
			{
				const Spheref& sphere1 = layer.transparentModelData[index1].squaredBoundingSphere;
				const Spheref& sphere2 = layer.transparentModelData[index2].squaredBoundingSphere;

				Vector3f position1 = sphere1.GetNegativeVertex(viewerNormal);
				Vector3f position2 = sphere2.GetNegativeVertex(viewerNormal);

				return nearPlane.Distance(position1) > nearPlane.Distance(position2);
			});

			for (auto& pair : layer.billboards)
			{
				const Material* mat = pair.first;

				if (mat->IsDepthSortingEnabled())
				{
					BatchedBillboardEntry& entry = pair.second;
					auto& billboardVector = entry.billboards;

					std::sort(billboardVector.begin(), billboardVector.end(), [&viewerPos] (const BillboardData& data1, const BillboardData& data2)
					{
						return viewerPos.SquaredDistance(data1.center) > viewerPos.SquaredDistance(data2.center);
					});
				}
			}
		}
	}

	ForwardRenderQueue::Layer& ForwardRenderQueue::GetLayer(int i)
	{
		auto it = layers.find(i);
		if (it == layers.end())
			it = layers.insert(std::make_pair(i, Layer())).first;
		
		Layer& layer = it->second;
		layer.clearCount = 0;

		return layer;
	}

	void ForwardRenderQueue::OnIndexBufferInvalidation(const IndexBuffer* indexBuffer)
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

	void ForwardRenderQueue::OnMaterialInvalidation(const Material* material)
	{
		for (auto& pair : layers)
		{
			Layer& layer = pair.second;

			layer.basicSprites.erase(material);
			layer.billboards.erase(material);
			layer.opaqueModels.erase(material);
		}
	}

	void ForwardRenderQueue::OnTextureInvalidation(const Texture* texture)
	{
		for (auto& pair : layers)
		{
			Layer& layer = pair.second;
			for (auto matIt = layer.basicSprites.begin(); matIt != layer.basicSprites.end(); ++matIt)
			{
				auto& overlayMap = matIt->second.overlayMap;
				overlayMap.erase(texture);
			}
		}
	}

	void ForwardRenderQueue::OnVertexBufferInvalidation(const VertexBuffer* vertexBuffer)
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

	bool ForwardRenderQueue::BatchedBillboardComparator::operator()(const Material* mat1, const Material* mat2) const
	{
		const UberShader* uberShader1 = mat1->GetShader();
		const UberShader* uberShader2 = mat2->GetShader();
		if (uberShader1 != uberShader2)
			return uberShader1 < uberShader2;

		const Shader* shader1 = mat1->GetShaderInstance(ShaderFlags_Billboard | ShaderFlags_VertexColor)->GetShader();
		const Shader* shader2 = mat2->GetShaderInstance(ShaderFlags_Billboard | ShaderFlags_VertexColor)->GetShader();
		if (shader1 != shader2)
			return shader1 < shader2;

		const Texture* diffuseMap1 = mat1->GetDiffuseMap();
		const Texture* diffuseMap2 = mat2->GetDiffuseMap();
		if (diffuseMap1 != diffuseMap2)
			return diffuseMap1 < diffuseMap2;

		return mat1 < mat2;
	}

	bool ForwardRenderQueue::BatchedModelMaterialComparator::operator()(const Material* mat1, const Material* mat2) const
	{
		const UberShader* uberShader1 = mat1->GetShader();
		const UberShader* uberShader2 = mat2->GetShader();
		if (uberShader1 != uberShader2)
			return uberShader1 < uberShader2;

		const Shader* shader1 = mat1->GetShaderInstance()->GetShader();
		const Shader* shader2 = mat2->GetShaderInstance()->GetShader();
		if (shader1 != shader2)
			return shader1 < shader2;

		const Texture* diffuseMap1 = mat1->GetDiffuseMap();
		const Texture* diffuseMap2 = mat2->GetDiffuseMap();
		if (diffuseMap1 != diffuseMap2)
			return diffuseMap1 < diffuseMap2;

		return mat1 < mat2;
	}

	bool ForwardRenderQueue::BatchedSpriteMaterialComparator::operator()(const Material* mat1, const Material* mat2)
	{
		const UberShader* uberShader1 = mat1->GetShader();
		const UberShader* uberShader2 = mat2->GetShader();
		if (uberShader1 != uberShader2)
			return uberShader1 < uberShader2;

		const Shader* shader1 = mat1->GetShaderInstance()->GetShader();
		const Shader* shader2 = mat2->GetShaderInstance()->GetShader();
		if (shader1 != shader2)
			return shader1 < shader2;

		const Texture* diffuseMap1 = mat1->GetDiffuseMap();
		const Texture* diffuseMap2 = mat2->GetDiffuseMap();
		if (diffuseMap1 != diffuseMap2)
			return diffuseMap1 < diffuseMap2;

		return mat1 < mat2;
	}

	bool ForwardRenderQueue::MeshDataComparator::operator()(const MeshData& data1, const MeshData& data2) const
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
