// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Gets the texture of the background
	* \return Texture of the background
	*/

	inline const TextureRef& TextureBackground::GetTexture() const
	{
		return m_texture;
	}

	/*!
	* \brief Sets the texture of the background
	*
	* \param texture Texture of the background
	*/

	inline void TextureBackground::SetTexture(TextureRef texture)
	{
		NazaraAssert(!texture || texture->IsValid(), "Invalid texture");

		m_texture = std::move(texture);
	}

	/*!
	* \brief Creates a new texture background from the arguments
	* \return A reference to the newly created texture background
	*
	* \param args Arguments for the texture background
	*/

	template<typename... Args>
	TextureBackgroundRef TextureBackground::New(Args&&... args)
	{
		std::unique_ptr<TextureBackground> object(new TextureBackground(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
