// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_LIGHTSHADOWDATA_HPP
#define NAZARA_GRAPHICS_LIGHTSHADOWDATA_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class BakedFrameGraph;
	class FrameGraph;
	class FramePass;
	class MaterialInstance;
	class RenderFrame;
	class Texture;

	class NAZARA_GRAPHICS_API LightShadowData
	{
		public:
			LightShadowData() = default;
			LightShadowData(const LightShadowData&) = delete;
			LightShadowData(LightShadowData&&) = delete;
			virtual ~LightShadowData();

			virtual void PrepareRendering(RenderFrame& renderFrame) = 0;

			virtual void RegisterMaterialInstance(const MaterialInstance& matInstance) = 0;
			virtual void RegisterPassInputs(FramePass& pass) = 0;
			virtual void RegisterToFrameGraph(FrameGraph& frameGraph) = 0;

			virtual const Texture* RetrieveLightShadowmap(const BakedFrameGraph& bakedGraph) const = 0;

			virtual void UnregisterMaterialInstance(const MaterialInstance& matInstance) = 0;

			LightShadowData& operator=(const LightShadowData&) = delete;
			LightShadowData& operator=(LightShadowData&&) = delete;

		private:
	};
}

#include <Nazara/Graphics/LightShadowData.inl>

#endif // NAZARA_GRAPHICS_LIGHTSHADOWDATA_HPP
