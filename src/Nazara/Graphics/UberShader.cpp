// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Shader/Ast/AstReflect.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <limits>
#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	UberShader::UberShader(ShaderStageTypeFlags shaderStages, const ShaderAst::StatementPtr& shaderAst) :
	m_shaderStages(shaderStages)
	{
		NazaraAssert(m_shaderStages != 0, "there must be at least one shader stage");

		//TODO: Try to partially sanitize shader?
		m_shaderAst = ShaderAst::Clone(*shaderAst);

		std::size_t optionCount = 0;

		ShaderStageTypeFlags supportedStageType;

		ShaderAst::AstReflect::Callbacks callbacks;
		callbacks.onEntryPointDeclaration = [&](ShaderStageType stageType, const std::string& /*name*/)
		{
			supportedStageType |= stageType;
		};

		callbacks.onOptionDeclaration = [&](const std::string& optionName, const ShaderAst::ExpressionType& optionType)
		{
			m_optionIndexByName[optionName] = optionCount;
			optionCount++;
		};

		ShaderAst::AstReflect reflect;
		reflect.Reflect(*m_shaderAst, callbacks);

		if (m_shaderStages & supportedStageType != m_shaderStages)
			throw std::runtime_error("shader doesn't support all required shader stages");

		if (optionCount >= 64)
			throw std::runtime_error("Too many conditions");

		m_combinationMask = std::numeric_limits<UInt64>::max();
		m_combinationMask <<= optionCount;
		m_combinationMask = ~m_combinationMask;
	}

	UInt64 UberShader::GetOptionFlagByName(const std::string& optionName) const
	{
		auto it = m_optionIndexByName.find(optionName);
		if (it == m_optionIndexByName.end())
			return 0;

		return SetBit<UInt64>(0, it->second);
	}

	const std::shared_ptr<ShaderModule>& UberShader::Get(UInt64 combination)
	{
		combination &= m_combinationMask;

		auto it = m_combinations.find(combination);
		if (it == m_combinations.end())
		{
			ShaderWriter::States states;
			states.enabledOptions = combination;

			std::shared_ptr<ShaderModule> stage = Graphics::Instance()->GetRenderDevice()->InstantiateShaderModule(m_shaderStages, *m_shaderAst, std::move(states));

			it = m_combinations.emplace(combination, std::move(stage)).first;
		}

		return it->second;
	}
}
