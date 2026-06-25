// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_LIGHTSHADOWDATA_HPP
#define NAZARA_GRAPHICS_LIGHTSHADOWDATA_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <utility>

namespace Nz
{
	class AbstractViewer;
	class BakedFrameGraph;
	class FrameGraph;
	class FramePass;
	class MaterialInstance;
	class RenderResources;
	class ShadowAtlas;
	class ShadowViewer;
	class Texture;

	class NAZARA_GRAPHICS_API LightShadowData
	{
		public:
			inline LightShadowData();
			LightShadowData(const LightShadowData&) = delete;
			LightShadowData(LightShadowData&&) = delete;
			virtual ~LightShadowData();

			virtual void ForEachView(FunctionRef<void(std::size_t shadowAtlasEntry, ShadowViewer& shadowViewer)> callback) = 0;

			inline bool IsPerViewer() const;

			virtual void PrepareRendering(RenderResources& renderResources);

			virtual void RegisterToAtlas(ShadowAtlas& atlas) = 0;
			virtual void RegisterViewer(const AbstractViewer* viewer);

			virtual void UnregisterViewer(const AbstractViewer* viewer);

			virtual void WriteToShader(const ShadowAtlas& atlas, const AbstractViewer* viewer, void* basePtr) const = 0;

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
