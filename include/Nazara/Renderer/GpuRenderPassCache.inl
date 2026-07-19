// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline GpuRenderPassCache::GpuRenderPassCache(GpuDevice& device) :
	m_device(device)
	{
	}

	template<typename T>
	std::size_t GpuRenderPassCache::Hasher::operator()(const T& key) const
	{
		return operator()(ToRenderPassData(key));
	}

	template<typename T1, typename T2>
	inline bool GpuRenderPassCache::EqualityChecker::operator()(const T1& lhs, const T2& rhs) const
	{
		return operator()(ToRenderPassData(lhs), ToRenderPassData(rhs));
	}

	inline auto GpuRenderPassCache::ToRenderPassData(const Key& key)
	{
		return std::visit([&](auto&& arg)
		{
			return ToRenderPassData(arg);
		}, key);
	}

	inline auto GpuRenderPassCache::ToRenderPassData(const std::shared_ptr<GpuRenderPass>& renderPass) -> RenderPassData
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

	inline auto GpuRenderPassCache::ToRenderPassData(const RenderPassData& renderPassData) -> const RenderPassData&
	{
		return renderPassData;
	}
}
