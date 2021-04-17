// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <limits>
#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	UberShader::UberShader(ShaderStageType shaderStage, ShaderAst::StatementPtr shaderAst) :
	m_shaderAst(std::move(shaderAst)),
	m_shaderStage(shaderStage)
	{
		//std::size_t conditionCount = m_shaderAst.GetConditionCount();
		std::size_t conditionCount = 0;

		if (conditionCount >= 64)
			throw std::runtime_error("Too many conditions");

		m_combinationMask = std::numeric_limits<UInt64>::max();
		m_combinationMask <<= conditionCount;
		m_combinationMask = ~m_combinationMask;
	}

	UInt64 UberShader::GetConditionFlagByName(const std::string_view& condition) const
	{
		/*std::size_t conditionIndex = m_shaderAst.FindConditionByName(condition);
		if (conditionIndex != ShaderAst::InvalidCondition)
			return SetBit<UInt64>(0, conditionIndex);
		else*/
			return 0;
	}

	const std::shared_ptr<ShaderModule>& UberShader::Get(UInt64 combination)
	{
		combination &= m_combinationMask;

		auto it = m_combinations.find(combination);
		if (it == m_combinations.end())
		{
			ShaderWriter::States states;
			states.enabledOptions = combination;

			std::shared_ptr<ShaderModule> stage = Graphics::Instance()->GetRenderDevice().InstantiateShaderModule(m_shaderStage, m_shaderAst, std::move(states));

			it = m_combinations.emplace(combination, std::move(stage)).first;
		}

		return it->second;
	}
}
