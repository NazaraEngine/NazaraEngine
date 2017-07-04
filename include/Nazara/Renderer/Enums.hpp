// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_RENDERER_HPP
#define NAZARA_ENUMS_RENDERER_HPP

namespace Nz
{
	enum RenderAPI
	{
		RenderAPI_Direct3D, ///< Microsoft Render API, only works on MS platforms
		RenderAPI_Mantle,   ///< AMD Render API, Vulkan predecessor, only works on AMD GPUs
		RenderAPI_Metal,    ///< Apple Render API, only works on OS X platforms
		RenderAPI_OpenGL,   ///< Khronos Render API, works on Web/Desktop/Mobile and some consoles
		RenderAPI_Vulkan,   ///< New Khronos Render API, made to replace OpenGL, works on desktop (Windows/Linux) and mobile (Android)

		RenderAPI_Other,    ///< RenderAPI not corresponding to an entry of the enum, or result of a failed query

		RenderAPI_Max = RenderAPI_Other
	};

	enum RenderDeviceType
	{
		RenderDeviceType_Integrated, ///< Hardware-accelerated chipset integrated to a CPU (ex: Intel Graphics HD 4000)
		RenderDeviceType_Dedicated,  ///< Hardware-accelerated GPU (ex: AMD R9 390)
		RenderDeviceType_Software,   ///< Software-renderer
		RenderDeviceType_Virtual,    ///< Proxy renderer relaying instructions to another unknown device

		RenderDeviceType_Unknown,    ///< Device type not corresponding to an entry of the enum, or result of a failed query

		RenderDeviceType_Max = RenderDeviceType_Unknown
	};
}

#endif // NAZARA_ENUMS_RENDERER_HPP
