// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

inline NzBillboard::NzBillboard()
{
	SetColor(NzColor::White);
	SetDefaultMaterial();
	SetRotation(0.f);
	SetSize(64.f, 64.f);
}

inline NzBillboard::NzBillboard(NzMaterialRef material)
{
	SetColor(NzColor::White);
	SetMaterial(std::move(material), true);
	SetRotation(0.f);
	SetSize(64.f, 64.f);
}

inline NzBillboard::NzBillboard(NzTexture* texture)
{
	SetColor(NzColor::White);
	SetRotation(0.f);
	SetSize(64.f, 64.f);
	SetTexture(texture, true);
}

inline NzBillboard::NzBillboard(const NzBillboard& billboard) :
NzInstancedRenderable(billboard),
m_color(billboard.m_color),
m_material(billboard.m_material),
m_rotation(billboard.m_rotation),
m_sinCos(billboard.m_sinCos),
m_size(billboard.m_size)
{
}

inline const NzColor& NzBillboard::GetColor() const
{
	return m_color;
}

inline const NzMaterialRef& NzBillboard::GetMaterial() const
{
	return m_material;
}

inline float NzBillboard::GetRotation() const
{
	return m_rotation;
}

inline const NzVector2f& NzBillboard::GetSize() const
{
	return m_size;
}

inline void NzBillboard::SetColor(const NzColor& color)
{
	m_color = color;
}

inline void NzBillboard::SetDefaultMaterial()
{
	NzMaterialRef material = NzMaterial::New();
	material->Enable(nzRendererParameter_FaceCulling, true);
	material->EnableLighting(false);

	SetMaterial(std::move(material));
}

inline void NzBillboard::SetMaterial(NzMaterialRef material, bool resizeBillboard)
{
	m_material = std::move(material);
	if (m_material && resizeBillboard)
	{
		NzTexture* diffuseMap = m_material->GetDiffuseMap();
		if (diffuseMap && diffuseMap->IsValid())
			SetSize(NzVector2f(NzVector2ui(diffuseMap->GetSize())));
	}
}

inline void NzBillboard::SetRotation(float rotation)
{
	m_rotation = rotation;
	m_sinCos.Set(std::sin(m_rotation), std::cos(m_rotation));
}

inline void NzBillboard::SetSize(const NzVector2f& size)
{
	m_size = size;

	// On invalide la bounding box
	InvalidateBoundingVolume();
}

inline void NzBillboard::SetSize(float sizeX, float sizeY)
{
	SetSize(NzVector2f(sizeX, sizeY));
}

inline void NzBillboard::SetTexture(NzTextureRef texture, bool resizeBillboard)
{
	if (!m_material)
		SetDefaultMaterial();
	else if (m_material->GetReferenceCount() > 1)
		m_material = NzMaterial::New(*m_material); // Copie

	if (resizeBillboard && texture && texture->IsValid())
		SetSize(NzVector2f(NzVector2ui(texture->GetSize())));

	m_material->SetDiffuseMap(std::move(texture));
}

inline NzBillboard& NzBillboard::operator=(const NzBillboard& billboard)
{
	NzInstancedRenderable::operator=(billboard);

	m_color = billboard.m_color;
	m_material = billboard.m_material;
	m_size = billboard.m_size;

	InvalidateBoundingVolume();

	return *this;
}

template<typename... Args>
NzBillboardRef NzBillboard::New(Args&&... args)
{
	std::unique_ptr<NzBillboard> object(new NzBillboard(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

#include <Nazara/Graphics/DebugOff.hpp>
