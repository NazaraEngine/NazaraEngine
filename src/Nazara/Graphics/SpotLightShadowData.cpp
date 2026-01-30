// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/SpotLightShadowData.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/SpotLight.hpp>
#include <cassert>

namespace Nz
{
	SpotLightShadowData::SpotLightShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry, const SpotLight& light) :
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

		std::size_t shadowPassIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex("ShadowPass");

		FramePipelinePass::PassData passData = {
			&m_viewer,
			elementRegistry,
			m_pipeline
		};

		m_depthPass.emplace(passData, "Spotlight shadow mapping", shadowPassIndex);
		m_pipeline.ForEachRegisteredMaterialInstance([this](const MaterialInstance& matInstance)
		{
			m_depthPass->RegisterMaterialInstance(matInstance);
		});

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

	void SpotLightShadowData::PrepareRendering(RenderResources& renderResources, [[maybe_unused]] const AbstractViewer* viewer)
	{
		assert(viewer == nullptr);

		const Matrix4f& viewProjMatrix = m_viewer.GetViewerInstance().GetViewProjMatrix();

		Frustumf frustum = Frustumf::Extract(viewProjMatrix);

		std::size_t visibilityHash = 5U;
		const auto& visibleRenderables = m_pipeline.FrustumCull(frustum, 0xFFFFFFFF, visibilityHash);

		FramePipelinePass::FrameData passData = {
			nullptr,
			frustum,
			renderResources,
			visibleRenderables,
			visibilityHash
		};

		m_depthPass->Prepare(passData);
	}

	void SpotLightShadowData::RegisterMaterialInstance(const MaterialInstance& matInstance)
	{
		m_depthPass->RegisterMaterialInstance(matInstance);
	}

	void SpotLightShadowData::RegisterPassInputs(FramePass& pass, [[maybe_unused]] const AbstractViewer* viewer)
	{
		assert(viewer == nullptr);

		pass.AddInput(m_attachmentIndex);
	}

	void SpotLightShadowData::RegisterToFrameGraph(FrameGraph& frameGraph, [[maybe_unused]] const AbstractViewer* viewer)
	{
		assert(viewer == nullptr);

		UInt32 shadowMapSize = m_light.GetShadowMapSize();

		m_attachmentIndex = frameGraph.AddAttachment({
			.name = "Shadowmap",
			.format = m_light.GetShadowMapFormat(),
			.size = FramePassAttachmentSize::Fixed,
			.width = shadowMapSize,
			.height = shadowMapSize,
		});

		FramePipelinePass::PassInputOuputs passInputOuputs;
		passInputOuputs.clearDepth = 1.f;
		passInputOuputs.depthStencilOutput = m_attachmentIndex;

		m_depthPass->RegisterToFrameGraph(frameGraph, passInputOuputs);
	}

	const Texture* SpotLightShadowData::RetrieveLightShadowmap(const BakedFrameGraph& bakedGraph, const AbstractViewer* /*viewer*/) const
	{
		return bakedGraph.GetAttachmentTexture(m_attachmentIndex).get();
	}

	void SpotLightShadowData::UnregisterMaterialInstance(const MaterialInstance& matInstance)
	{
		m_depthPass->UnregisterMaterialInstance(matInstance);
	}
}
