// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERERIMPL_HPP
#define NAZARA_RENDERER_RENDERERIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <vector>

namespace Nz
{
	class RendererImpl;

	using CreateRendererImplFunc = RendererImpl*(*)();

	class NAZARA_RENDERER_API RendererImpl
	{
		public:
			RendererImpl() = default;
			virtual ~RendererImpl();

			virtual bool IsBetterThan(const RendererImpl* other) const = 0;

			virtual RenderAPI QueryAPI() const = 0;
			virtual String QueryAPIString() const = 0;
			virtual UInt32 QueryAPIVersion() const = 0;

			virtual std::vector<RenderDevice> QueryRenderDevices() const = 0;

			virtual bool Prepare(const ParameterList& parameters) = 0;
	};
}

#endif // NAZARA_RENDERER_RENDERERIMPL_HPP
