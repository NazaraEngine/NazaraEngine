// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Billboard.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::Billboard
	* \brief Graphics class that represents a billboard, a 2D surface which simulates a 3D object
	*/

	/*!
	* \brief Adds this billboard to the render queue
	*
	* \param renderQueue Queue to be added
	* \param instanceData Data used for instance
	*/

	void Billboard::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData, std::size_t /*instanceIndex*/, const Recti& scissorRect) const
	{
		Nz::Vector3f position = instanceData.transformMatrix.GetTranslation();
		renderQueue->AddBillboards(instanceData.renderOrder, GetMaterial(), 1, scissorRect, &position, &m_size, &m_sinCos, &m_color);
	}

	/*!
	* \brief Clones this billboard
	*/
	std::unique_ptr<InstancedRenderable> Billboard::Clone() const
	{
		return std::make_unique<Billboard>(*this);
	}

	/*!
	* \brief Sets the default material of the billboard (just default material)
	*/
	void Billboard::SetDefaultMaterial()
	{
		MaterialRef material = Material::New(BasicMaterial::GetSettings());
		material->EnableFaceCulling(true);

		SetMaterial(std::move(material));
	}

	/*!
	* \brief Sets the material of the billboard
	*
	* \param skinIndex Skin index to change
	* \param material Material for the billboard
	* \param resizeBillboard Should billboard be resized to the material size (diffuse map)
	*/
	void Billboard::SetMaterial(std::size_t skinIndex, MaterialRef material, bool resizeBillboard)
	{
		InstancedRenderable::SetMaterial(skinIndex, 0, std::move(material));

		if (resizeBillboard)
		{
			if (const MaterialRef& newMat = GetMaterial())
			{
				BasicMaterial phongMaterial(newMat);

				const Texture* diffuseMap = phongMaterial.GetDiffuseMap();
				if (diffuseMap && diffuseMap->IsValid())
					SetSize(Vector2f(Vector2ui(diffuseMap->GetSize())));
			}
		}
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
	void Billboard::SetTexture(std::size_t skinIndex, TextureRef texture, bool resizeBillboard)
	{
		if (resizeBillboard && texture && texture->IsValid())
			SetSize(Vector2f(Vector2ui(texture->GetSize())));

		const MaterialRef& material = GetMaterial(skinIndex);

		if (material->GetReferenceCount() > 1)
		{
			MaterialRef newMat = Material::New(*material); // Copy
			BasicMaterial phongMaterial(newMat);
			phongMaterial.SetDiffuseMap(std::move(texture));

			SetMaterial(skinIndex, std::move(newMat));
		}
		else
		{
			BasicMaterial phongMaterial(material);
			phongMaterial.SetDiffuseMap(std::move(texture));
		}
	}

	/*
	* \brief Makes the bounding volume of this billboard
	*/

	void Billboard::MakeBoundingVolume() const
	{
		// As billboard always face the screen, we must take its maximum size in account on every axis
		float maxSize = float(M_SQRT2) * std::max(m_size.x, m_size.y);

		Nz::Vector3f halfSize = (maxSize * Vector3f::Right() + maxSize * Vector3f::Down() + maxSize * Vector3f::Forward()) / 2.f;

		m_boundingVolume.Set(-halfSize, halfSize);
	}

	BillboardLibrary::LibraryMap Billboard::s_library;
}
