// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DirectionalLightShadowData.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/DirectionalLight.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <NazaraUtils/StackVector.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	DirectionalLightShadowData::DirectionalLightShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry, const DirectionalLight& light, std::size_t cascadeCount) :
	m_cascadeCount(cascadeCount),
	m_elementRegistry(elementRegistry),
	m_pipeline(pipeline),
	m_light(light),
	m_isShadowStabilizationEnabled(true)
	{
		m_onLightShadowMapSettingChange.Connect(m_light.OnLightShadowMapSettingChange, [this](Light* /*light*/, PixelFormat /*newPixelFormat*/, UInt32 newSize)
		{
			m_texelScale = 2.0f / newSize;
			m_invTexelScale = 1.0f / m_texelScale;

			ForEachCascade([newSize](CascadeData& cascade)
			{
				cascade.viewer.UpdateViewport(Recti(0, 0, SafeCast<int>(newSize), SafeCast<int>(newSize)));
			});
		});
		m_texelScale = 2.0f / m_light.GetShadowMapSize();
		m_invTexelScale = 1.0f / m_texelScale;

		UpdatePerViewerStatus(true);
	}

	void DirectionalLightShadowData::PrepareRendering(RenderResources& renderResources, const AbstractViewer* viewer)
	{
		assert(viewer);
		PerViewerData& viewerData = *Retrieve(m_viewerData, viewer);

		const ViewerInstance& viewerInstance = viewer->GetViewerInstance();

		// Extract frustum from main viewer
		const Vector3f& eyePosition = viewerInstance.GetEyePosition();
		const Matrix4f& viewProjMatrix = viewerInstance.GetViewProjMatrix();

		float nearPlane = viewerInstance.GetNearPlane();
		float farPlane = viewerInstance.GetFarPlane();

		// Calculate split depths based on view camera frustum
		// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
		constexpr float lambda = 0.95f;

		float ratio = farPlane / nearPlane;
		float clipRange = farPlane - nearPlane;

		StackArray<float> cascadeSplits = NazaraStackArrayNoInit(float, m_cascadeCount - 1);
		for (uint32_t i = 0; i < m_cascadeCount - 1; i++)
		{
			float p = float(i + 1) / float(m_cascadeCount);
			float log = nearPlane * std::pow(ratio, p);
			float uniform = nearPlane + clipRange * p;
			float d = lambda * (log - uniform) + uniform;
			cascadeSplits[i] = (d - nearPlane) / clipRange;
		}

		StackVector<Frustumf> frustums = NazaraStackVector(Frustumf, m_cascadeCount);
		StackVector<float> frustumDists = NazaraStackVector(float, m_cascadeCount);

		Frustumf frustum = Frustumf::Extract(viewProjMatrix);
		frustum.Split(cascadeSplits.data(), m_cascadeCount - 1, [&](float zNearPct, float zFarPct)
		{
			frustums.push_back(frustum.Reduce(zNearPct, zFarPct));
			frustumDists.push_back(frustums.back().GetPlane(FrustumPlane::Far).SignedDistance(eyePosition));
		});

		constexpr std::array cascadeColors = {
			Color::Green(),
			Color::Yellow(),
			Color::Red(),
			Color::Blue(),
			Color::Cyan(),
			Color::Magenta(),
			Color::Orange(),
			Color::Gray()
		};

		for (std::size_t cascadeIndex = 0; cascadeIndex < m_cascadeCount; ++cascadeIndex)
		{
			CascadeData& cascade = viewerData.cascades[cascadeIndex];
			ComputeLightView(cascade, frustums[cascadeIndex], frustumDists[cascadeIndex]);

			if (m_isShadowStabilizationEnabled)
				StabilizeShadows(cascade);

			constexpr Matrix4f biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
			                              0.0f, 0.5f, 0.0f, 0.0f,
			                              0.0f, 0.0f, 1.0f, 0.0f,
			                              0.5f, 0.5f, 0.0f, 1.0f);

			ViewerInstance& cascadeViewerInstance = cascade.viewer.GetViewerInstance();
			cascade.viewProjMatrix = cascadeViewerInstance.GetViewProjMatrix() * biasMatrix;

			m_pipeline.QueueTransfer(&cascadeViewerInstance);

			// Prepare depth pass
			Frustumf lightFrustum = Frustumf::Extract(cascadeViewerInstance.GetViewProjMatrix());

			//m_pipeline.GetDebugDrawer().DrawFrustum(lightFrustum, cascadeColors[cascadeIndex]);

			std::size_t visibilityHash = 5U;
			const auto& visibleRenderables = m_pipeline.FrustumCull(lightFrustum, 0xFFFFFFFF, visibilityHash);

			FramePipelinePass::FrameData passData = {
				nullptr,
				frustum,
				renderResources,
				visibleRenderables,
				visibilityHash
			};

			cascade.depthPass->Prepare(passData);
		}
	}

	void DirectionalLightShadowData::ComputeLightView(CascadeData& cascade, const Frustumf& cascadeFrustum, float cascadeDist)
	{
		ViewerInstance& shadowViewer = cascade.viewer.GetViewerInstance();

		EnumArray<BoxCorner, Vector3f> frustumCorners = cascadeFrustum.ComputeCorners();

		// Get frustum center
		Vector3f frustumCenter = Vector3f::Zero();
		for (const Vector3f& corner : frustumCorners)
			frustumCenter += corner;

		frustumCenter /= float(frustumCorners.size());

		// Compute radius (= biggest distance to the center)
		float radius = 0.f;
		for (const Vector3f& corner : frustumCorners)
			radius = std::max(radius, frustumCenter.SquaredDistance(corner));

		radius = std::ceil(std::sqrt(radius));

		Matrix4f lightView = Matrix4f::TransformInverse(frustumCenter, m_light.GetRotation());

		// Compute light projection matrix
		Vector3f maxExtent = frustumCenter + Vector3f(radius);
		Vector3f minExtent = frustumCenter - Vector3f(radius);

		Boxf aabb = Boxf::FromExtents(minExtent, maxExtent);

		float left = std::numeric_limits<float>::infinity();
		float right = -std::numeric_limits<float>::infinity();
		float top = std::numeric_limits<float>::infinity();
		float bottom = -std::numeric_limits<float>::infinity();
		float zNear = std::numeric_limits<float>::infinity();
		float zFar = -std::numeric_limits<float>::infinity();
		for (const Vector3f& corner : aabb.GetCorners())
		{
			Vector3f viewCorner = lightView * corner;

			left = std::min(left, viewCorner.x);
			right = std::max(right, viewCorner.x);
			top = std::min(top, viewCorner.y);
			bottom = std::max(bottom, viewCorner.y);
			zNear = std::min(zNear, viewCorner.z);
			zFar = std::max(zFar, viewCorner.z);
		}

		cascade.distance = cascadeDist;

		Matrix4f lightProj = Matrix4f::Ortho(left, right, top, bottom, zNear, zFar);

		shadowViewer.UpdateProjViewMatrices(lightProj, lightView);
		shadowViewer.UpdateEyePosition(frustumCenter);
		shadowViewer.UpdateNearFarPlanes(zNear, zFar);
	}

	void DirectionalLightShadowData::StabilizeShadows(CascadeData& cascade)
	{
		ViewerInstance& shadowViewer = cascade.viewer.GetViewerInstance();

		// Stabilize cascade shadows by keeping the center to a texel boundary
		// see Michal Valient's article "Stable Cascaded Shadow Maps"
		Vector4f shadowOrigin = shadowViewer.GetViewProjMatrix() * Vector4f(0.f, 0.f, 0.f, 1.f);
		shadowOrigin *= m_invTexelScale;

		Vector2f roundedOrigin = { std::round(shadowOrigin.x), std::round(shadowOrigin.y) };
		Vector2f roundOffset = roundedOrigin - Vector2f(shadowOrigin);
		roundOffset *= m_texelScale;

		Matrix4f lightProj = shadowViewer.GetProjectionMatrix();
		lightProj.ApplyTranslation(Vector3f(roundOffset.x, roundOffset.y, 0.f));
		shadowViewer.UpdateProjectionMatrix(lightProj);
	}

	void DirectionalLightShadowData::RegisterMaterialInstance(const MaterialInstance& matInstance)
	{
		ForEachCascade([&](CascadeData& cascade)
		{
			cascade.depthPass->RegisterMaterialInstance(matInstance);
		});
	}

	void DirectionalLightShadowData::RegisterPassInputs(FramePass& pass, const AbstractViewer* viewer)
	{
		assert(viewer);
		PerViewerData& viewerData = *Retrieve(m_viewerData, viewer);

		std::size_t arrayInputIndex = pass.AddInput(viewerData.textureArrayAttachmentIndex);
		pass.SetInputAssumedLayout(arrayInputIndex, TextureLayout::ColorInput);

		for (CascadeData& cascade : viewerData.cascades)
			pass.AddInput(cascade.attachmentIndex);
	}

	void DirectionalLightShadowData::RegisterToFrameGraph(FrameGraph& frameGraph, const AbstractViewer* viewer)
	{
		UInt32 shadowMapSize = m_light.GetShadowMapSize();

		PerViewerData& viewerData = *Retrieve(m_viewerData, viewer);

		viewerData.textureArrayAttachmentIndex = frameGraph.AddAttachmentArray({
			.name = "Directional-light cascade shadowmaps",
			.format = m_light.GetShadowMapFormat(),
			.size = FramePassAttachmentSize::Fixed,
			.width = shadowMapSize,
			.height = shadowMapSize,
		}, SafeCast<unsigned int>(m_cascadeCount));

		for (std::size_t i = 0; i < viewerData.cascades.size(); ++i)
		{
			CascadeData& cascade = viewerData.cascades[i];

			cascade.attachmentIndex = frameGraph.AddAttachmentArrayLayer(viewerData.textureArrayAttachmentIndex, i);

			FramePipelinePass::PassInputOuputs passInputOuputs;
			passInputOuputs.depthStencilOutput = cascade.attachmentIndex;

			cascade.depthPass->RegisterToFrameGraph(frameGraph, passInputOuputs);
		}
	}

	void DirectionalLightShadowData::RegisterViewer(const AbstractViewer* viewer)
	{
		std::size_t shadowPassIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex("ShadowPass");

		std::unique_ptr<PerViewerData> perViewerData = std::make_unique<PerViewerData>();
		perViewerData->cascades.resize(m_cascadeCount);

		std::size_t cascadeIndex = 0;

		UInt32 shadowMapSize = m_light.GetShadowMapSize();
		for (CascadeData& cascade : perViewerData->cascades)
		{
			ShadowViewer& shadowViewer = cascade.viewer;

			shadowViewer.UpdateRenderMask(0xFFFFFFFF);
			shadowViewer.UpdateViewport(Recti(0, 0, SafeCast<int>(shadowMapSize), SafeCast<int>(shadowMapSize)));

			FramePipelinePass::PassData passData = {
				&shadowViewer,
				m_elementRegistry,
				m_pipeline
			};

			cascade.depthPass.emplace(passData, Format("Cascade #{}", cascadeIndex++), shadowPassIndex);
		}

		m_pipeline.ForEachRegisteredMaterialInstance([&](const MaterialInstance& matInstance)
		{
			for (CascadeData& cascade : perViewerData->cascades)
				cascade.depthPass->RegisterMaterialInstance(matInstance);
		});

		assert(m_viewerData.find(viewer) == m_viewerData.end());
		m_viewerData[viewer] = std::move(perViewerData);
	}

	const Texture* DirectionalLightShadowData::RetrieveLightShadowmap(const BakedFrameGraph& bakedGraph, const AbstractViewer* viewer) const
	{
		assert(viewer);
		const PerViewerData& viewerData = *Retrieve(m_viewerData, viewer);

		return bakedGraph.GetAttachmentTexture(viewerData.textureArrayAttachmentIndex).get();
	}

	void DirectionalLightShadowData::UnregisterMaterialInstance(const MaterialInstance& matInstance)
	{
		ForEachCascade([&](CascadeData& cascade)
		{
			cascade.depthPass->UnregisterMaterialInstance(matInstance);
		});
	}

	void DirectionalLightShadowData::UnregisterViewer(const AbstractViewer* viewer)
	{
		m_viewerData.erase(viewer);
	}

	template<typename F>
	void DirectionalLightShadowData::ForEachCascade(F&& callback)
	{
		for (auto it = m_viewerData.begin(); it != m_viewerData.end(); ++it)
		{
			for (CascadeData& cascade : it->second->cascades)
				callback(cascade);
		}
	}
}
