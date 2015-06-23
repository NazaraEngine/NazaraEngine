// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

inline const NzTextureRef& NzTextureBackground::GetTexture() const
{
	return m_texture;
}

inline void NzTextureBackground::SetTexture(NzTextureRef texture)
{
	NazaraAssert(!texture || texture->IsValid(), "Invalid texture");

	m_texture = std::move(texture);
}

template<typename... Args>
NzTextureBackgroundRef NzTextureBackground::New(Args&&... args)
{
	std::unique_ptr<NzTextureBackground> object(new NzTextureBackground(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

#include <Nazara/Graphics/DebugOff.hpp>
