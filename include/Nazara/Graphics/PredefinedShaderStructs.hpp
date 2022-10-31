// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTS_HPP
#define NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTS_HPP

#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <array>

namespace Nz
{
	struct NAZARA_GRAPHICS_API PredefinedLightData
	{
		struct Light
		{
			std::size_t type;
			std::size_t color;
			std::size_t factor;
			std::size_t parameter1;
			std::size_t parameter2;
			std::size_t parameter3;
			std::size_t shadowMappingFlag;
		};

		std::size_t lightsOffset;
		std::size_t lightCountOffset;
		std::size_t lightSize;
		std::size_t totalSize;
		Light lightMemberOffsets;

		static constexpr std::size_t MaxLightCount = 3;

		static PredefinedLightData GetOffsets();
	};

	struct NAZARA_GRAPHICS_API PredefinedInstanceData
	{
		std::size_t invWorldMatrixOffset;
		std::size_t totalSize;
		std::size_t worldMatrixOffset;

		static PredefinedInstanceData GetOffsets();
	};

	struct NAZARA_GRAPHICS_API PredefinedSkeletalData
	{
		std::size_t totalSize;
		std::size_t jointMatricesOffset;

		static constexpr std::size_t MaxMatricesCount = 256;

		static PredefinedSkeletalData GetOffsets();
	};

	struct NAZARA_GRAPHICS_API PredefinedViewerData
	{
		std::size_t eyePositionOffset;
		std::size_t invProjMatrixOffset;
		std::size_t invTargetSizeOffset;
		std::size_t invViewMatrixOffset;
		std::size_t invViewProjMatrixOffset;
		std::size_t projMatrixOffset;
		std::size_t targetSizeOffset;
		std::size_t totalSize;
		std::size_t viewMatrixOffset;
		std::size_t viewProjMatrixOffset;

		static PredefinedViewerData GetOffsets();
	};
}

#include <Nazara/Graphics/PredefinedShaderStructs.inl>

#endif // NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTS_HPP
