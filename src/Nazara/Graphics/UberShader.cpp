// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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
	UberShader::UberShader(ShaderStageTypeFlags shaderStages, ShaderAst::ModulePtr shaderModule) :
	m_shaderModule(std::move(shaderModule)),
	m_shaderStages(shaderStages)
	{
		NazaraAssert(m_shaderStages != 0, "there must be at least one shader stage");
		NazaraAssert(m_shaderModule, "invalid shader module");

		//TODO: Try to partially sanitize shader?

		std::size_t optionCount = 0;

		ShaderStageTypeFlags supportedStageType;

		ShaderAst::AstReflect::Callbacks callbacks;
		callbacks.onEntryPointDeclaration = [&](ShaderStageType stageType, const std::string& /*name*/)
		{
			supportedStageType |= stageType;
		};

		callbacks.onOptionDeclaration = [&](const ShaderAst::DeclareOptionStatement& option)
		{
			//TODO: Check optionType

			m_optionIndexByName[option.optName] = Option{
				CRC32(option.optName)
			};

			optionCount++;
		};

		ShaderAst::AstReflect reflect;
		reflect.Reflect(*m_shaderModule->rootNode, callbacks);

		if ((m_shaderStages & supportedStageType) != m_shaderStages)
			throw std::runtime_error("shader doesn't support all required shader stages");
	}

	const std::shared_ptr<ShaderModule>& UberShader::Get(const Config& config)
	{
		auto it = m_combinations.find(config);
		if (it == m_combinations.end())
		{
			ShaderWriter::States states;
			states.optionValues = config.optionValues;

			std::shared_ptr<ShaderModule> stage = Graphics::Instance()->GetRenderDevice()->InstantiateShaderModule(m_shaderStages, *m_shaderModule, std::move(states));

			it = m_combinations.emplace(config, std::move(stage)).first;
		}

		return it->second;
	}
}
