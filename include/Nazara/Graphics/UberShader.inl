// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline ShaderStageTypeFlags UberShader::GetSupportedStages() const
	{
		return m_shaderStages;
	}

	inline bool UberShader::HasOption(const std::string& optionName, Pointer<const Option>* option) const
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
		for (std::size_t i = 0; i < lhs.optionValues.size(); ++i)
		{
			if (lhs.optionValues[i] != rhs.optionValues[i])
				return false;
		}

		return true;
	}

	inline std::size_t UberShader::ConfigHasher::operator()(const Config& config) const
	{
		std::size_t hash = 0;
		for (std::size_t i = 0; i < config.optionValues.size(); ++i)
			HashCombine(hash, config.optionValues[i]);

		return hash;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
