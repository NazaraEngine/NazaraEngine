// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Billboard.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a Billboard object by default
	*/

	inline Billboard::Billboard()
	{
		ResetMaterials(1);

		SetColor(Color::White);
		SetDefaultMaterial();
		SetRotation(0.f);
		SetSize(64.f, 64.f);
	}

	/*!
	* \brief Constructs a Billboard object with a reference to a material
	*
	* \param material Reference to a material
	*/

	inline Billboard::Billboard(MaterialRef material)
	{
		ResetMaterials(1);

		SetColor(Color::White);
		SetMaterial(std::move(material), true);
		SetRotation(0.f);
		SetSize(64.f, 64.f);
	}

	/*!
	* \brief Constructs a Billboard object with a pointer to a texture
	*
	* \param texture Pointer to a texture
	*/

	inline Billboard::Billboard(Texture* texture)
	{
		ResetMaterials(1);

		SetColor(Color::White);
		SetRotation(0.f);
		SetSize(64.f, 64.f);
		SetTexture(texture, true);
	}

	/*!
	* \brief Constructs a Billboard object by assignation
	*
	* \param billboard Billboard to copy into this
	*/

	inline Billboard::Billboard(const Billboard& billboard) :
	InstancedRenderable(billboard),
	m_color(billboard.m_color),
	m_sinCos(billboard.m_sinCos),
	m_size(billboard.m_size),
	m_rotation(billboard.m_rotation)
	{
	}

	/*!
	* \brief Gets the color of the billboard
	* \return Current color
	*/

	inline const Color& Billboard::GetColor() const
	{
		return m_color;
	}

	/*!
	* \brief Gets the rotation of the billboard
	* \return Current rotation
	*/

	inline float Billboard::GetRotation() const
	{
		return m_rotation;
	}

	/*!
	* \brief Gets the size of the billboard
	* \return Current size
	*/

	inline const Vector2f& Billboard::GetSize() const
	{
		return m_size;
	}

	/*!
	* \brief Sets the color of the billboard
	*
	* \param color Color for the billboard
	*/

	inline void Billboard::SetColor(const Color& color)
	{
		m_color = color;
	}

	/*!
	* \brief Sets the default material of the billboard (just default material)
	*/

	inline void Billboard::SetDefaultMaterial()
	{
		MaterialRef material = Material::New();
		material->EnableFaceCulling(true);

		SetMaterial(std::move(material));
	}

	/*!
	* \brief Sets the material of the billboard
	*
	* \param material Material for the billboard
	* \param resizeBillboard Should billboard be resized to the material size (diffuse map)
	*/
	inline void Billboard::SetMaterial(MaterialRef material, bool resizeBillboard)
	{
		SetMaterial(GetSkin(), std::move(material), resizeBillboard);
	}

	/*!
	* \brief Sets the material of the billboard
	*
	* \param skinIndex Skin index to change
	* \param material Material for the billboard
	* \param resizeBillboard Should billboard be resized to the material size (diffuse map)
	*/
	inline void Billboard::SetMaterial(std::size_t skinIndex, MaterialRef material, bool resizeBillboard)
	{
		InstancedRenderable::SetMaterial(skinIndex, 0, std::move(material));

		if (resizeBillboard)
		{
			if (const MaterialRef& newMat = GetMaterial())
			{
				const TextureRef& diffuseMap = newMat->GetDiffuseMap();
				if (diffuseMap && diffuseMap->IsValid())
					SetSize(Vector2f(Vector2ui(diffuseMap->GetSize())));
			}
		}
	}

	/*!
	* \brief Sets the rotation of the billboard
	*
	* \param rotation Rotation for the billboard
	*/

	inline void Billboard::SetRotation(float rotation)
	{
		m_rotation = rotation;
		m_sinCos.Set(std::sin(m_rotation), std::cos(m_rotation));
	}

	/*!
	* \brief Sets the size of the billboard
	*
	* \param size Size for the billboard
	*/

	inline void Billboard::SetSize(const Vector2f& size)
	{
		m_size = size;

		// We invalidate the bounding volume
		InvalidateBoundingVolume();
	}

	/*!
	* \brief Sets the size of the billboard
	*
	* \param sizeX Size in X for the billboard
	* \param sizeY Size in Y for the billboard
	*/

	inline void Billboard::SetSize(float sizeX, float sizeY)
	{
		SetSize(Vector2f(sizeX, sizeY));
	}

	/*!
	* \brief Sets the texture of the billboard
	*
	* \param texture Texture for the billboard
	* \param resizeBillboard Should billboard be resized to the texture size
	*/
	inline void Billboard::SetTexture(TextureRef texture, bool resizeBillboard)
	{
		SetTexture(GetSkin(), std::move(texture), resizeBillboard);
	}

	/*!
	* \brief Sets the texture of the billboard for a specific index
	*
	* This function changes the diffuse map of the material associated with the specified skin index
	*
	* \param skinIndex Skin index to change
	* \param texture Texture for the billboard
	* \param resizeBillboard Should billboard be resized to the texture size
	*/
	inline void Billboard::SetTexture(std::size_t skinIndex, TextureRef texture, bool resizeBillboard)
	{
		if (resizeBillboard && texture && texture->IsValid())
			SetSize(Vector2f(Vector2ui(texture->GetSize())));

		const MaterialRef& material = GetMaterial(skinIndex);

		if (material->GetReferenceCount() > 1)
		{
			MaterialRef newMat = Material::New(*material); // Copy
			newMat->SetDiffuseMap(std::move(texture));

			SetMaterial(skinIndex, std::move(newMat));
		}
		else
			material->SetDiffuseMap(std::move(texture));
	}

	/*!
	* \brief Sets the current billboard with the content of the other one
	* \return A reference to this
	*
	* \param billboard The other Billboard
	*/

	inline Billboard& Billboard::operator=(const Billboard& billboard)
	{
		InstancedRenderable::operator=(billboard);

		m_color = billboard.m_color;
		m_size = billboard.m_size;

		InvalidateBoundingVolume();

		return *this;
	}

	/*!
	* \brief Creates a new billboard from the arguments
	* \return A reference to the newly created billboard
	*
	* \param args Arguments for the billboard
	*/

	template<typename... Args>
	BillboardRef Billboard::New(Args&&... args)
	{
		std::unique_ptr<Billboard> object(new Billboard(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
