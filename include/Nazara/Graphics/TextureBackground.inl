// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const TextureRef& TextureBackground::GetTexture() const
	{
		return m_texture;
	}

	inline void TextureBackground::SetTexture(TextureRef texture)
	{
		NazaraAssert(!texture || texture->IsValid(), "Invalid texture");

		m_texture = std::move(texture);
	}

	template<typename... Args>
	TextureBackgroundRef TextureBackground::New(Args&&... args)
	{
		std::unique_ptr<TextureBackground> object(new TextureBackground(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
