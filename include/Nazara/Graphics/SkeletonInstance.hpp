// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SKELETONINSTANCE_HPP
#define NAZARA_GRAPHICS_SKELETONINSTANCE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/TransferInterface.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <memory>

namespace Nz
{
	class CommandBufferBuilder;
	class RenderBuffer;
	class SkeletonInstance;
	class UploadPool;

	using SkeletonInstancePtr = std::shared_ptr<SkeletonInstance>;

	class NAZARA_GRAPHICS_API SkeletonInstance : public TransferInterface
	{
		public:
			SkeletonInstance(std::shared_ptr<const Skeleton> skeleton);
			SkeletonInstance(const SkeletonInstance&) = delete;
			SkeletonInstance(SkeletonInstance&& skeletonInstance) noexcept;
			~SkeletonInstance() = default;

			inline std::shared_ptr<RenderBuffer>& GetSkeletalBuffer();
			inline const std::shared_ptr<RenderBuffer>& GetSkeletalBuffer() const;
			inline const std::shared_ptr<const Skeleton>& GetSkeleton() const;

			void OnTransfer(RenderFrame& renderFrame, CommandBufferBuilder& builder) override;

			SkeletonInstance& operator=(const SkeletonInstance&) = delete;
			SkeletonInstance& operator=(SkeletonInstance&& skeletonInstance) noexcept;

		private:
			NazaraSlot(Skeleton, OnSkeletonJointsInvalidated, m_onSkeletonJointsInvalidated);

			std::shared_ptr<RenderBuffer> m_skeletalDataBuffer;
			std::shared_ptr<const Skeleton> m_skeleton;
			bool m_dataInvalided;
	};
}

#include <Nazara/Graphics/SkeletonInstance.inl>

#endif // NAZARA_GRAPHICS_SKELETONINSTANCE_HPP
