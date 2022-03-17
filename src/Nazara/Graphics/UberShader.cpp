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

		Validate(*m_shaderModule);

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
				// FIXME: Validate is destructive, in case of failure it can invalidate the shader
				Validate(*newShaderModule);
			}
			catch (const std::exception& e)
			{
				NazaraError("failed to retrieve updated shader module " + name + ": " + e.what());
				return;
			}

			m_shaderModule = std::move(newShaderModule);

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

		Validate(*m_shaderModule);
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

	void UberShader::Validate(ShaderAst::Module& module)
	{
		NazaraAssert(m_shaderStages != 0, "there must be at least one shader stage");

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
		reflect.Reflect(*module.rootNode, callbacks);

		if ((m_shaderStages & supportedStageType) != m_shaderStages)
			throw std::runtime_error("shader doesn't support all required shader stages");
	}
}
