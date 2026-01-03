// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERPASSCACHE_HPP
#define NAZARA_RENDERER_RENDERPASSCACHE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <memory>
#include <unordered_map>
#include <variant>

namespace Nz
{
	class RenderDevice;

	class NAZARA_RENDERER_API RenderPassCache
	{
		public:
			inline RenderPassCache(RenderDevice& device);
			RenderPassCache(const RenderPassCache&) = delete;
			RenderPassCache(RenderPassCache&&) noexcept = default;
			~RenderPassCache() = default;

			const std::shared_ptr<RenderPass>& Get(const std::vector<RenderPass::Attachment>& attachments, const std::vector<RenderPass::SubpassDescription>& subpassDescriptions, const std::vector<RenderPass::SubpassDependency>& subpassDependencies) const;

			RenderPassCache& operator=(const RenderPassCache&) = delete;
			RenderPassCache& operator=(RenderPassCache&&) = delete;

		private:
			struct RenderPassData
			{
				std::size_t attachmentCount;
				std::size_t dependencyCount;
				std::size_t descriptionCount;
				const RenderPass::Attachment* attachments;
				const RenderPass::SubpassDependency* subpassDependencies;
				const RenderPass::SubpassDescription* subpassDescriptions;
			};

			using Key = std::variant<std::shared_ptr<RenderPass>, RenderPassData>;

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
			static inline RenderPassData ToRenderPassData(const std::shared_ptr<RenderPass>& renderPass);
			static inline const RenderPassData& ToRenderPassData(const RenderPassData& renderPassData);

			mutable std::unordered_map<Key, std::shared_ptr<RenderPass>, Hasher, EqualityChecker> m_renderPasses;
			RenderDevice& m_device;
	};
}

#include <Nazara/Renderer/RenderPassCache.inl>

#endif // NAZARA_RENDERER_RENDERPASSCACHE_HPP
