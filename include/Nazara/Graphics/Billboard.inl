// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline Billboard::Billboard()
	{
		SetColor(Color::White);
		SetDefaultMaterial();
		SetRotation(0.f);
		SetSize(64.f, 64.f);
	}

	inline Billboard::Billboard(MaterialRef material)
	{
		SetColor(Color::White);
		SetMaterial(std::move(material), true);
		SetRotation(0.f);
		SetSize(64.f, 64.f);
	}

	inline Billboard::Billboard(Texture* texture)
	{
		SetColor(Color::White);
		SetRotation(0.f);
		SetSize(64.f, 64.f);
		SetTexture(texture, true);
	}

	inline Billboard::Billboard(const Billboard& billboard) :
	InstancedRenderable(billboard),
	m_color(billboard.m_color),
	m_material(billboard.m_material),
	m_rotation(billboard.m_rotation),
	m_sinCos(billboard.m_sinCos),
	m_size(billboard.m_size)
	{
	}

	inline const Color& Billboard::GetColor() const
	{
		return m_color;
	}

	inline const MaterialRef& Billboard::GetMaterial() const
	{
		return m_material;
	}

	inline float Billboard::GetRotation() const
	{
		return m_rotation;
	}

	inline const Vector2f& Billboard::GetSize() const
	{
		return m_size;
	}

	inline void Billboard::SetColor(const Color& color)
	{
		m_color = color;
	}

	inline void Billboard::SetDefaultMaterial()
	{
		MaterialRef material = Material::New();
		material->Enable(RendererParameter_FaceCulling, true);
		material->EnableLighting(false);

		SetMaterial(std::move(material));
	}

	inline void Billboard::SetMaterial(MaterialRef material, bool resizeBillboard)
	{
		m_material = std::move(material);
		if (m_material && resizeBillboard)
		{
			Texture* diffuseMap = m_material->GetDiffuseMap();
			if (diffuseMap && diffuseMap->IsValid())
				SetSize(Vector2f(Vector2ui(diffuseMap->GetSize())));
		}
	}

	inline void Billboard::SetRotation(float rotation)
	{
		m_rotation = rotation;
		m_sinCos.Set(std::sin(m_rotation), std::cos(m_rotation));
	}

	inline void Billboard::SetSize(const Vector2f& size)
	{
		m_size = size;

		// On invalide la bounding box
		InvalidateBoundingVolume();
	}

	inline void Billboard::SetSize(float sizeX, float sizeY)
	{
		SetSize(Vector2f(sizeX, sizeY));
	}

	inline void Billboard::SetTexture(TextureRef texture, bool resizeBillboard)
	{
		if (!m_material)
			SetDefaultMaterial();
		else if (m_material->GetReferenceCount() > 1)
			m_material = Material::New(*m_material); // Copie

		if (resizeBillboard && texture && texture->IsValid())
			SetSize(Vector2f(Vector2ui(texture->GetSize())));

		m_material->SetDiffuseMap(std::move(texture));
	}

	inline Billboard& Billboard::operator=(const Billboard& billboard)
	{
		InstancedRenderable::operator=(billboard);

		m_color = billboard.m_color;
		m_material = billboard.m_material;
		m_size = billboard.m_size;

		InvalidateBoundingVolume();

		return *this;
	}

	template<typename... Args>
	BillboardRef Billboard::New(Args&&... args)
	{
		std::unique_ptr<Billboard> object(new Billboard(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
