// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SpotLightShadowData.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/SpotLight.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	SpotLightShadowData::SpotLightShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry, const SpotLight& light) :
	m_pipeline(pipeline),
	m_light(light)
	{
		UInt32 shadowMapSize = light.GetShadowMapSize();
		m_viewer.UpdateRenderMask(0xFFFFFFFF);
		m_viewer.UpdateViewport(Recti(0, 0, SafeCast<int>(shadowMapSize), SafeCast<int>(shadowMapSize)));

		ViewerInstance& viewerInstance = m_viewer.GetViewerInstance();
		viewerInstance.UpdateProjectionMatrix(Matrix4f::Perspective(m_light.GetOuterAngle() * 2.f, 1.f, 0.01f, m_light.GetRadius()));

		m_onLightShadowMapSettingChange.Connect(m_light.OnLightShadowMapSettingChange, [this](Light* /*light*/, PixelFormat /*newPixelFormat*/, UInt32 newSize)
		{
			m_viewer.UpdateViewport(Recti(0, 0, SafeCast<int>(newSize), SafeCast<int>(newSize)));
		});

		m_onLightTransformInvalidated.Connect(m_light.OnLightTransformInvalided, [this]([[maybe_unused]] Light* light)
		{
			assert(&m_light == light);

			ViewerInstance& viewerInstance = m_viewer.GetViewerInstance();
			viewerInstance.UpdateEyePosition(m_light.GetPosition());
			viewerInstance.UpdateViewMatrix(Nz::Matrix4f::TransformInverse(m_light.GetPosition(), m_light.GetRotation()));

			m_pipeline.QueueTransfer(&viewerInstance);
		});
		viewerInstance.UpdateEyePosition(m_light.GetPosition());
		viewerInstance.UpdateViewMatrix(Nz::Matrix4f::TransformInverse(m_light.GetPosition(), m_light.GetRotation()));
		m_pipeline.QueueTransfer(&viewerInstance);

		std::size_t shadowPassIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex("ShadowPass");

		m_depthPass.emplace(m_pipeline, elementRegistry, &m_viewer, shadowPassIndex, "Spotlight shadow mapping");
		m_pipeline.ForEachRegisteredMaterialInstance([this](const MaterialInstance& matInstance)
		{
			m_depthPass->RegisterMaterialInstance(matInstance);
		});
	}

	void SpotLightShadowData::PrepareRendering(RenderFrame& renderFrame)
{
		const Matrix4f& viewProjMatrix = m_viewer.GetViewerInstance().GetViewProjMatrix();

		Frustumf frustum = Frustumf::Extract(viewProjMatrix);

		std::size_t visibilityHash = 5U;
		const auto& visibleRenderables = m_pipeline.FrustumCull(frustum, 0xFFFFFFFF, visibilityHash);

		m_depthPass->Prepare(renderFrame, frustum, visibleRenderables, visibilityHash);
	}

	void SpotLightShadowData::RegisterMaterialInstance(const MaterialInstance& matInstance)
	{
		m_depthPass->RegisterMaterialInstance(matInstance);
	}

	void SpotLightShadowData::RegisterPassInputs(FramePass& pass)
	{
		pass.AddInput(m_attachmentIndex);
	}

	void SpotLightShadowData::RegisterToFrameGraph(FrameGraph& frameGraph)
	{
		UInt32 shadowMapSize = m_light.GetShadowMapSize();

		m_attachmentIndex = frameGraph.AddAttachment({
			"Shadowmap",
			m_light.GetShadowMapFormat(),
			FramePassAttachmentSize::Fixed,
			shadowMapSize, shadowMapSize,
		});

		m_depthPass->RegisterToFrameGraph(frameGraph, m_attachmentIndex);
	}

	const Nz::Texture* SpotLightShadowData::RetrieveLightShadowmap(const BakedFrameGraph& bakedGraph) const
	{
		return bakedGraph.GetAttachmentTexture(m_attachmentIndex).get();
	}

	void SpotLightShadowData::UnregisterMaterialInstance(const MaterialInstance& matInstance)
	{
		m_depthPass->UnregisterMaterialInstance(matInstance);
	}
}
