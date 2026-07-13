// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/DirectionalLightShadowData.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/DirectionalLight.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/ShadowAtlas.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <NazaraUtils/StackVector.hpp>

namespace Nz
{
	DirectionalLightShadowData::DirectionalLightShadowData(FramePipeline& pipeline, const DirectionalLight& light, std::size_t cascadeCount) :
	m_cascadeCount(cascadeCount),
	m_pipeline(pipeline),
	m_light(light),
	m_isShadowStabilizationEnabled(true),
	m_depthPlaneFactor(5.f)
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

	void DirectionalLightShadowData::ForEachView(FunctionRef<void(std::size_t shadowAtlasEntry, ShadowViewer& shadowViewer)> callback)
	{
		for (auto&& [viewer, viewerData] : m_viewerData)
		{
			std::size_t shadowAtlasIndex = viewerData->firstShadowAtlasIndex;
			for (CascadeData& cascade : viewerData->cascades)
				callback(shadowAtlasIndex++, cascade.viewer);
		}
	}

	void DirectionalLightShadowData::PrepareRendering(RenderResources& renderResources)
	{
		// Push unregistered viewers data for release
		for (auto& perViewerData : m_destructionQueue)
			renderResources.PushForRelease(std::move(perViewerData));
		m_destructionQueue.clear();

		for (auto&& [viewer, viewerData] : m_viewerData)
		{
			const ViewerInstance& viewerInstance = viewer->GetViewerInstance();

			// Extract frustum from main viewer
			const Vector3f& eyePosition = viewerInstance.GetEyePosition();
			const Matrix4f& viewProjMatrix = viewerInstance.GetViewProjMatrix();

			// Don't use viewer instance values as they can be infinite
			float nearPlane = viewer->GetZNear();
			float farPlane = viewer->GetZFar();

			StackArray<float> cascadeSplits = NazaraStackArrayNoInit(float, m_cascadeCount - 1);
			if (m_light.IsUsingFixedShadowCascadeSplit())
			{
				std::span<const float> splitFactors = m_light.GetShadowCascadeFixedSplitFactors();
				cascadeSplits.fill(1.f);

				std::size_t splitCount = std::min(cascadeSplits.size(), splitFactors.size());
				for (std::size_t i = 0; i < splitCount; ++i)
					cascadeSplits[i] = splitFactors[i];
			}
			else
			{
				// Calculate split depths based on view camera frustum
				// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
				float lambda = m_light.GetShadowCascadeSplitLambda();

				float ratio = farPlane / nearPlane;
				float clipRange = farPlane - nearPlane;

				for (std::size_t i = 0; i < m_cascadeCount - 1; i++)
				{
					float p = float(i + 1) / float(m_cascadeCount);
					float log = nearPlane * std::pow(ratio, p);
					float uniform = nearPlane + clipRange * p;
					float d = lambda * (log - uniform) + uniform;
					cascadeSplits[i] = (d - nearPlane) / clipRange;
				}
			}

			StackVector<Frustumf> frustums = NazaraStackVector(Frustumf, m_cascadeCount);
			StackVector<float> frustumDists = NazaraStackVector(float, m_cascadeCount);

			Frustumf frustum = Frustumf::Extract(viewProjMatrix, viewer->IsZReversed());
			if (frustum.HasInfiniteFarPlane())
				frustum.UpdateFarPlaneDistance(nearPlane, farPlane);

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
				CascadeData& cascade = viewerData->cascades[cascadeIndex];
				ComputeLightView(cascade, frustums[cascadeIndex], frustumDists[cascadeIndex]);

				if (m_isShadowStabilizationEnabled)
					StabilizeShadows(cascade);

				constexpr Matrix4f biasMatrix(
					0.5f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.5f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.5f, 0.5f, 0.0f, 1.0f
				);

				ViewerInstance& cascadeViewerInstance = cascade.viewer.GetViewerInstance();
				cascade.viewProjMatrix = cascadeViewerInstance.GetViewProjMatrix() * biasMatrix;

				m_pipeline.QueueTransfer(&cascadeViewerInstance);

				// Prepare depth pass
				Frustumf lightFrustum = Frustumf::Extract(cascadeViewerInstance.GetViewProjMatrix());
				lightFrustum.SetInfiniteNearPlane();
				//m_pipeline.GetDebugDrawer().DrawFrustum(lightFrustum, cascadeColors[cascadeIndex]);
			}
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

		if (zNear < 0.f)
			zNear *= m_depthPlaneFactor;
		else
			zNear /= m_depthPlaneFactor;

		if (zFar > 0.f)
			zFar *= m_depthPlaneFactor;
		else
			zFar /= m_depthPlaneFactor;

		Matrix4f lightProj = Matrix4f::Ortho(left, right, top, bottom, zNear, zFar);

		shadowViewer.UpdateProjViewMatrices(lightProj, lightView);
		shadowViewer.UpdateEyePosition(frustumCenter);
		shadowViewer.UpdateNearFarPlanes(Infinity(), zFar);
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

	void DirectionalLightShadowData::RegisterToAtlas(ShadowAtlas& atlas)
	{
		UInt32 shadowMapSize = m_light.GetShadowMapSize();

		for (auto&& [viewer, viewerData] : m_viewerData)
			viewerData->firstShadowAtlasIndex = atlas.Register(shadowMapSize, m_cascadeCount);
	}

	void DirectionalLightShadowData::RegisterViewer(const AbstractViewer* viewer)
	{
		std::unique_ptr<PerViewerData> perViewerData = std::make_unique<PerViewerData>();
		perViewerData->cascades.resize(m_cascadeCount);

		UInt32 shadowMapSize = m_light.GetShadowMapSize();
		for (CascadeData& cascade : perViewerData->cascades)
		{
			ShadowViewer& shadowViewer = cascade.viewer;
			shadowViewer.UpdateRenderMask(0xFFFFFFFF);
			shadowViewer.UpdateViewport(Recti(0, 0, SafeCast<int>(shadowMapSize), SafeCast<int>(shadowMapSize)));
		}

		assert(m_viewerData.find(viewer) == m_viewerData.end());
		m_viewerData[viewer] = std::move(perViewerData);
	}

	void DirectionalLightShadowData::UnregisterViewer(const AbstractViewer* viewer)
	{
		auto it = m_viewerData.find(viewer);
		assert(it != m_viewerData.end());

		m_destructionQueue.push_back(std::move(it->second));
		m_viewerData.erase(it);
	}

	void DirectionalLightShadowData::WriteToShader(const ShadowAtlas& atlas, const AbstractViewer* viewer, void* basePtr) const
	{
		PerViewerData& viewerData = *Retrieve(m_viewerData, viewer);

		AccessByOffset<UInt32&>(basePtr, PredefinedDirectionalShadowAtlasEntryOffsets.cascadeCount) = SafeCaster(m_cascadeCount);
		for (std::size_t i = 0; i < m_cascadeCount; ++i)
		{
			std::optional<Rectf> rect = atlas.GetNormalizedRect(viewerData.firstShadowAtlasIndex + i);
			if (!rect)
				rect = Rectf(-1.0f, -1.0f, -1.0f, -1.0f);

			AccessByOffset<Vector2f&>(basePtr, PredefinedDirectionalShadowAtlasEntryOffsets.offset + i * sizeof(Vector2f)) = rect->GetPosition();
			AccessByOffset<Vector2f&>(basePtr, PredefinedDirectionalShadowAtlasEntryOffsets.size + i * sizeof(Vector2f)) = rect->GetLengths();
			AccessByOffset<Matrix4f&>(basePtr, PredefinedDirectionalShadowAtlasEntryOffsets.viewProjMatrices + i * sizeof(Matrix4f)) = viewerData.cascades[i].viewProjMatrix;
			AccessByOffset<float&>(basePtr, PredefinedDirectionalShadowAtlasEntryOffsets.cascadeDistances + i * sizeof(float)) = viewerData.cascades[i].distance;
		}
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
