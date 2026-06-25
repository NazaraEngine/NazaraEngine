// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_POINTLIGHTSHADOWDATA_HPP
#define NAZARA_GRAPHICS_POINTLIGHTSHADOWDATA_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/LightShadowData.hpp>
#include <Nazara/Graphics/ShadowViewer.hpp>
#include <array>

namespace Nz
{
	class FramePipeline;
	class PointLight;

	class NAZARA_GRAPHICS_API PointLightShadowData : public LightShadowData
	{
		public:
			PointLightShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry, const PointLight& light);
			PointLightShadowData(const PointLightShadowData&) = delete;
			PointLightShadowData(PointLightShadowData&&) = delete;
			~PointLightShadowData() = default;

			void ForEachView(FunctionRef<void(std::size_t shadowAtlasEntry, ShadowViewer& shadowViewer)> callback) override;

			void RegisterToAtlas(ShadowAtlas& atlas) override;

			void WriteToShader(const ShadowAtlas& atlas, const AbstractViewer* viewer, void* basePtr) const override;

			PointLightShadowData& operator=(const PointLightShadowData&) = delete;
			PointLightShadowData& operator=(PointLightShadowData&&) = delete;

		private:
			NazaraSlot(Light, OnLightDataInvalidated, m_onLightDataInvalidated);
			NazaraSlot(Light, OnLightShadowMapSettingChange, m_onLightShadowMapSettingChange);
			NazaraSlot(Light, OnLightTransformInvalidated, m_onLightTransformInvalidated);

			struct DirectionData
			{
				std::size_t attachmentIndex;
				ShadowViewer viewer;
			};

			std::array<DirectionData, 6> m_directions;
			std::size_t m_cubeAttachmentIndex;
			std::size_t m_firstShadowAtlasIndex;
			FramePipeline& m_pipeline;
			const PointLight& m_light;
	};
}

#include <Nazara/Graphics/PointLightShadowData.inl>

#endif // NAZARA_GRAPHICS_POINTLIGHTSHADOWDATA_HPP
