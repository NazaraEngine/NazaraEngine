// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	DepthRenderQueue::DepthRenderQueue()
	{
		// Material
		m_baseMaterial = Material::New();
		m_baseMaterial->Enable(RendererParameter_ColorWrite, false);
		m_baseMaterial->Enable(RendererParameter_FaceCulling, false);
		//m_baseMaterial->SetFaceCulling(FaceSide_Front);
	}

	void DepthRenderQueue::AddBillboard(int renderOrder, const Material* material, const Vector3f& position, const Vector2f& size, const Vector2f& sinCos, const Color& color)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboard(0, material, position, size, sinCos, color);
	}

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
	}

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
	}

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, anglePtr, colorPtr);
	}

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
	}

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
	}

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
	}

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, anglePtr, colorPtr);
	}

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
	}

	void DepthRenderQueue::AddDirectionalLight(const DirectionalLight& light)
	{
		NazaraAssert(false, "Depth render queue doesn't handle lights");
		NazaraUnused(light);
	}

	void DepthRenderQueue::AddMesh(int renderOrder, const Material* material, const MeshData& meshData, const Boxf& meshAABB, const Matrix4f& transformMatrix)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);
		NazaraUnused(meshAABB);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddMesh(0, material, meshData, meshAABB, transformMatrix);
	}

	void DepthRenderQueue::AddPointLight(const PointLight& light)
	{
		NazaraAssert(false, "Depth render queue doesn't handle lights");
		NazaraUnused(light);
	}

	void DepthRenderQueue::AddSpotLight(const SpotLight& light)
	{
		NazaraAssert(false, "Depth render queue doesn't handle lights");
		NazaraUnused(light);
	}

	void DepthRenderQueue::AddSprites(int renderOrder, const Material* material, const VertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const Texture* overlay)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);
		NazaraUnused(overlay);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddSprites(0, material, vertices, spriteCount, overlay);
	}
}

