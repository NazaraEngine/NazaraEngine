// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PROPERTYHANDLER_TEXTUREPROPERTYHANDLER_HPP
#define NAZARA_GRAPHICS_PROPERTYHANDLER_TEXTUREPROPERTYHANDLER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/PropertyHandler/PropertyHandler.hpp>
#include <string>

namespace Nz
{
	class NAZARA_GRAPHICS_API TexturePropertyHandler : public PropertyHandler
	{
		public:
			inline TexturePropertyHandler(std::string propertyName);
			inline TexturePropertyHandler(std::string propertyName, std::string optionName);
			inline TexturePropertyHandler(std::string propertyName, std::string samplerTag, std::string optionName);
			TexturePropertyHandler(const TexturePropertyHandler&) = delete;
			TexturePropertyHandler(TexturePropertyHandler&&) = delete;
			~TexturePropertyHandler() = default;

			bool NeedsUpdateOnTextureUpdate(std::size_t updatedPropertyIndex) const override;

			void Setup(const Material& material, const ShaderReflection& reflection) override;

			void Update(MaterialInstance& materialInstance) const override;

			TexturePropertyHandler& operator=(const TexturePropertyHandler&) = delete;
			TexturePropertyHandler& operator=(TexturePropertyHandler&&) = delete;

		private:
			std::size_t m_propertyIndex;
			std::size_t m_textureIndex;
			std::string m_optionName;
			std::string m_propertyName;
			std::string m_samplerTag;
			UInt32 m_optionHash;
	};
}

#include <Nazara/Graphics/PropertyHandler/TexturePropertyHandler.inl>

#endif // NAZARA_GRAPHICS_PROPERTYHANDLER_TEXTUREPROPERTYHANDLER_HPP
