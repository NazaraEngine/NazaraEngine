// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PREDEFINEDSHADERSTRUCTS_HPP
#define NAZARA_PREDEFINEDSHADERSTRUCTS_HPP

#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <array>

namespace Nz
{
	struct NAZARA_GRAPHICS_API PredefinedLightData
	{
		struct InnerStruct
		{
			std::size_t type;
			std::size_t color;
			std::size_t factor;
			std::size_t parameter1;
			std::size_t parameter2;
			std::size_t parameter3;
			std::size_t shadowMappingFlag;
			std::size_t totalSize;
		};

		InnerStruct innerOffsets;
		std::array<std::size_t, 3> lightArray;
		std::size_t lightArraySize;

		static PredefinedLightData GetOffset();
		static MaterialSettings::SharedUniformBlock GetUniformBlock();
	};

	struct NAZARA_GRAPHICS_API PredefinedInstanceData
	{
		std::size_t invWorldMatrixOffset;
		std::size_t totalSize;
		std::size_t worldMatrixOffset;

		static PredefinedInstanceData GetOffsets();
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

#endif // NAZARA_PREDEFINEDSHADERSTRUCTS_HPP
