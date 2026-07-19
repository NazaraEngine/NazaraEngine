// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERERIMPL_HPP
#define NAZARA_RENDERER_RENDERERIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <Nazara/Renderer/GpuDeviceInfo.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class Buffer;
	class RendererImpl;
	class GpuDevice;
	class RenderSurface;
	class WindowSwapchain;
	class Swapchain;

	using CreateRendererImplFunc = RendererImpl*(*)();

	class NAZARA_RENDERER_API RendererImpl
	{
		public:
			RendererImpl() = default;
			virtual ~RendererImpl();

			virtual std::shared_ptr<GpuDevice> InstanciateGpuDevice(std::size_t deviceIndex, const GpuDeviceFeatures& enabledFeatures) = 0;

			virtual GpuBackend QueryAPI() const = 0;
			virtual std::string QueryAPIString() const = 0;
			virtual UInt32 QueryAPIVersion() const = 0;

			virtual const std::vector<GpuDeviceInfo>& QueryGpuDevices() const = 0;

			virtual bool Prepare(const Renderer::Config& config) = 0;
	};
}

#endif // NAZARA_RENDERER_RENDERERIMPL_HPP
