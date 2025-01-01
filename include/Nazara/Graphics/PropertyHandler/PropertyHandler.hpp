// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PROPERTYHANDLER_PROPERTYHANDLER_HPP
#define NAZARA_GRAPHICS_PROPERTYHANDLER_PROPERTYHANDLER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <NZSL/Enums.hpp>

namespace Nz
{
	class Material;
	class MaterialInstance;
	class ShaderReflection;

	class NAZARA_GRAPHICS_API PropertyHandler
	{
		public:
			PropertyHandler() = default;
			PropertyHandler(const PropertyHandler&) = default;
			PropertyHandler(PropertyHandler&&) = delete;
			virtual ~PropertyHandler();

			virtual bool NeedsUpdateOnStorageBufferUpdate(std::size_t updatedStorageBufferPropertyIndex) const;
			virtual bool NeedsUpdateOnTextureUpdate(std::size_t updatedTexturePropertyIndex) const;
			virtual bool NeedsUpdateOnValueUpdate(std::size_t updatedValuePropertyIndex) const;

			virtual void Setup(const Material& material, const ShaderReflection& reflection) = 0;

			virtual void Update(MaterialInstance& materialInstance) const = 0;

			PropertyHandler& operator=(const PropertyHandler&) = default;
			PropertyHandler& operator=(PropertyHandler&&) = delete;

			struct SamplerData
			{
				std::size_t textureIndex;
			};

			struct UniformValue
			{
				std::size_t uniformBufferIndex;
				std::size_t offset;
			};
	};
}

#include <Nazara/Graphics/PropertyHandler/PropertyHandler.inl>

#endif // NAZARA_GRAPHICS_PROPERTYHANDLER_PROPERTYHANDLER_HPP
