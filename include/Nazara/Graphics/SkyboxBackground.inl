// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Gets the movement offset
	* \return Offset of the movement
	*/

	inline const Vector3f& Nz::SkyboxBackground::GetMovementOffset() const
	{
		return m_movementOffset;
	}

	/*!
	* \brief Gets the movement scale
	* \return Scale of the movement
	*/

	inline float SkyboxBackground::GetMovementScale() const
	{
		return m_movementScale;
	}

	/*!
	* \brief Gets the texture of the background
	* \return Texture of the background
	*/

	inline const TextureRef& SkyboxBackground::GetTexture() const
	{
		return m_texture;
	}

	/*!
	* \brief Gets the texture sampler of the background
	* \return A reference to the texture sampler of the background
	*/

	inline TextureSampler& SkyboxBackground::GetTextureSampler()
	{
		return m_sampler;
	}

	/*!
	* \brief Gets the texture sampler of the background
	* \return A constant reference to the texture sampler of the background
	*/

	inline const TextureSampler& SkyboxBackground::GetTextureSampler() const
	{
		return m_sampler;
	}

	/*!
	* \brief Sets the movement offset
	*
	* \param offset Offset of the movement
	*/

	inline void SkyboxBackground::SetMovementOffset(const Vector3f& offset)
	{
		NazaraAssert(std::isfinite(offset.x) && std::isfinite(offset.y) && std::isfinite(offset.z), "Offset must be a finite vector");

		m_movementOffset = offset;
	}

	/*!
	* \brief Sets the movement scale
	*
	* \param scale Scale of the movement
	*/

	inline void SkyboxBackground::SetMovementScale(float scale)
	{
		NazaraAssert(std::isfinite(scale), "Scale must be a finite value");

		m_movementScale = scale;
	}

	/*!
	* \brief Sets the texture of the background
	*
	* \param cubemapTexture Texture of the background
	*/

	inline void SkyboxBackground::SetTexture(TextureRef cubemapTexture)
	{
		NazaraAssert(!cubemapTexture || cubemapTexture->IsValid(), "Invalid texture");
		NazaraAssert(!cubemapTexture || cubemapTexture->IsCubemap(), "Texture must be a cubemap");

		m_texture = std::move(cubemapTexture);
	}

	/*!
	* \brief Sets the texture sampler of the background
	*
	* \param sampler Texture sampler of the background
	*/

	void SkyboxBackground::SetTextureSampler(const TextureSampler& sampler)
	{
		m_sampler = sampler;
	}

	/*!
	* \brief Creates a new skybox background from the arguments
	* \return A reference to the newly created skybox background
	*
	* \param args Arguments for the skybox background
	*/

	template<typename... Args>
	SkyboxBackgroundRef SkyboxBackground::New(Args&&... args)
	{
		std::unique_ptr<SkyboxBackground> object(new SkyboxBackground(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
