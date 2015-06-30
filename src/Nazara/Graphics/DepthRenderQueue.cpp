// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Remplacer les sinus/cosinus par une lookup table (va booster les perfs d'un bon x10)

void NzDepthRenderQueue::AddBillboard(const NzMaterial* material, const NzVector3f& position, const NzVector2f& size, const NzVector2f& sinCos, const NzColor& color)
{
	NazaraAssert(material, "Invalid material");

	if (IsMaterialSuitable(material))
		billboards.push_back(BillboardData{color, position, size, sinCos});
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const NzColor> colorPtr)
{
	///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	NzVector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

	if (!sinCosPtr)
		sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	if (!colorPtr)
		colorPtr.Reset(&NzColor::White, 0); // Pareil

	unsigned int prevSize = billboards.size();
	billboards.resize(prevSize + count);

	BillboardData* billboardData = &billboards[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		billboardData->center = *positionPtr++;
		billboardData->color = *colorPtr++;
		billboardData->sinCos = *sinCosPtr++;
		billboardData->size = *sizePtr++;
		billboardData++;
	}
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr)
{
	///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	NzVector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

	if (!sinCosPtr)
		sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	float defaultAlpha = 1.f;

	if (!alphaPtr)
		alphaPtr.Reset(&defaultAlpha, 0); // Pareil

	unsigned int prevSize = billboards.size();
	billboards.resize(prevSize + count);

	BillboardData* billboardData = &billboards[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		billboardData->center = *positionPtr++;
		billboardData->color = NzColor(255, 255, 255, static_cast<nzUInt8>(255.f * (*alphaPtr++)));
		billboardData->sinCos = *sinCosPtr++;
		billboardData->size = *sizePtr++;
		billboardData++;
	}
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr)
{
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	float defaultRotation = 0.f;

	if (!anglePtr)
		anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	if (!colorPtr)
		colorPtr.Reset(&NzColor::White, 0); // Pareil

	unsigned int prevSize = billboards.size();
	billboards.resize(prevSize + count);

	BillboardData* billboardData = &billboards[prevSize];
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

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr)
{
	///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	float defaultRotation = 0.f;

	if (!anglePtr)
		anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	float defaultAlpha = 1.f;

	if (!alphaPtr)
		alphaPtr.Reset(&defaultAlpha, 0); // Pareil

	unsigned int prevSize = billboards.size();
	billboards.resize(prevSize + count);

	BillboardData* billboardData = &billboards[prevSize];
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

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const NzColor> colorPtr)
{
	///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	NzVector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

	if (!sinCosPtr)
		sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	if (!colorPtr)
		colorPtr.Reset(&NzColor::White, 0); // Pareil

	unsigned int prevSize = billboards.size();
	billboards.resize(prevSize + count);

	BillboardData* billboardData = &billboards[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		billboardData->center = *positionPtr++;
		billboardData->color = *colorPtr++;
		billboardData->sinCos = *sinCosPtr++;
		billboardData->size.Set(*sizePtr++);
		billboardData++;
	}
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr)
{
	///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	NzVector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

	if (!sinCosPtr)
		sinCosPtr.Reset(&defaultSinCos, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	float defaultAlpha = 1.f;

	if (!alphaPtr)
		alphaPtr.Reset(&defaultAlpha, 0); // Pareil

	unsigned int prevSize = billboards.size();
	billboards.resize(prevSize + count);

	BillboardData* billboardData = &billboards[prevSize];
	for (unsigned int i = 0; i < count; ++i)
	{
		billboardData->center = *positionPtr++;
		billboardData->color = NzColor(255, 255, 255, static_cast<nzUInt8>(255.f * (*alphaPtr++)));
		billboardData->sinCos = *sinCosPtr++;
		billboardData->size.Set(*sizePtr++);
		billboardData++;
	}
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr)
{
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	///DOC: sinCosPtr et colorPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	float defaultRotation = 0.f;

	if (!anglePtr)
		anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	if (!colorPtr)
		colorPtr.Reset(&NzColor::White, 0); // Pareil

	unsigned int prevSize = billboards.size();
	billboards.resize(prevSize + count);

	BillboardData* billboardData = &billboards[prevSize];
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

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr)
{
	///DOC: sinCosPtr et alphaPtr peuvent être nuls, ils seront remplacés respectivement par Vector2f(0.f, 1.f) et Color::White
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	float defaultRotation = 0.f;

	if (!anglePtr)
		anglePtr.Reset(&defaultRotation, 0); // L'astuce ici est de mettre le stride sur zéro, rendant le pointeur immobile

	float defaultAlpha = 1.f;

	if (!alphaPtr)
		alphaPtr.Reset(&defaultAlpha, 0); // Pareil

	unsigned int prevSize = billboards.size();
	billboards.resize(prevSize + count);

	BillboardData* billboardData = &billboards[prevSize];
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

void NzDepthRenderQueue::AddDrawable(const NzDrawable* drawable)
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

void NzDepthRenderQueue::AddMesh(const NzMaterial* material, const NzMeshData& meshData, const NzBoxf& meshAABB, const NzMatrix4f& transformMatrix)
{
	NazaraAssert(material, "Invalid material");
	NazaraUnused(meshAABB);

	if (!IsMaterialSuitable(material))
		return;

	auto it = meshes.find(meshData);
	if (it == meshes.end())
	{
		MeshInstanceEntry instanceEntry;

		if (meshData.indexBuffer)
			instanceEntry.indexBufferReleaseSlot.Connect(meshData.indexBuffer->OnIndexBufferRelease, this, &NzDepthRenderQueue::OnIndexBufferInvalidation);

		instanceEntry.vertexBufferReleaseSlot.Connect(meshData.vertexBuffer->OnVertexBufferRelease, this, &NzDepthRenderQueue::OnVertexBufferInvalidation);

		it = meshes.insert(std::make_pair(meshData, std::move(instanceEntry))).first;
	}

	std::vector<NzMatrix4f>& instances = it->second.instances;
	instances.push_back(transformMatrix);

	// Avons-nous suffisamment d'instances pour que le coût d'utilisation de l'instancing soit payé ?
	//if (instances.size() >= NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT)
	//	entry.instancingEnabled = true; // Apparemment oui, activons l'instancing avec ce matériau
}

void NzDepthRenderQueue::AddSprites(const NzMaterial* material, const NzVertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const NzTexture* overlay)
{
	NazaraAssert(material, "Invalid material");
	NazaraUnused(overlay);

	if (!IsMaterialSuitable(material))
		return;

	basicSprites.push_back(SpriteChain_XYZ_Color_UV({vertices, spriteCount}));
}

void NzDepthRenderQueue::Clear(bool fully)
{
	NzAbstractRenderQueue::Clear(fully);

	basicSprites.clear();
	billboards.clear();
	otherDrawables.clear();

	if (fully)
		meshes.clear();
}

bool NzDepthRenderQueue::IsMaterialSuitable(const NzMaterial* material) const
{
	NazaraAssert(material, "Invalid material");

	return material->IsEnabled(nzRendererParameter_DepthBuffer) && material->IsEnabled(nzRendererParameter_DepthWrite);
}

void NzDepthRenderQueue::OnIndexBufferInvalidation(const NzIndexBuffer* indexBuffer)
{
	for (auto it = meshes.begin(); it != meshes.end();)
	{
		const NzMeshData& renderData = it->first;
		if (renderData.indexBuffer == indexBuffer)
			it = meshes.erase(it);
		else
			++it;
	}
}

void NzDepthRenderQueue::OnVertexBufferInvalidation(const NzVertexBuffer* vertexBuffer)
{
	for (auto it = meshes.begin(); it != meshes.end();)
	{
		const NzMeshData& renderData = it->first;
		if (renderData.vertexBuffer == vertexBuffer)
			it = meshes.erase(it);
		else
			++it;
	}
}

