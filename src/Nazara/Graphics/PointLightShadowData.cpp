// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/PointLightShadowData.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Math/Quaternion.hpp>

namespace Nz
{
	namespace
	{
		// TODO: Make it constexpr
		std::array s_dirRotations = {
			 Quaternionf::RotationBetween(Vector3f::Forward(),  Vector3f::UnitX()),
			 Quaternionf::RotationBetween(Vector3f::Forward(), -Vector3f::UnitX()),
			 Quaternionf::RotationBetween(Vector3f::Forward(),  Vector3f::UnitY()),
			 Quaternionf::RotationBetween(Vector3f::Forward(), -Vector3f::UnitY()),
			 Quaternionf::RotationBetween(Vector3f::Forward(), -Vector3f::UnitZ()), //< Z is reversed as the cubemap convention is left-handed
			 Quaternionf::RotationBetween(Vector3f::Forward(),  Vector3f::UnitZ()),
		};

		constexpr std::array<std::string_view, 6> s_dirNames = {
			"Point-light shadow mapping +X",
			"Point-light shadow mapping -X",
			"Point-light shadow mapping +Y",
			"Point-light shadow mapping -Y",
			"Point-light shadow mapping -Z",
			"Point-light shadow mapping +Z"
		};
	}

	PointLightShadowData::PointLightShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry, const PointLight& light) :
	m_pipeline(pipeline),
	m_light(light)
	{
		m_onLightShadowMapSettingChange.Connect(m_light.OnLightShadowMapSettingChange, [this](Light* /*light*/, PixelFormat /*newPixelFormat*/, UInt32 newSize)
		{
			for (DirectionData& direction : m_directions)
				direction.viewer.UpdateViewport(Recti(0, 0, SafeCast<int>(newSize), SafeCast<int>(newSize)));
		});

		m_onLightTransformInvalidated.Connect(m_light.OnLightTransformInvalided, [this]([[maybe_unused]] Light* light)
		{
			assert(&m_light == light);

			for (std::size_t i = 0; i < m_directions.size(); ++i)
			{
				DirectionData& direction = m_directions[i];

				ViewerInstance& viewerInstance = direction.viewer.GetViewerInstance();
				viewerInstance.UpdateEyePosition(m_light.GetPosition());
				viewerInstance.UpdateViewMatrix(Matrix4f::TransformInverse(m_light.GetPosition(), s_dirRotations[i]));

				m_pipeline.QueueTransfer(&viewerInstance);
			}
		});

		std::size_t shadowPassIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex("DistanceShadowPass");

		constexpr float zNear = 0.01f;

		Matrix4f projectionMatrix = Matrix4f::Perspective(RadianAnglef(HalfPi<float>), 1.f, zNear, m_light.GetRadius());

		UInt32 shadowMapSize = light.GetShadowMapSize();
		for (std::size_t i = 0; i < m_directions.size(); ++i)
		{
			ShadowViewer& viewer = m_directions[i].viewer;

			viewer.UpdateRenderMask(0xFFFFFFFF);
			viewer.UpdateViewport(Recti(0, 0, SafeCast<int>(shadowMapSize), SafeCast<int>(shadowMapSize)));

			ViewerInstance& viewerInstance = viewer.GetViewerInstance();
			viewerInstance.UpdateEyePosition(m_light.GetPosition());
			viewerInstance.UpdateProjectionMatrix(projectionMatrix);
			viewerInstance.UpdateViewMatrix(Matrix4f::TransformInverse(m_light.GetPosition(), s_dirRotations[i]));
			viewerInstance.UpdateNearFarPlanes(zNear, m_light.GetRadius());

			m_pipeline.QueueTransfer(&viewerInstance);

			FramePipelinePass::PassData passData = {
				&viewer,
				elementRegistry,
				m_pipeline
			};

			m_directions[i].depthPass.emplace(passData, std::string(s_dirNames[i]), shadowPassIndex);
		}

		m_pipeline.ForEachRegisteredMaterialInstance([this](const MaterialInstance& matInstance)
		{
			for (DirectionData& direction : m_directions)
				direction.depthPass->RegisterMaterialInstance(matInstance);
		});
	}

	void PointLightShadowData::PrepareRendering(RenderResources& renderResources, [[maybe_unused]] const AbstractViewer* viewer)
	{
		assert(viewer == nullptr);

		for (DirectionData& direction : m_directions)
		{
			const Matrix4f& viewProjMatrix = direction.viewer.GetViewerInstance().GetViewProjMatrix();

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

			direction.depthPass->Prepare(passData);
		}
	}

	void PointLightShadowData::RegisterMaterialInstance(const MaterialInstance& matInstance)
	{
		for (DirectionData& direction : m_directions)
			direction.depthPass->RegisterMaterialInstance(matInstance);
	}

	void PointLightShadowData::RegisterPassInputs(FramePass& pass, [[maybe_unused]] const AbstractViewer* viewer)
	{
		assert(viewer == nullptr);

		std::size_t cubeInputIndex = pass.AddInput(m_cubeAttachmentIndex);
		pass.SetInputAssumedLayout(cubeInputIndex, TextureLayout::ColorInput);

		for (DirectionData& direction : m_directions)
			pass.AddInput(direction.attachmentIndex);
	}

	void PointLightShadowData::RegisterToFrameGraph(FrameGraph& frameGraph, [[maybe_unused]] const AbstractViewer* viewer)
	{
		assert(viewer == nullptr);

		UInt32 shadowMapSize = m_light.GetShadowMapSize();

		m_cubeAttachmentIndex = frameGraph.AddAttachmentCube({
			.name = "Point-light shadowmap",
			.format = m_light.GetShadowMapFormat(),
			.size = FramePassAttachmentSize::Fixed,
			.width = shadowMapSize,
			.height = shadowMapSize,
		});

		for (std::size_t i = 0; i < m_directions.size(); ++i)
		{
			DirectionData& direction = m_directions[i];
			direction.attachmentIndex = frameGraph.AddAttachmentCubeFace(m_cubeAttachmentIndex, static_cast<CubemapFace>(i));

			FramePipelinePass::PassInputOuputs passInputOuputs;
			passInputOuputs.depthStencilOutput = direction.attachmentIndex;

			direction.depthPass->RegisterToFrameGraph(frameGraph, passInputOuputs);
		}
	}

	const Texture* PointLightShadowData::RetrieveLightShadowmap(const BakedFrameGraph& bakedGraph, const AbstractViewer* /*viewer*/) const
	{
		return bakedGraph.GetAttachmentTexture(m_cubeAttachmentIndex).get();
	}

	void PointLightShadowData::UnregisterMaterialInstance(const MaterialInstance& matInstance)
	{
		for (DirectionData& direction : m_directions)
			direction.depthPass->UnregisterMaterialInstance(matInstance);
	}
}
