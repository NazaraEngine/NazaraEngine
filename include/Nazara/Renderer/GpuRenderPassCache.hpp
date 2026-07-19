// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_GPURENDERPASSCACHE_HPP
#define NAZARA_RENDERER_GPURENDERPASSCACHE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <Nazara/Renderer/GpuRenderPass.hpp>
#include <memory>
#include <unordered_map>
#include <variant>

namespace Nz
{
	class GpuDevice;

	class NAZARA_RENDERER_API GpuRenderPassCache
	{
		public:
			inline GpuRenderPassCache(GpuDevice& device);
			GpuRenderPassCache(const GpuRenderPassCache&) = delete;
			GpuRenderPassCache(GpuRenderPassCache&&) noexcept = default;
			~GpuRenderPassCache() = default;

			const std::shared_ptr<GpuRenderPass>& Get(const std::vector<GpuRenderPass::Attachment>& attachments, const std::vector<GpuRenderPass::SubpassDescription>& subpassDescriptions, const std::vector<GpuRenderPass::SubpassDependency>& subpassDependencies) const;

			GpuRenderPassCache& operator=(const GpuRenderPassCache&) = delete;
			GpuRenderPassCache& operator=(GpuRenderPassCache&&) = delete;

		private:
			struct RenderPassData
			{
				std::size_t attachmentCount;
				std::size_t dependencyCount;
				std::size_t descriptionCount;
				const GpuRenderPass::Attachment* attachments;
				const GpuRenderPass::SubpassDependency* subpassDependencies;
				const GpuRenderPass::SubpassDescription* subpassDescriptions;
			};

			using Key = std::variant<std::shared_ptr<GpuRenderPass>, RenderPassData>;

			struct Hasher
			{
				template<typename T> std::size_t operator()(const T& renderPass) const;
				std::size_t operator()(const RenderPassData& renderPassData) const;
			};

			struct EqualityChecker
			{
				template<typename T1, typename T2> bool operator()(const T1& lhs, const T2& rhs) const;
				bool operator()(const RenderPassData& lhs, const RenderPassData& rhs) const;
			};

			static inline auto ToRenderPassData(const Key& key);
			static inline RenderPassData ToRenderPassData(const std::shared_ptr<GpuRenderPass>& renderPass);
			static inline const RenderPassData& ToRenderPassData(const RenderPassData& renderPassData);

			mutable std::unordered_map<Key, std::shared_ptr<GpuRenderPass>, Hasher, EqualityChecker> m_renderPasses;
			GpuDevice& m_device;
	};
}

#include <Nazara/Renderer/GpuRenderPassCache.inl>

#endif // NAZARA_RENDERER_GPURENDERPASSCACHE_HPP
