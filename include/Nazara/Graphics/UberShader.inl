// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline nzsl::ShaderStageTypeFlags UberShader::GetSupportedStages() const
	{
		return m_shaderStages;
	}

	inline bool UberShader::HasOption(std::string_view optionName, Pointer<const Option>* option) const
	{
		auto it = m_optionIndexByName.find(optionName);
		if (it == m_optionIndexByName.end())
			return false;

		if (option)
			*option = &it->second;

		return true;
	}

	inline void UberShader::UpdateConfig(Config& config, const std::vector<RenderPipelineInfo::VertexBufferData>& vertexBuffers)
	{
		if (m_configCallback)
			m_configCallback(config, vertexBuffers);
	}

	inline void UberShader::UpdateConfigCallback(ConfigCallback callback)
	{
		m_configCallback = std::move(callback);
	}

	inline bool UberShader::ConfigEqual::operator()(const Config& lhs, const Config& rhs) const
	{
		return lhs.optionValues == rhs.optionValues;
	}

	inline std::size_t UberShader::ConfigHasher::operator()(const Config& config) const
	{
		std::size_t hash = 0;
		for (auto&& [key, value] : config.optionValues)
		{
			HashCombine(hash, key);
			HashCombine(hash, value);
		}

		return hash;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
