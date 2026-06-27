// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/PointLightShadowData.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/ShadowAtlas.hpp>
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
		constexpr float zNear = 0.01f;

		Matrix4f projectionMatrix = Matrix4f::Perspective(RadianAnglef(HalfPi<float>()), 1.f, zNear, m_light.GetRadius());

		UInt32 shadowMapSize = light.GetShadowMapSize();
		for (std::size_t i = 0; i < m_directions.size(); ++i)
		{
			ShadowViewer& viewer = m_directions[i].viewer;

			viewer.UpdateRenderMask(0xFFFFFFFF);
			viewer.UpdateViewport(Recti(0, 0, SafeCast<int>(shadowMapSize), SafeCast<int>(shadowMapSize)));

			ViewerInstance& viewerInstance = viewer.GetViewerInstance();
			viewerInstance.UpdateEyePosition(m_light.GetPosition());
			viewerInstance.UpdateNearFarPlanes(zNear, m_light.GetRadius());
			viewerInstance.UpdateProjectionMatrix(projectionMatrix);
			viewerInstance.UpdateViewMatrix(Matrix4f::TransformInverse(m_light.GetPosition(), s_dirRotations[i]));

			m_pipeline.QueueTransfer(&viewerInstance);
		}

		m_onLightDataInvalidated.Connect(m_light.OnLightDataInvalidated, [this]([[maybe_unused]] Light* light)
		{
			assert(&m_light == light);

			Matrix4f projectionMatrix = Matrix4f::Perspective(RadianAnglef(HalfPi<float>()), 1.f, zNear, m_light.GetRadius());

			for (std::size_t i = 0; i < m_directions.size(); ++i)
			{
				DirectionData& direction = m_directions[i];

				ViewerInstance& viewerInstance = direction.viewer.GetViewerInstance();
				viewerInstance.UpdateProjectionMatrix(projectionMatrix);
				viewerInstance.UpdateNearFarPlanes(zNear, m_light.GetRadius());

				m_pipeline.QueueTransfer(&viewerInstance);
			}
		});

		m_onLightShadowMapSettingChange.Connect(m_light.OnLightShadowMapSettingChange, [this](Light* /*light*/, PixelFormat /*newPixelFormat*/, UInt32 newSize)
		{
			for (DirectionData& direction : m_directions)
				direction.viewer.UpdateViewport(Recti(0, 0, SafeCast<int>(newSize), SafeCast<int>(newSize)));
		});

		m_onLightTransformInvalidated.Connect(m_light.OnLightTransformInvalidated, [this]([[maybe_unused]] Light* light)
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
	}

	void PointLightShadowData::ForEachView(FunctionRef<void(std::size_t shadowAtlasEntry, ShadowViewer& shadowViewer)> callback)
	{
		std::size_t atlasEntry = m_firstShadowAtlasIndex;
		for (DirectionData& direction : m_directions)
			callback(atlasEntry++, direction.viewer);
	}

	void PointLightShadowData::RegisterToAtlas(ShadowAtlas& atlas)
	{
		UInt32 shadowMapSize = m_light.GetShadowMapSize();

		m_firstShadowAtlasIndex = atlas.Register(shadowMapSize, 6);
	}

	void PointLightShadowData::WriteToShader(const ShadowAtlas& atlas, [[maybe_unused]] const AbstractViewer* viewer, void* basePtr) const
	{
		assert(viewer == nullptr);

		for (UInt32 i = 0; i < 6; ++i)
		{
			std::optional<Rectf> rect = atlas.GetNormalizedRect(m_firstShadowAtlasIndex + i);
			if (!rect)
				rect = Rectf(-1.0f, -1.0f, -1.0f, -1.0f);

			AccessByOffset<Vector2f&>(basePtr, PredefinedPointShadowAtlasEntryOffsets.offset + i * sizeof(Vector2f)) = rect->GetPosition();
			AccessByOffset<Vector2f&>(basePtr, PredefinedPointShadowAtlasEntryOffsets.size + i * sizeof(Vector2f)) = rect->GetLengths();
		}
	}
}
