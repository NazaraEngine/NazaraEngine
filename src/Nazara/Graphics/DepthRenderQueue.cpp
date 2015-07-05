// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzDepthRenderQueue::NzDepthRenderQueue()
{
	// Material
	m_baseMaterial = NzMaterial::New();
	m_baseMaterial->Enable(nzRendererParameter_ColorWrite, false);
	m_baseMaterial->Enable(nzRendererParameter_FaceCulling, false);
	//m_baseMaterial->SetFaceCulling(nzFaceSide_Front);
}

void NzDepthRenderQueue::AddBillboard(const NzMaterial* material, const NzVector3f& position, const NzVector2f& size, const NzVector2f& sinCos, const NzColor& color)
{
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	if (material->HasDepthMaterial())
		material = material->GetDepthMaterial();
	else
		material = m_baseMaterial;

	NzForwardRenderQueue::AddBillboard(material, position, size, sinCos, color);
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const NzColor> colorPtr)
{
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	if (material->HasDepthMaterial())
		material = material->GetDepthMaterial();
	else
		material = m_baseMaterial;

	NzForwardRenderQueue::AddBillboards(material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr)
{
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	if (material->HasDepthMaterial())
		material = material->GetDepthMaterial();
	else
		material = m_baseMaterial;

	NzForwardRenderQueue::AddBillboards(material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr)
{
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	if (material->HasDepthMaterial())
		material = material->GetDepthMaterial();
	else
		material = m_baseMaterial;

	NzForwardRenderQueue::AddBillboards(material, count, positionPtr, sizePtr, anglePtr, colorPtr);
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr)
{
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	if (material->HasDepthMaterial())
		material = material->GetDepthMaterial();
	else
		material = m_baseMaterial;

	NzForwardRenderQueue::AddBillboards(material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const NzColor> colorPtr)
{
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	if (material->HasDepthMaterial())
		material = material->GetDepthMaterial();
	else
		material = m_baseMaterial;

	NzForwardRenderQueue::AddBillboards(material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr)
{
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	if (material->HasDepthMaterial())
		material = material->GetDepthMaterial();
	else
		material = m_baseMaterial;

	NzForwardRenderQueue::AddBillboards(material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr)
{
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	if (material->HasDepthMaterial())
		material = material->GetDepthMaterial();
	else
		material = m_baseMaterial;

	NzForwardRenderQueue::AddBillboards(material, count, positionPtr, sizePtr, anglePtr, colorPtr);
}

void NzDepthRenderQueue::AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr)
{
	NazaraAssert(material, "Invalid material");

	if (!IsMaterialSuitable(material))
		return;

	if (material->HasDepthMaterial())
		material = material->GetDepthMaterial();
	else
		material = m_baseMaterial;

	NzForwardRenderQueue::AddBillboards(material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
}

void NzDepthRenderQueue::AddDirectionalLight(const DirectionalLight& light)
{
	NazaraAssert(false, "Depth render queue doesn't handle lights");
	NazaraUnused(light);
}

void NzDepthRenderQueue::AddMesh(const NzMaterial* material, const NzMeshData& meshData, const NzBoxf& meshAABB, const NzMatrix4f& transformMatrix)
{
	NazaraAssert(material, "Invalid material");
	NazaraUnused(meshAABB);

	if (!IsMaterialSuitable(material))
		return;

	if (material->HasDepthMaterial())
		material = material->GetDepthMaterial();
	else
		material = m_baseMaterial;

	NzForwardRenderQueue::AddMesh(material, meshData, meshAABB, transformMatrix);
}

void NzDepthRenderQueue::AddPointLight(const PointLight& light)
{
	NazaraAssert(false, "Depth render queue doesn't handle lights");
	NazaraUnused(light);
}

void NzDepthRenderQueue::AddSpotLight(const SpotLight& light)
{
	NazaraAssert(false, "Depth render queue doesn't handle lights");
	NazaraUnused(light);
}

void NzDepthRenderQueue::AddSprites(const NzMaterial* material, const NzVertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const NzTexture* overlay)
{
	NazaraAssert(material, "Invalid material");
	NazaraUnused(overlay);

	if (!IsMaterialSuitable(material))
		return;

	if (material->HasDepthMaterial())
		material = material->GetDepthMaterial();
	else
		material = m_baseMaterial;

	NzForwardRenderQueue::AddSprites(material, vertices, spriteCount, overlay);
}

bool NzDepthRenderQueue::IsMaterialSuitable(const NzMaterial* material) const
{
	NazaraAssert(material, "Invalid material");

	return material->HasDepthMaterial() || (material->IsEnabled(nzRendererParameter_DepthBuffer) && material->IsEnabled(nzRendererParameter_DepthWrite) && material->IsShadowCastingEnabled());
}
