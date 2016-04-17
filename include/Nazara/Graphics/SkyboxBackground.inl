// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const Vector3f& Nz::SkyboxBackground::GetMovementOffset() const
	{
		return m_movementOffset;
	}

	inline float SkyboxBackground::GetMovementScale() const
	{
		return m_movementScale;
	}

	inline const TextureRef& SkyboxBackground::GetTexture() const
	{
		return m_texture;
	}

	inline TextureSampler& SkyboxBackground::GetTextureSampler()
	{
		return m_sampler;
	}

	inline const TextureSampler& SkyboxBackground::GetTextureSampler() const
	{
		return m_sampler;
	}

	inline void SkyboxBackground::SetMovementOffset(const Vector3f& offset)
	{
		NazaraAssert(std::isfinite(offset.x) && std::isfinite(offset.y) && std::isfinite(offset.z), "Offset must be a finite vector");

		m_movementOffset = offset;
	}

	inline void SkyboxBackground::SetMovementScale(float scale)
	{
		NazaraAssert(std::isfinite(scale), "Scale must be a finite value");

		m_movementScale = scale;
	}

	inline void SkyboxBackground::SetTexture(TextureRef cubemapTexture)
	{
		NazaraAssert(!cubemapTexture || cubemapTexture->IsValid(), "Invalid texture");
		NazaraAssert(!cubemapTexture || cubemapTexture->IsCubemap(), "Texture must be a cubemap");

		m_texture = std::move(cubemapTexture);
	}

	void SkyboxBackground::SetTextureSampler(const TextureSampler& sampler)
	{
		m_sampler = sampler;
	}

	template<typename... Args>
	SkyboxBackgroundRef SkyboxBackground::New(Args&&... args)
	{
		std::unique_ptr<SkyboxBackground> object(new SkyboxBackground(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
