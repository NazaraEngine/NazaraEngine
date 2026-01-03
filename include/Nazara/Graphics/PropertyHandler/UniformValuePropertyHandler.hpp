// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PROPERTYHANDLER_UNIFORMVALUEPROPERTYHANDLER_HPP
#define NAZARA_GRAPHICS_PROPERTYHANDLER_UNIFORMVALUEPROPERTYHANDLER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/PropertyHandler/PropertyHandler.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API UniformValuePropertyHandler : public PropertyHandler
	{
		public:
			inline UniformValuePropertyHandler(std::string propertyName, std::string blockTag = "Settings");
			inline UniformValuePropertyHandler(std::string propertyName, std::string memberTag, std::string blockTag = "Settings");
			UniformValuePropertyHandler(const UniformValuePropertyHandler&) = delete;
			UniformValuePropertyHandler(UniformValuePropertyHandler&&) = delete;
			~UniformValuePropertyHandler() = default;

			bool NeedsUpdateOnValueUpdate(std::size_t updatedPropertyIndex) const override;

			void Setup(const Material& material, const ShaderReflection& reflection) override;

			void Update(MaterialInstance& materialInstance) const override;

			UniformValuePropertyHandler& operator=(const UniformValuePropertyHandler&) = delete;
			UniformValuePropertyHandler& operator=(UniformValuePropertyHandler&&) = delete;

		private:
			std::size_t m_propertyIndex;
			std::size_t m_offset;
			std::size_t m_size;
			std::size_t m_uniformBlockIndex;
			std::string m_blockTag;
			std::string m_propertyName;
			std::string m_memberTag;
	};
}

#include <Nazara/Graphics/PropertyHandler/UniformValuePropertyHandler.inl>

#endif // NAZARA_GRAPHICS_PROPERTYHANDLER_UNIFORMVALUEPROPERTYHANDLER_HPP
