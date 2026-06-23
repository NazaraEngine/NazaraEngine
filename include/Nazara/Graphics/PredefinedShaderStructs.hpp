// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTS_HPP
#define NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTS_HPP

#include <Nazara/Graphics/PredefinedShaderStructBuilder.hpp>

namespace Nz
{
	static constexpr PredefinedDirectionalLightData PredefinedDirectionalLightOffsets = PredefinedDirectionalLightData::Build();
	static constexpr PredefinedDirectionalLightsData PredefinedDirectionalLightsOffsets = PredefinedDirectionalLightsData::Build();
	static constexpr PredefinedDirectionalShadowAtlasEntryData PredefinedDirectionalShadowAtlasEntryOffsets = PredefinedDirectionalShadowAtlasEntryData::Build();
	static constexpr PredefinedPointLightData PredefinedPointLightOffsets = PredefinedPointLightData::Build();
	static constexpr PredefinedPointLightsData PredefinedPointLightsOffsets = PredefinedPointLightsData::Build();
	static constexpr PredefinedPointShadowAtlasEntryData PredefinedPointShadowAtlasEntryOffsets = PredefinedPointShadowAtlasEntryData::Build();
	static constexpr PredefinedSpotLightData PredefinedSpotLightOffsets = PredefinedSpotLightData::Build();
	static constexpr PredefinedSpotLightsData PredefinedSpotLightsOffsets = PredefinedSpotLightsData::Build();
	static constexpr PredefinedSpotShadowAtlasEntryData PredefinedSpotShadowAtlasEntryOffsets = PredefinedSpotShadowAtlasEntryData::Build();
	static constexpr PredefinedInstanceData PredefinedInstanceOffsets = PredefinedInstanceData::Build();
	static constexpr PredefinedSkeletalData PredefinedSkeletalOffsets = PredefinedSkeletalData::Build();
	static constexpr PredefinedViewerData PredefinedViewerOffsets = PredefinedViewerData::Build();
	static constexpr ShadowAtlasEntry ShadowAtlasEntryOffsets = ShadowAtlasEntry::Build();
}

#endif // NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTS_HPP
