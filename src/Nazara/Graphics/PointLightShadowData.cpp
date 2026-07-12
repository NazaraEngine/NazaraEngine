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
		constexpr Matrix4f BuildViewMatrix(const Vector3f& lightPos, std::size_t directionIndex)
		{
			switch (directionIndex)
			{
				case 0:
					return Matrix4f(
						0.0, 0.0, -1.0, 0.0,
						0.0, 1.0, 0.0, 0.0,
						1.0, 0.0, 0.0, 0.0,
						-lightPos.z, -lightPos.y, lightPos.x, 1.0
					);
					
				case 1:
					return Matrix4f(
						0.0, 0.0, 1.0, 0.0,
						0.0, 1.0, 0.0, 0.0,
						-1.0, 0.0, 0.0, 0.0,
						lightPos.z, -lightPos.y, -lightPos.x, 1.0
					);
					
				case 2:
					return Matrix4f(
						1.0, 0.0, 0.0, 0.0,
						0.0, 0.0, -1.0, 0.0,
						0.0, 1.0, 0.0, 0.0,
						-lightPos.x, -lightPos.z, lightPos.y, 1.0
					);
					
				case 3:
					return Matrix4f(
						1.0, 0.0, 0.0, 0.0,
						0.0, 0.0, 1.0, 0.0,
						0.0, -1.0, 0.0, 0.0,
						-lightPos.x, lightPos.z, -lightPos.y, 1.0
					);
					
				case 4:
					return Matrix4f(
						1.0, 0.0, 0.0, 0.0,
						0.0, 1.0, 0.0, 0.0,
						0.0, 0.0, 1.0, 0.0,
						-lightPos.x, -lightPos.y, -lightPos.z, 1.0
					);
					
				case 5:
					return Matrix4f(
						-1.0, 0.0, 0.0, 0.0,
						0.0, 1.0, 0.0, 0.0,
						0.0, 0.0, -1.0, 0.0,
						lightPos.x, -lightPos.y, lightPos.z, 1.0
					);
			}

			NAZARA_UNREACHABLE();
		}

		constexpr std::array<std::string_view, 6> s_dirNames = {
			"Point-light shadow mapping +X",
			"Point-light shadow mapping -X",
			"Point-light shadow mapping +Y",
			"Point-light shadow mapping -Y",
			"Point-light shadow mapping -Z",
			"Point-light shadow mapping +Z"
		};
	}

	PointLightShadowData::PointLightShadowData(FramePipeline& pipeline, const PointLight& light) :
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
			viewerInstance.UpdateViewMatrix(BuildViewMatrix(m_light.GetPosition(), i));

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
				viewerInstance.UpdateViewMatrix(BuildViewMatrix(m_light.GetPosition(), i));

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
