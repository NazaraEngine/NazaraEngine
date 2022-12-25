// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERDEVICEINFO_HPP
#define NAZARA_RENDERER_RENDERDEVICEINFO_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <string>

namespace Nz
{
	struct RenderDeviceFeatures
	{
		bool anisotropicFiltering = false;
		bool computeShaders = false;
		bool depthClamping = false;
		bool nonSolidFaceFilling = false;
		bool storageBuffers = false;
		bool textureRead = false;
		bool textureReadWithoutFormat = false;
		bool textureWrite = false;
		bool textureWriteWithoutFormat = false;
		bool unrestrictedTextureViews = false;
	};

	struct RenderDeviceLimits
	{
		UInt64 maxComputeSharedMemorySize;
		UInt32 maxComputeWorkGroupInvocations;
		Vector3ui32 maxComputeWorkGroupCount;
		Vector3ui32 maxComputeWorkGroupSize;
		UInt64 maxStorageBufferSize;
		UInt64 maxUniformBufferSize;
		UInt64 minStorageBufferOffsetAlignment;
		UInt64 minUniformBufferOffsetAlignment;
	};

	struct RenderDeviceInfo
	{
		RenderDeviceFeatures features;
		RenderDeviceLimits limits;
		RenderDeviceType type;
		std::string name;
	};
}

#endif // NAZARA_RENDERER_RENDERDEVICEINFO_HPP
