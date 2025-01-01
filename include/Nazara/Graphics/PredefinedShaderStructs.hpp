// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTS_HPP
#define NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTS_HPP

#include <Nazara/Graphics/PredefinedShaderStructBuilder.hpp>

namespace Nz
{
	static constexpr PredefinedDirectionalLightData PredefinedDirectionalLightOffsets = PredefinedDirectionalLightData::Build();
	static constexpr PredefinedPointLightData PredefinedPointLightOffsets = PredefinedPointLightData::Build();
	static constexpr PredefinedSpotLightData PredefinedSpotLightOffsets = PredefinedSpotLightData::Build();
	static constexpr PredefinedLightData PredefinedLightOffsets = PredefinedLightData::Build();
	static constexpr PredefinedInstanceData PredefinedInstanceOffsets = PredefinedInstanceData::Build();
	static constexpr PredefinedSkeletalData PredefinedSkeletalOffsets = PredefinedSkeletalData::Build();
	static constexpr PredefinedViewerData PredefinedViewerOffsets = PredefinedViewerData::Build();
}

#endif // NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTS_HPP
