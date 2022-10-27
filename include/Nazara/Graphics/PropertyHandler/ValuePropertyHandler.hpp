// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PROPERTYHANDLER_VALUEPROPERTYHANDLER_HPP
#define NAZARA_GRAPHICS_PROPERTYHANDLER_VALUEPROPERTYHANDLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/PropertyHandler/PropertyHandler.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API ValuePropertyHandler : public PropertyHandler
	{
		public:
			inline ValuePropertyHandler(std::string propertyName, std::string blockTag = "Settings");
			inline ValuePropertyHandler(std::string propertyName, std::string memberTag, std::string blockTag = "Settings");
			ValuePropertyHandler(const ValuePropertyHandler&) = delete;
			ValuePropertyHandler(ValuePropertyHandler&&) = delete;
			~ValuePropertyHandler() = default;

			bool NeedsUpdateOnValueUpdate(std::size_t updatedPropertyIndex) const override;

			void Setup(const Material& material, const ShaderReflection& reflection) override;

			void Update(MaterialInstance& materialInstance) const override;

			ValuePropertyHandler& operator=(const ValuePropertyHandler&) = delete;
			ValuePropertyHandler& operator=(ValuePropertyHandler&&) = delete;
			
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

#include <Nazara/Graphics/PropertyHandler/ValuePropertyHandler.inl>

#endif // NAZARA_GRAPHICS_PROPERTYHANDLER_VALUEPROPERTYHANDLER_HPP
