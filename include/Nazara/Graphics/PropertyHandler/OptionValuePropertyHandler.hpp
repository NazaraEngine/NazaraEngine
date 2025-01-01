// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PROPERTYHANDLER_OPTIONVALUEPROPERTYHANDLER_HPP
#define NAZARA_GRAPHICS_PROPERTYHANDLER_OPTIONVALUEPROPERTYHANDLER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/PropertyHandler/PropertyHandler.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API OptionValuePropertyHandler : public PropertyHandler
	{
		public:
			inline OptionValuePropertyHandler(std::string propertyName, std::string optionName);
			OptionValuePropertyHandler(const OptionValuePropertyHandler&) = default;
			OptionValuePropertyHandler(OptionValuePropertyHandler&&) = delete;
			~OptionValuePropertyHandler() = default;

			bool NeedsUpdateOnValueUpdate(std::size_t updatedPropertyIndex) const override;

			void Setup(const Material& material, const ShaderReflection& reflection) override;

			void Update(MaterialInstance& materialInstance) const override;

			OptionValuePropertyHandler& operator=(const OptionValuePropertyHandler&) = delete;
			OptionValuePropertyHandler& operator=(OptionValuePropertyHandler&&) = delete;

		private:
			std::size_t m_propertyIndex;
			std::string m_propertyName;
			std::string m_optionName;
			UInt32 m_optionHash;
	};
}

#include <Nazara/Graphics/PropertyHandler/OptionValuePropertyHandler.inl>

#endif // NAZARA_GRAPHICS_PROPERTYHANDLER_OPTIONVALUEPROPERTYHANDLER_HPP
