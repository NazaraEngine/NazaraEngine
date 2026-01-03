// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PROPERTYHANDLER_BUFFERPROPERTYHANDLER_HPP
#define NAZARA_GRAPHICS_PROPERTYHANDLER_BUFFERPROPERTYHANDLER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/PropertyHandler/PropertyHandler.hpp>
#include <string>

namespace Nz
{
	class NAZARA_GRAPHICS_API BufferPropertyHandler : public PropertyHandler
	{
		public:
			inline BufferPropertyHandler(std::string propertyName);
			inline BufferPropertyHandler(std::string propertyName, std::string optionName);
			inline BufferPropertyHandler(std::string propertyName, std::string bufferTag, std::string optionName);
			BufferPropertyHandler(const BufferPropertyHandler&) = delete;
			BufferPropertyHandler(BufferPropertyHandler&&) = delete;
			~BufferPropertyHandler() = default;

			bool NeedsUpdateOnStorageBufferUpdate(std::size_t updatedPropertyIndex) const override;

			void Setup(const Material& material, const ShaderReflection& reflection) override;

			void Update(MaterialInstance& materialInstance) const override;

			BufferPropertyHandler& operator=(const BufferPropertyHandler&) = delete;
			BufferPropertyHandler& operator=(BufferPropertyHandler&&) = delete;

		private:
			std::size_t m_propertyIndex;
			std::size_t m_storageBufferIndex;
			std::string m_bufferTag;
			std::string m_optionName;
			std::string m_propertyName;
			UInt32 m_optionHash;
	};
}

#include <Nazara/Graphics/PropertyHandler/BufferPropertyHandler.inl>

#endif // NAZARA_GRAPHICS_PROPERTYHANDLER_BUFFERPROPERTYHANDLER_HPP
