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
	UberShader::UberShader(ShaderStageType shaderStage, const ShaderAst::StatementPtr& shaderAst) :
	m_shaderStage(shaderStage)
	{
		ShaderAst::SanitizeVisitor::Options options;
		options.removeOptionDeclaration = false;

		m_shaderAst = ShaderAst::Sanitize(shaderAst, options);

		std::size_t optionCount = 0;

		ShaderAst::AstReflect::Callbacks callbacks;
		callbacks.onOptionDeclaration = [&](const std::string& optionName, const ShaderAst::ExpressionType& optionType)
		{
			m_optionIndexByName[optionName] = optionCount;
			optionCount++;
		};

		ShaderAst::AstReflect reflect;
		reflect.Reflect(m_shaderAst, callbacks);

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
			states.sanitized = true;

			std::shared_ptr<ShaderModule> stage = Graphics::Instance()->GetRenderDevice().InstantiateShaderModule(m_shaderStage, m_shaderAst, std::move(states));

			it = m_combinations.emplace(combination, std::move(stage)).first;
		}

		return it->second;
	}
}
