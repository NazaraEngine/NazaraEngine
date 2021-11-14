// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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
			m_optionIndexByName[optionName] = Option{
				optionCount,
				optionType
			};

			optionCount++;
		};

		ShaderAst::AstReflect reflect;
		reflect.Reflect(*m_shaderAst, callbacks);

		if ((m_shaderStages & supportedStageType) != m_shaderStages)
			throw std::runtime_error("shader doesn't support all required shader stages");

		if (optionCount >= MaximumOptionValue)
			throw std::runtime_error("Too many shader options (at most " + std::to_string(MaximumOptionValue) + " are supported)");
	}

	const std::shared_ptr<ShaderModule>& UberShader::Get(const Config& config)
	{
		auto it = m_combinations.find(config);
		if (it == m_combinations.end())
		{
			ShaderWriter::States states;

			for (std::size_t i = 0; i < MaximumOptionValue; ++i)
			{
				if (!std::holds_alternative<ShaderAst::NoValue>(config.optionValues[i]))
					states.optionValues[i] = config.optionValues[i];
			}

			std::shared_ptr<ShaderModule> stage = Graphics::Instance()->GetRenderDevice()->InstantiateShaderModule(m_shaderStages, *m_shaderAst, std::move(states));

			it = m_combinations.emplace(config, std::move(stage)).first;
		}

		return it->second;
	}
}
