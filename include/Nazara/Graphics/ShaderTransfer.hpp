// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SHADERTRANSFER_HPP
#define NAZARA_GRAPHICS_SHADERTRANSFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>

namespace Nz
{
	class AbstractViewer;
	class DirectionalLight;
	class DirectionalLightShadowData;
	class PointLight;
	class SpotLight;

	namespace ShaderTransfer
	{
		NAZARA_GRAPHICS_API void WriteLight(const DirectionalLight* light, void* basePtr);
		NAZARA_GRAPHICS_API void WriteLight(const PointLight* light, void* basePtr);
		NAZARA_GRAPHICS_API void WriteLight(const SpotLight* light, void* basePtr);
		NAZARA_GRAPHICS_API void WriteLightShadowData(const AbstractViewer* viewer, const DirectionalLightShadowData* shadowData, void* basePtr);
	};
}

#include <Nazara/Graphics/ShaderTransfer.inl>

#endif // NAZARA_GRAPHICS_SHADERTRANSFER_HPP
