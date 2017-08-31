// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONTEXTPARAMETERS_HPP
#define NAZARA_CONTEXTPARAMETERS_HPP

#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RenderTargetParameters.hpp>
#include <Nazara/Platform/VideoMode.hpp>
#include <Nazara/Platform/WindowHandle.hpp>

namespace Nz
{
	class Context;

	struct NAZARA_RENDERER_API ContextParameters
	{
		ContextParameters(const RenderTargetParameters& parameters = RenderTargetParameters()) :
		antialiasingLevel(parameters.antialiasingLevel),
		bitsPerPixel(VideoMode::GetDesktopMode().bitsPerPixel),
		depthBits(parameters.depthBits),
		majorVersion(defaultMajorVersion),
		minorVersion(defaultMinorVersion),
		stencilBits(parameters.stencilBits),
		shareContext(defaultShareContext),
		window(0),
		compatibilityProfile(defaultCompatibilityProfile),
		debugMode(defaultDebugMode),
		doubleBuffered(defaultDoubleBuffered),
		shared(defaultShared)
		{
		}

		UInt8 antialiasingLevel;
		UInt8 bitsPerPixel;
		UInt8 depthBits;
		UInt8 majorVersion;
		UInt8 minorVersion;
		UInt8 stencilBits;
		const Context* shareContext;
		WindowHandle window;
		bool compatibilityProfile;
		bool debugMode;
		bool doubleBuffered;
		bool shared;

		static UInt8 defaultMajorVersion;
		static UInt8 defaultMinorVersion;
		static const Context* defaultShareContext;
		static bool defaultCompatibilityProfile;
		static bool defaultDebugMode;
		static bool defaultDoubleBuffered;
		static bool defaultShared;
	};
}

#endif // NAZARA_CONTEXTPARAMETERS_HPP
