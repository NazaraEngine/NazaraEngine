// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PREDEFINEDMATERIALS_HPP
#define NAZARA_GRAPHICS_PREDEFINEDMATERIALS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>

namespace Nz
{
	class MaterialSettings;

	class NAZARA_GRAPHICS_API PredefinedMaterials
	{
		public:
			PredefinedMaterials() = delete;
			~PredefinedMaterials() = delete;

			static void AddBasicSettings(MaterialSettings& settings);
			static void AddPbrSettings(MaterialSettings& settings);
			static void AddPhongSettings(MaterialSettings& settings);
	};
}

#include <Nazara/Graphics/PredefinedMaterials.inl>

#endif // NAZARA_GRAPHICS_PREDEFINEDMATERIALS_HPP
