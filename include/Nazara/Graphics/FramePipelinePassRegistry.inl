// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::unique_ptr<FramePipelinePass> FramePipelinePassRegistry::BuildPass(std::size_t passIndex, FramePipelinePass::PassData& passData, std::string passName, const ParameterList& parameters) const
	{
		assert(passIndex < m_passes.size());
		return m_passes[passIndex].factory(passData, passName, parameters);
	}

	inline std::size_t FramePipelinePassRegistry::GetPassIndex(std::string_view passName) const
	{
		auto it = m_passIndex.find(passName);
		if (it == m_passIndex.end())
			return InvalidIndex;

		return it->second;
	}

	inline std::size_t FramePipelinePassRegistry::GetPassInputIndex(std::size_t passIndex, std::string_view inputName) const
	{
		assert(passIndex < m_passes.size());
		auto& passData = m_passes[passIndex];
		auto it = std::find(passData.inputs.begin(), passData.inputs.end(), inputName);
		if (it == passData.inputs.end())
			return InvalidIndex;

		return std::distance(passData.inputs.begin(), it);
	}

	inline std::size_t FramePipelinePassRegistry::GetPassOutputIndex(std::size_t passIndex, std::string_view outputName) const
	{
		assert(passIndex < m_passes.size());
		auto& passData = m_passes[passIndex];
		auto it = std::find(passData.outputs.begin(), passData.outputs.end(), outputName);
		if (it == passData.outputs.end())
			return InvalidIndex;

		return std::distance(passData.outputs.begin(), it);
	}

	template<typename T>
	std::size_t FramePipelinePassRegistry::RegisterPass(std::string passName, std::vector<std::string> inputs, std::vector<std::string> outputs)
	{
		return RegisterPass(std::move(passName), std::move(inputs), std::move(outputs), [](FramePipelinePass::PassData& passData, std::string passName, const ParameterList& parameters) -> std::unique_ptr<FramePipelinePass>
		{
			return std::make_unique<T>(passData, std::move(passName), parameters);
		});
	}

	inline std::size_t FramePipelinePassRegistry::RegisterPass(std::string passName, std::vector<std::string> inputs, std::vector<std::string> outputs, Factory factory)
	{
		if (m_passIndex.find(passName) != m_passIndex.end())
			throw std::runtime_error("pass " + passName + " is already registered");

		m_passNames.push_back(std::move(passName));

		std::size_t passIndex = m_passIndex.size();
		m_passIndex.emplace(m_passNames.back(), passIndex);
		auto& passData = m_passes.emplace_back();
		passData.factory = std::move(factory);
		passData.inputs = std::move(inputs);
		passData.outputs = std::move(outputs);

		return passIndex;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
