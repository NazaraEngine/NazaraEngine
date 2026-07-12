// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/SpotLightShadowData.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/ShadowAtlas.hpp>
#include <Nazara/Graphics/SpotLight.hpp>
#include <cassert>

namespace Nz
{
	SpotLightShadowData::SpotLightShadowData(FramePipeline& pipeline, const SpotLight& light) :
	m_pipeline(pipeline),
	m_light(light)
	{
		UInt32 shadowMapSize = light.GetShadowMapSize();
		m_viewer.UpdateRenderMask(0xFFFFFFFF);
		m_viewer.UpdateViewport(Recti(0, 0, SafeCast<int>(shadowMapSize), SafeCast<int>(shadowMapSize)));

		constexpr float zNear = 0.01f;

		ViewerInstance& viewerInstance = m_viewer.GetViewerInstance();
		viewerInstance.UpdateEyePosition(m_light.GetPosition());
		viewerInstance.UpdateProjectionMatrix(Matrix4f::Perspective(m_light.GetOuterAngle() * 2.f, 1.f, zNear, m_light.GetRadius()));
		viewerInstance.UpdateNearFarPlanes(zNear, m_light.GetRadius());
		viewerInstance.UpdateViewMatrix(Nz::Matrix4f::TransformInverse(m_light.GetPosition(), m_light.GetRotation()));
		m_pipeline.QueueTransfer(&viewerInstance);

		m_onLightDataInvalidated.Connect(m_light.OnLightDataInvalidated, [this]([[maybe_unused]] Light* light)
		{
			assert(&m_light == light);

			ViewerInstance& viewerInstance = m_viewer.GetViewerInstance();
			viewerInstance.UpdateProjectionMatrix(Matrix4f::Perspective(m_light.GetOuterAngle() * 2.f, 1.f, zNear, m_light.GetRadius()));
			viewerInstance.UpdateNearFarPlanes(zNear, m_light.GetRadius());
		});

		m_onLightShadowMapSettingChange.Connect(m_light.OnLightShadowMapSettingChange, [this](Light* /*light*/, PixelFormat /*newPixelFormat*/, UInt32 newSize)
		{
			m_viewer.UpdateViewport(Recti(0, 0, SafeCast<int>(newSize), SafeCast<int>(newSize)));
		});

		m_onLightTransformInvalidated.Connect(m_light.OnLightTransformInvalidated, [this]([[maybe_unused]] Light* light)
		{
			assert(&m_light == light);

			ViewerInstance& viewerInstance = m_viewer.GetViewerInstance();
			viewerInstance.UpdateEyePosition(m_light.GetPosition());
			viewerInstance.UpdateViewMatrix(Nz::Matrix4f::TransformInverse(m_light.GetPosition(), m_light.GetRotation()));

			m_pipeline.QueueTransfer(&viewerInstance);
		});
	}

	void SpotLightShadowData::ForEachView(FunctionRef<void(std::size_t shadowAtlasEntry, ShadowViewer& shadowViewer)> callback)
	{
		callback(m_shadowAtlasIndex, m_viewer);
	}

	void SpotLightShadowData::RegisterToAtlas(ShadowAtlas& atlas)
	{
		UInt32 shadowMapSize = m_light.GetShadowMapSize();

		m_shadowAtlasIndex = atlas.Register(shadowMapSize);
	}

	void SpotLightShadowData::WriteToShader(const ShadowAtlas& atlas, [[maybe_unused]] const AbstractViewer* viewer, void* basePtr) const
	{
		assert(viewer == nullptr);

		std::optional<Rectf> rect = atlas.GetNormalizedRect(m_shadowAtlasIndex);
		if (!rect)
			rect = Rectf(-1.0f, -1.0f, -1.0f, -1.0f);

		AccessByOffset<Vector2f&>(basePtr, PredefinedSpotShadowAtlasEntryOffsets.offset) = rect->GetPosition();
		AccessByOffset<Vector2f&>(basePtr, PredefinedSpotShadowAtlasEntryOffsets.size) = rect->GetLengths();
	}
}
