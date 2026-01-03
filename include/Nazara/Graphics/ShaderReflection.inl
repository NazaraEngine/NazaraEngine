// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline const RenderPipelineLayoutInfo& ShaderReflection::GetPipelineLayoutInfo() const&
	{
		return m_pipelineLayoutInfo;
	}

	inline RenderPipelineLayoutInfo&& ShaderReflection::GetPipelineLayoutInfo() &&
	{
		return std::move(m_pipelineLayoutInfo);
	}

	inline auto ShaderReflection::GetExternalBlockByTag(std::string_view tag) const -> const ExternalBlockData*
	{
		auto it = m_externalBlocks.find(tag);
		if (it == m_externalBlocks.end())
			return nullptr;

		return &it->second;
	}

	inline auto ShaderReflection::GetOptionByName(std::string_view optionName) const -> const OptionData*
	{
		auto it = m_options.find(optionName);
		if (it == m_options.end())
			return nullptr;

		return &it->second;
	}

	inline auto ShaderReflection::GetStructByIndex(std::size_t structIndex) const -> const StructData*
	{
		auto it = m_structs.find(structIndex);
		if (it == m_structs.end())
			return nullptr;

		return &it->second;
	}
}
