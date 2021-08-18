// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERERIMPL_HPP
#define NAZARA_RENDERER_RENDERERIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderDeviceInfo.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class Buffer;
	class RendererImpl;
	class RenderDevice;
	class RenderSurface;
	class RenderWindow;
	class RenderWindowImpl;

	using CreateRendererImplFunc = RendererImpl*(*)();

	class NAZARA_RENDERER_API RendererImpl
	{
		public:
			RendererImpl() = default;
			virtual ~RendererImpl();

			virtual std::unique_ptr<RenderSurface> CreateRenderSurfaceImpl() = 0;
			virtual std::unique_ptr<RenderWindowImpl> CreateRenderWindowImpl(RenderWindow& owner) = 0;

			virtual std::shared_ptr<RenderDevice> InstanciateRenderDevice(std::size_t deviceIndex, const RenderDeviceFeatures& enabledFeatures) = 0;

			virtual RenderAPI QueryAPI() const = 0;
			virtual std::string QueryAPIString() const = 0;
			virtual UInt32 QueryAPIVersion() const = 0;

			virtual const std::vector<RenderDeviceInfo>& QueryRenderDevices() const = 0;

			virtual bool Prepare(const ParameterList& parameters) = 0;
	};
}

#endif // NAZARA_RENDERER_RENDERERIMPL_HPP
