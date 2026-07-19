// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_DEBUGDRAWER_HPP
#define NAZARA_RENDERER_DEBUGDRAWER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/VertexStruct.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <Nazara/Renderer/GpuUploadPool.hpp>
#include <Nazara/Renderer/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <memory>
#include <span>
#include <vector>

namespace Nz
{
	class GpuCommandBufferBuilder;
	class GpuBuffer;
	class GpuDevice;
	class GpuRenderPipeline;
	class GpuPipelineLayout;
	class GpuResources;
	class Skeleton;

	class NAZARA_RENDERER_API DebugDrawer
	{
		public:
			DebugDrawer(GpuDevice& renderDevice, bool usesReversedZ = false, std::size_t maxVertexPerDraw = DefaultVertexBlockSize);
			DebugDrawer(const DebugDrawer&) = delete;
			DebugDrawer(DebugDrawer&&) noexcept = default;
			~DebugDrawer();

			void Draw(GpuCommandBufferBuilder& builder);

			inline void DrawBox(const Boxf& box, const Color& color);
			inline void DrawBoxCorners(const EnumArray<BoxCorner, Vector3f>& boxCorners, const Color& color);
			inline void DrawFrustum(const Frustumf& frustum, const Color& color);
			inline void DrawLine(const Vector3f& start, const Vector3f& end, const Color& color);
			inline void DrawLine(const Vector3f& start, const Vector3f& end, const Color& startColor, const Color& endColor);
			inline void DrawLines(std::span<const Vector3f> positions, const Color& color);
			inline void DrawLines(std::span<const UInt16> indices, std::span<const Vector3f> positions, const Color& color);
			inline void DrawPoint(const Vector3f& point, const Color& color, float boxSize = 0.01f);
			void DrawSphere(const Spheref& sphere, const Color& color);
			inline void DrawSphere(const Vector3f& point, float radius, const Color& color);
			void DrawSkeleton(const Skeleton& skeleton, const Color& color);

			void Prepare(GpuResources& renderResources);

			void Reset();

			void SetViewerData(const Matrix4f& viewProjMatrix);

			void Upload(GpuCommandBufferBuilder& builder, GpuResources& renderResources);

			DebugDrawer& operator=(const DebugDrawer&) = delete;
			DebugDrawer& operator=(DebugDrawer&&) = delete;

			static constexpr std::size_t DefaultVertexBlockSize = 4096;

		private:
			struct ViewerData
			{
				std::shared_ptr<GpuBuffer> buffer;
				ShaderBindingPtr binding;
			};

			struct DataPool
			{
				std::vector<std::shared_ptr<GpuBuffer>> vertexBuffers;
				std::vector<ViewerData> viewerData;
			};

			struct DrawCall
			{
				std::shared_ptr<GpuBuffer> vertexBuffer;
				std::size_t vertexCount;
			};

			struct PendingUpload
			{
				GpuUploadPool::Allocation* allocation;
				GpuBuffer* vertexBuffer;
			};

			std::array<UInt8, PredefinedDebugDrawerOffsets.totalSize> m_viewerData;
			std::shared_ptr<DataPool> m_dataPool;
			std::shared_ptr<GpuRenderPipeline> m_renderPipeline;
			std::shared_ptr<GpuPipelineLayout> m_renderPipelineLayout;
			std::size_t m_vertexPerBlock;
			std::vector<DrawCall> m_drawCalls;
			std::vector<PendingUpload> m_pendingUploads;
			std::vector<VertexStruct_XYZ_Color> m_lineVertices;
			GpuDevice& m_renderDevice;
			ViewerData m_currentViewerData;
			bool m_viewerDataUpdated;
	};
}

#include <Nazara/Renderer/DebugDrawer.inl>

#endif // NAZARA_RENDERER_DEBUGDRAWER_HPP
