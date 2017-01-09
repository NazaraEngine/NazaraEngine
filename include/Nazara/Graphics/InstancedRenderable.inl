// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a InstancedRenderable object by default
	*/
	inline InstancedRenderable::InstancedRenderable() :
	m_boundingVolumeUpdated(false)
	{
	}

	/*!
	* \brief Constructs a InstancedRenderable object by assignation
	*
	* \param renderable InstancedRenderable to copy into this
	*/
	inline InstancedRenderable::InstancedRenderable(const InstancedRenderable& renderable) :
	RefCounted(),
	m_boundingVolume(renderable.m_boundingVolume),
	m_matCount(renderable.m_matCount),
	m_skin(renderable.m_skin),
	m_skinCount(renderable.m_skinCount),
	m_materials(renderable.m_materials),
	m_boundingVolumeUpdated(renderable.m_boundingVolumeUpdated)
	{
	}

	/*!
	* \brief Ensures that the bounding volume is up to date
	*/

	inline void InstancedRenderable::EnsureBoundingVolumeUpdated() const
	{
		if (!m_boundingVolumeUpdated)
			UpdateBoundingVolume();
	}

	/*!
	* \brief Gets one of the material used by the object.
	* \return A reference to the material.
	*
	* This function returns the active material at the specified index, depending on the current active skin.
	*
	* \param matIndex Material index to query
	*
	* \see GetSkin, GetMaterialCount, SetSkin
	*/
	inline const MaterialRef& InstancedRenderable::GetMaterial(std::size_t matIndex) const
	{
		return GetMaterial(m_skin, matIndex);
	}

	/*!
	* \brief Gets one of the material used by the object, independently from the active skin.
	* \return A reference to the material.
	*
	* This function returns the active material at the specified index and the specified skin index.
	* This function is the only way to query a material independently from the active skin.
	*
	* \param skinIndex Skin index to query
	* \param matIndex Material index to query
	*
	* \see GetSkinCount, GetMaterialCount, SetSkin
	*/
	inline const MaterialRef& InstancedRenderable::GetMaterial(std::size_t skinIndex, std::size_t matIndex) const
	{
		NazaraAssert(skinIndex < m_skinCount, "Skin index out of bounds");
		NazaraAssert(matIndex < m_materials.size(), "Material index out of bounds");

		return m_materials[m_matCount * skinIndex + matIndex];
	}

	/*!
	* \brief Gets the number of material per skin.
	* \return The current material count per skin
	*
	* This function returns how many different materials entries exists per skin
	* and is independent from the number of skin.
	*/
	inline std::size_t InstancedRenderable::GetMaterialCount() const
	{
		return m_matCount;
	}

	/*!
	* \brief Gets the current active skin index
	* \return Current skin index
	*
	* \see SetSkin
	*/
	inline std::size_t InstancedRenderable::GetSkin() const
	{
		return m_skin;
	}

	/*!
	* \brief Gets the number of skins this object has
	* \return Skin count
	*
	* \see GetSkin, SetSkinCount
	*/
	inline std::size_t InstancedRenderable::GetSkinCount() const
	{
		return m_skinCount;
	}

	/*!
	* \brief Changes the active skin
	*
	* Each InstancedRenderable has the possibility to have multiples skins, which are sets of materials.
	* Using this function allows you to have an object reference multiple materials, while using only some of thems (depending on the type of the object, see GetMaterialCount).
	*
	* \param skinIndex Skin index to change to
	*
	* \see SetSkinCount
	*/
	inline void InstancedRenderable::SetSkin(std::size_t skinIndex)
	{
		NazaraAssert(skinIndex < m_skinCount, "Skin index out of bounds");

		m_skin = skinIndex;

		// Force render queue invalidation
		InvalidateInstanceData(0);
	}

	/*!
	* \brief Changes the maximum skin count of the object
	*
	* This functions allows the object to store up to skinCount skins, which can then be switched to using SetSkin.
	* Please note that the possibly new skins will be set to the default material, which should not be changed.
	*
	* \param skinCount Skin index to change to
	*
	* \see SetSkin
	*/
	inline void InstancedRenderable::SetSkinCount(std::size_t skinCount)
	{
		m_materials.resize(m_matCount * skinCount, Material::GetDefault());
		m_skinCount = skinCount;
	}

	/*!
	* \brief Invalidates the bounding volume
	*/

	inline void InstancedRenderable::InvalidateBoundingVolume()
	{
		m_boundingVolumeUpdated = false;

		OnInstancedRenderableInvalidateBoundingVolume(this);
	}

	/*!
	* \brief Invalidates the instance data based on flags
	*
	* \param flags Flags to invalidate
	*/

	inline void InstancedRenderable::InvalidateInstanceData(UInt32 flags)
	{
		OnInstancedRenderableInvalidateData(this, flags);
	}

	/*!
	* \brief Resets the materials, material count and skin count
	*
	* This function clears the materials in use by the InstancedRenderable and resets its material count per skin along with its skin count.
	* This is the only way of setting the material count per skin and should be called at least by the constructor of the derived class.
	* Please note that all materials will be set to the default material, which should not be changed.
	*
	* This function also resets the current skin to the first one.
	*
	* \param matCount The new material count per skin value, must be at least 1
	* \param skinCount The new skin count value
	*
	* \see GetMaterial, GetMaterialCount, GetSkinCount, SetSkinCount
	*/
	inline void InstancedRenderable::ResetMaterials(std::size_t matCount, std::size_t skinCount)
	{
		NazaraAssert(skinCount != 0, "Invalid skin count (cannot be zero)");

		m_materials.clear();
		m_materials.resize(matCount * skinCount, Material::GetDefault());

		m_matCount = matCount;
		m_skinCount = skinCount;
		m_skin = 0;
	}

	/*!
	* \brief Changes the material used at the specified index by another one
	*
	* This function changes the active material at the specified index, depending on the current active skin, to the one passed as parameter.
	*
	* \param matIndex Material index
	* \param material New material, cannot be null
	*
	* \remark If you wish to reset the material to the default one, use the default material (see Material::GetDefault)
	*
	* \see SetMaterial
	*/
	inline void InstancedRenderable::SetMaterial(std::size_t matIndex, MaterialRef material)
	{
		SetMaterial(m_skin, matIndex, std::move(material));
	}

	/*!
	* \brief Changes the material used at the specified index by another one, independently from the active skin.
	*
	* This function changes the active material at the specified index and for the specified skin index, to the one passed as parameter.
	*
	* \param skinIndex Skin index
	* \param matIndex Material index
	* \param material New material, cannot be null
	*
	* \remark If you wish to reset the material to the default one, use the default material (see Material::GetDefault)
	*
	* \see SetMaterial
	*/
	inline void InstancedRenderable::SetMaterial(std::size_t skinIndex, std::size_t matIndex, MaterialRef material)
	{
		NazaraAssert(skinIndex < m_skinCount, "Skin index out of bounds");
		NazaraAssert(matIndex < m_materials.size(), "Material index out of bounds");
		NazaraAssert(material.IsValid(), "Material must be valid");

		MaterialRef& matEntry = m_materials[m_matCount * skinIndex + matIndex];
		if (matEntry != material)
		{
			OnInstancedRenderableInvalidateMaterial(this, skinIndex, matIndex, material);

			matEntry = std::move(material);
		}
	}

	/*!
	* \brief Sets the current instanced renderable with the content of the other one
	* \return A reference to this
	*
	* \param renderable The other InstancedRenderable
	*/

	inline InstancedRenderable& InstancedRenderable::operator=(const InstancedRenderable& renderable)
	{
		m_boundingVolume = renderable.m_boundingVolume;
		m_boundingVolumeUpdated = renderable.m_boundingVolumeUpdated;
		m_matCount = renderable.m_matCount;
		m_materials = renderable.m_materials;
		m_skin = renderable.m_skin;
		m_skinCount = renderable.m_skinCount;

		return *this;
	}

	/*!
	* \brief Updates the bounding volume
	*/

	inline void InstancedRenderable::UpdateBoundingVolume() const
	{
		MakeBoundingVolume();

		m_boundingVolumeUpdated = true;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
