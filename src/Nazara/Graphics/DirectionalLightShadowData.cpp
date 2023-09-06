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

	void DirectionalLightShadowData::PrepareRendering(RenderFrame& renderFrame, const AbstractViewer* viewer)
	{
		assert(viewer);
		PerViewerData& viewerData = *Retrieve(m_viewerData, viewer);

		// Extract frustum from main viewer
		const Vector3f& eyePosition = viewer->GetViewerInstance().GetEyePosition();
		const Matrix4f& viewProjMatrix = viewer->GetViewerInstance().GetViewProjMatrix();

		//std::array planePct = { 0.1f, 0.3f, 0.6f }; // TODO: Generate the separations based on other settings
		std::array planePct = { 0.03f, 0.1f, 0.2f, 0.5f }; // TODO: Generate the separations based on other settings
		assert(m_cascadeCount <= planePct.size() + 1);

		StackVector<Frustumf> frustums = NazaraStackVector(Frustumf, m_cascadeCount);
		StackVector<float> frustumDists = NazaraStackVector(float, m_cascadeCount);

		Frustumf frustum = Frustumf::Extract(viewProjMatrix);
		frustum.Split(planePct.data(), m_cascadeCount - 1, [&](float zNearPct, float zFarPct)
		{
			frustums.push_back(frustum.Reduce(zNearPct, zFarPct));
			frustumDists.push_back(frustums.back().GetPlane(FrustumPlane::Far).SignedDistance(eyePosition));
		});

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

			ViewerInstance& viewerInstance = cascade.viewer.GetViewerInstance();
			cascade.viewProjMatrix = viewerInstance.GetViewProjMatrix() * biasMatrix;

			m_pipeline.QueueTransfer(&viewerInstance);

			// Prepare depth pass
			Frustumf lightFrustum = Frustumf::Extract(viewerInstance.GetViewProjMatrix());

			std::size_t visibilityHash = 5U;
			const auto& visibleRenderables = m_pipeline.FrustumCull(lightFrustum, 0xFFFFFFFF, visibilityHash);

			cascade.depthPass->Prepare(renderFrame, lightFrustum, visibleRenderables, visibilityHash);
		}
	}

	void DirectionalLightShadowData::ComputeLightView(CascadeData& cascade, const Frustumf& cascadeFrustum, float cascadeDist)
	{
		ViewerInstance& viewerInstance = cascade.viewer.GetViewerInstance();

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
		Boxf aabb = Boxf::FromExtends(frustumCenter - Vector3f(radius), frustumCenter + Vector3f(radius));

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

		// Tune this parameter according to the scene
		constexpr float zMult = 2.0f;
		if (zNear < 0)
			zNear *= zMult;
		else
			zNear /= zMult;

		if (zFar < 0)
			zFar /= zMult;
		else
			zFar *= zMult;

		cascade.distance = cascadeDist;

		Matrix4f lightProj = Matrix4f::Ortho(left, right, top, bottom, zNear, zFar);

		viewerInstance.UpdateProjViewMatrices(lightProj, lightView);
		viewerInstance.UpdateEyePosition(frustumCenter);
		viewerInstance.UpdateNearFarPlanes(zNear, zFar);
	}

	void DirectionalLightShadowData::StabilizeShadows(CascadeData& cascade)
	{
		ViewerInstance& viewerInstance = cascade.viewer.GetViewerInstance();

		// Stabilize cascade shadows by keeping the center to a texel boundary of the previous frame
		// https://www.junkship.net/News/2020/11/22/shadow-of-a-doubt-part-2
		Vector4f shadowOrigin(0.0f, 0.0f, 0.0f, 1.0f);
		shadowOrigin = viewerInstance.GetViewProjMatrix() * shadowOrigin;
		shadowOrigin *= m_texelScale;

		Vector2f roundedOrigin = { std::round(shadowOrigin.x), std::round(shadowOrigin.y) };
		Vector2f roundOffset = roundedOrigin - Vector2f(shadowOrigin);
		roundOffset *= m_invTexelScale;

		Matrix4f lightProj = viewerInstance.GetProjectionMatrix();
		lightProj.ApplyTranslation(Vector3f(roundOffset.x, roundOffset.y, 0.f));
		viewerInstance.UpdateProjectionMatrix(lightProj);
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
		pass.SetInputLayout(arrayInputIndex, TextureLayout::ColorInput);

		for (CascadeData& cascade : viewerData.cascades)
			pass.AddInput(cascade.attachmentIndex);
	}

	void DirectionalLightShadowData::RegisterToFrameGraph(FrameGraph& frameGraph, const AbstractViewer* viewer)
	{
		UInt32 shadowMapSize = m_light.GetShadowMapSize();

		PerViewerData& viewerData = *Retrieve(m_viewerData, viewer);

		viewerData.textureArrayAttachmentIndex = frameGraph.AddAttachmentArray({
			"Directional-light cascade shadowmaps",
			m_light.GetShadowMapFormat(),
			FramePassAttachmentSize::Fixed,
			shadowMapSize, shadowMapSize,
		}, m_cascadeCount);

		for (std::size_t i = 0; i < viewerData.cascades.size(); ++i)
		{
			CascadeData& cascade = viewerData.cascades[i];

			cascade.attachmentIndex = frameGraph.AddAttachmentArrayLayer(viewerData.textureArrayAttachmentIndex, i);
			cascade.depthPass->RegisterToFrameGraph(frameGraph, cascade.attachmentIndex);
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

			cascade.depthPass.emplace(m_pipeline, m_elementRegistry, &shadowViewer, shadowPassIndex, Format("Cascade #{}", cascadeIndex++));
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
