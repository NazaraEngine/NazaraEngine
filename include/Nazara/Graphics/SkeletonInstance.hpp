// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SKELETONINSTANCE_HPP
#define NAZARA_GRAPHICS_SKELETONINSTANCE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/TransferInterface.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <memory>

namespace Nz
{
	class GpuCommandBufferBuilder;
	class Material;
	class GpuBuffer;
	struct RenderResourceReferences;
	class SkeletonInstance;
	class GpuUploadPool;

	using SkeletonInstancePtr = std::shared_ptr<SkeletonInstance>;

	class NAZARA_GRAPHICS_API SkeletonInstance : public TransferInterface
	{
		public:
			SkeletonInstance(std::shared_ptr<const Skeleton> skeleton);
			SkeletonInstance(const SkeletonInstance&) = delete;
			SkeletonInstance(SkeletonInstance&& skeletonInstance) noexcept;
			~SkeletonInstance() = default;

			inline std::shared_ptr<GpuBuffer>& GetSkeletalBuffer();
			inline const std::shared_ptr<GpuBuffer>& GetSkeletalBuffer() const;
			inline const std::shared_ptr<const Skeleton>& GetSkeleton() const;

			void OnTransfer(GpuResources& renderResources, GpuCommandBufferBuilder& builder) override;

			SkeletonInstance& operator=(const SkeletonInstance&) = delete;
			SkeletonInstance& operator=(SkeletonInstance&& skeletonInstance) noexcept;

		private:
			NazaraSlot(Skeleton, OnSkeletonJointsInvalidated, m_onSkeletonJointsInvalidated);

			std::shared_ptr<GpuBuffer> m_skeletalDataBuffer;
			std::shared_ptr<const Skeleton> m_skeleton;
			bool m_dataInvalidated;
	};
}

#include <Nazara/Graphics/SkeletonInstance.inl>

#endif // NAZARA_GRAPHICS_SKELETONINSTANCE_HPP
