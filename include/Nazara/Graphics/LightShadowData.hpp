// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_LIGHTSHADOWDATA_HPP
#define NAZARA_GRAPHICS_LIGHTSHADOWDATA_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>

namespace Nz
{
	class AbstractViewer;
	class BakedFrameGraph;
	class FrameGraph;
	class FramePass;
	class MaterialInstance;
	class RenderResources;
	class Texture;

	class NAZARA_GRAPHICS_API LightShadowData
	{
		public:
			inline LightShadowData();
			LightShadowData(const LightShadowData&) = delete;
			LightShadowData(LightShadowData&&) = delete;
			virtual ~LightShadowData();

			inline bool IsPerViewer() const;

			virtual void PrepareRendering(RenderResources& renderResources, [[maybe_unused]] const AbstractViewer* viewer) = 0;

			virtual void RegisterMaterialInstance(const MaterialInstance& matInstance) = 0;
			virtual void RegisterPassInputs(FramePass& pass, const AbstractViewer* viewer) = 0;
			virtual void RegisterToFrameGraph(FrameGraph& frameGraph, const AbstractViewer* viewer) = 0;
			virtual void RegisterViewer(const AbstractViewer* viewer);

			virtual const Texture* RetrieveLightShadowmap(const BakedFrameGraph& bakedGraph, const AbstractViewer* viewer) const = 0;

			virtual void UnregisterMaterialInstance(const MaterialInstance& matInstance) = 0;
			virtual void UnregisterViewer(const AbstractViewer* viewer);

			LightShadowData& operator=(const LightShadowData&) = delete;
			LightShadowData& operator=(LightShadowData&&) = delete;

		protected:
			inline void UpdatePerViewerStatus(bool isPerViewer);

		private:
			bool m_isPerViewer;
	};
}

#include <Nazara/Graphics/LightShadowData.inl>

#endif // NAZARA_GRAPHICS_LIGHTSHADOWDATA_HPP
