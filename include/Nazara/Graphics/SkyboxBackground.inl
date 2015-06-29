// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

inline const NzTextureRef& NzSkyboxBackground::GetTexture() const
{
	return m_texture;
}

inline NzTextureSampler& NzSkyboxBackground::GetTextureSampler()
{
	return m_sampler;
}

inline const NzTextureSampler& NzSkyboxBackground::GetTextureSampler() const
{
	return m_sampler;
}

inline void NzSkyboxBackground::SetTexture(NzTextureRef cubemapTexture)
{
	NazaraAssert(!cubemapTexture || cubemapTexture->IsValid(), "Invalid texture");
	NazaraAssert(!cubemapTexture || cubemapTexture->IsCubemap(), "Texture must be a cubemap");

	m_texture = std::move(cubemapTexture);
}

void NzSkyboxBackground::SetTextureSampler(const NzTextureSampler& sampler)
{
	m_sampler = sampler;
}

template<typename... Args>
NzSkyboxBackgroundRef NzSkyboxBackground::New(Args&&... args)
{
	std::unique_ptr<NzSkyboxBackground> object(new NzSkyboxBackground(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

#include <Nazara/Graphics/DebugOff.hpp>
