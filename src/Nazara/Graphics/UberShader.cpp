// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Shader/Ast/AstReflect.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <limits>
#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	UberShader::UberShader(ShaderStageTypeFlags shaderStages, std::string moduleName) :
	UberShader(shaderStages, *Graphics::Instance()->GetShaderModuleResolver(), std::move(moduleName))
	{
	}

	UberShader::UberShader(ShaderStageTypeFlags shaderStages, ShaderModuleResolver& moduleResolver, std::string moduleName) :
	m_shaderStages(shaderStages)
	{
		m_shaderModule = moduleResolver.Resolve(moduleName);
		NazaraAssert(m_shaderModule, "invalid shader module");

		m_shaderModule = Validate(*m_shaderModule, &m_optionIndexByName);

		m_onShaderModuleUpdated.Connect(moduleResolver.OnModuleUpdated, [this, name = std::move(moduleName)](ShaderModuleResolver* resolver, const std::string& updatedModuleName)
		{
			if (updatedModuleName != name)
				return;

			ShaderAst::ModulePtr newShaderModule = resolver->Resolve(name);
			if (!newShaderModule)
			{
				NazaraError("failed to retrieve updated shader module " + name);
				return;
			}

			try
			{
				m_shaderModule = Validate(*newShaderModule, &m_optionIndexByName);
			}
			catch (const std::exception& e)
			{
				NazaraError("failed to retrieve updated shader module " + name + ": " + e.what());
				return;
			}

			// Clear cache
			m_combinations.clear();

			OnShaderUpdated(this);
		});
	}

	UberShader::UberShader(ShaderStageTypeFlags shaderStages, ShaderAst::ModulePtr shaderModule) :
	m_shaderModule(std::move(shaderModule)),
	m_shaderStages(shaderStages)
	{
		NazaraAssert(m_shaderModule, "invalid shader module");

		Validate(*m_shaderModule, &m_optionIndexByName);
	}

	const std::shared_ptr<ShaderModule>& UberShader::Get(const Config& config)
	{
		auto it = m_combinations.find(config);
		if (it == m_combinations.end())
		{
			ShaderWriter::States states;
			states.optionValues = config.optionValues;
			states.shaderModuleResolver = Graphics::Instance()->GetShaderModuleResolver();

			std::shared_ptr<ShaderModule> stage = Graphics::Instance()->GetRenderDevice()->InstantiateShaderModule(m_shaderStages, *m_shaderModule, std::move(states));

			it = m_combinations.emplace(config, std::move(stage)).first;
		}

		return it->second;
	}

	ShaderAst::ModulePtr UberShader::Validate(const ShaderAst::Module& module, std::unordered_map<std::string, Option>* options)
	{
		NazaraAssert(m_shaderStages != 0, "there must be at least one shader stage");
		assert(options);

		// Try to partially sanitize shader

		ShaderAst::SanitizeVisitor::Options sanitizeOptions;
		sanitizeOptions.allowPartialSanitization = true;

		ShaderAst::ModulePtr sanitizedModule = ShaderAst::Sanitize(module, sanitizeOptions);

		ShaderStageTypeFlags supportedStageType;

		ShaderAst::AstReflect::Callbacks callbacks;
		callbacks.onEntryPointDeclaration = [&](ShaderStageType stageType, const std::string& /*name*/)
		{
			supportedStageType |= stageType;
		};

		std::unordered_map<std::string, Option> optionByName;
		callbacks.onOptionDeclaration = [&](const ShaderAst::DeclareOptionStatement& option)
		{
			//TODO: Check optionType

			optionByName[option.optName] = Option{
				CRC32(option.optName)
			};
		};

		ShaderAst::AstReflect reflect;
		reflect.Reflect(*sanitizedModule->rootNode, callbacks);

		if ((m_shaderStages & supportedStageType) != m_shaderStages)
			throw std::runtime_error("shader doesn't support all required shader stages");

		*options = std::move(optionByName);

		return sanitizedModule;
	}
}
