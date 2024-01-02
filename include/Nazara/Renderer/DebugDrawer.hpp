// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_DEBUGDRAWER_HPP
#define NAZARA_RENDERER_DEBUGDRAWER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class CommandBufferBuilder;
	class RenderBuffer;
	class RenderDevice;
	class RenderPipeline;
	class RenderPipelineLayout;
	class RenderResources;
	class ShaderBinding;
	class Skeleton;

	class NAZARA_RENDERER_API DebugDrawer
	{
		public:
			DebugDrawer(RenderDevice& renderDevice, std::size_t maxVertexPerDraw = DefaultVertexBlockSize);
			DebugDrawer(const DebugDrawer&) = delete;
			DebugDrawer(DebugDrawer&&) noexcept = default;
			~DebugDrawer();

			void Draw(CommandBufferBuilder& builder);

			inline void DrawBox(const Boxf& box, const Color& color);
			inline void DrawBoxCorners(const EnumArray<BoxCorner, Vector3f>& boxCorners, const Color& color);
			inline void DrawFrustum(const Frustumf& frustum, const Color& color);
			inline void DrawLine(const Vector3f& start, const Vector3f& end, const Color& color);
			inline void DrawLine(const Vector3f& start, const Vector3f& end, const Color& startColor, const Color& endColor);
			inline void DrawPoint(const Vector3f& point, const Color& color, float boxSize = 0.01f);
			void DrawSkeleton(const Skeleton& skeleton, const Color& color);

			void Prepare(RenderResources& renderFrame);

			void Reset(RenderResources& renderFrame);

			void SetViewerData(const Matrix4f& viewProjMatrix);

			DebugDrawer& operator=(const DebugDrawer&) = delete;
			DebugDrawer& operator=(DebugDrawer&&) = delete;

			static constexpr std::size_t DefaultVertexBlockSize = 4096;

		private:
			struct ViewerData
			{
				std::shared_ptr<RenderBuffer> buffer;
				std::shared_ptr<ShaderBinding> binding;
			};

			struct DataPool
			{
				std::vector<std::shared_ptr<RenderBuffer>> vertexBuffers;
				std::vector<ViewerData> viewerData;
			};

			struct DrawCall
			{
				std::shared_ptr<RenderBuffer> vertexBuffer;
				std::size_t vertexCount;
			};

			struct PendingUpload
			{
				UploadPool::Allocation* allocation;
				RenderBuffer* vertexBuffer;
			};

			std::shared_ptr<DataPool> m_dataPool;
			std::shared_ptr<RenderPipeline> m_renderPipeline;
			std::shared_ptr<RenderPipelineLayout> m_renderPipelineLayout;
			std::size_t m_vertexPerBlock;
			std::vector<DrawCall> m_drawCalls;
			std::vector<PendingUpload> m_pendingUploads;
			std::vector<UInt8> m_viewerData;
			std::vector<VertexStruct_XYZ_Color> m_lineVertices;
			RenderDevice& m_renderDevice;
			ViewerData m_currentViewerData;
			bool m_viewerDataUpdated;
	};
}

#include <Nazara/Renderer/DebugDrawer.inl>

#endif // NAZARA_RENDERER_DEBUGDRAWER_HPP
