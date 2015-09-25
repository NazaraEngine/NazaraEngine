// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
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
