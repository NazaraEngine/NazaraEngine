// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline RenderPassCache::RenderPassCache(RenderDevice& device) :
	m_device(device)
	{
	}

	template<typename T>
	std::size_t RenderPassCache::Hasher::operator()(const T& key) const
	{
		return operator()(ToRenderPassData(key));
	}

	template<typename T1, typename T2>
	inline bool RenderPassCache::EqualityChecker::operator()(const T1& lhs, const T2& rhs) const
	{
		return operator()(ToRenderPassData(lhs), ToRenderPassData(rhs));
	}

	inline auto RenderPassCache::ToRenderPassData(const Key& key)
	{
		return std::visit([&](auto&& arg)
		{
			return ToRenderPassData(arg);
		}, key);
	}

	inline auto RenderPassCache::ToRenderPassData(const std::shared_ptr<RenderPass>& renderPass) -> RenderPassData
	{
		const auto& attachments = renderPass->GetAttachments();
		const auto& subpassDeps = renderPass->GetSubpassDependencies();
		const auto& subpassDesc = renderPass->GetSubpassDescriptions();

		RenderPassData data;
		data.attachmentCount = attachments.size();
		data.attachments = attachments.data();

		data.dependencyCount = subpassDeps.size();
		data.subpassDependencies = subpassDeps.data();

		data.descriptionCount = subpassDesc.size();
		data.subpassDescriptions = subpassDesc.data();

		return data;
	}

	inline auto RenderPassCache::ToRenderPassData(const RenderPassData& renderPassData) -> const RenderPassData&
	{
		return renderPassData;
	}
}

