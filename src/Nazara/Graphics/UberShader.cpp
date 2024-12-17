// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <NZSL/Ast/ReflectVisitor.hpp>
#include <NZSL/Ast/SanitizeVisitor.hpp>
#include <limits>
#include <stdexcept>

namespace Nz
{
	UberShader::UberShader(nzsl::ShaderStageTypeFlags shaderStages, std::string moduleName) :
	UberShader(shaderStages, *Graphics::Instance()->GetShaderModuleResolver(), std::move(moduleName))
	{
	}

	UberShader::UberShader(nzsl::ShaderStageTypeFlags shaderStages, nzsl::ModuleResolver& moduleResolver, std::string moduleName) :
	m_shaderStages(shaderStages)
	{
		m_shaderModule = moduleResolver.Resolve(moduleName);
		if (!m_shaderModule)
			throw std::runtime_error(Format("failed to resolve shader module \"{0}\"", moduleName));

		try
		{
			m_shaderModule = Validate(*m_shaderModule, &m_optionIndexByName);
		}
		catch (const std::exception& e)
		{
			NazaraError("failed to compile ubershader {0}: {1}", moduleName, e.what());
			throw;
		}

		m_onShaderModuleUpdated.Connect(moduleResolver.OnModuleUpdated, [this, name = std::move(moduleName)](nzsl::ModuleResolver* resolver, const std::string& updatedModuleName)
		{
			if (!m_usedModules.contains(updatedModuleName))
				return;

			nzsl::Ast::ModulePtr newShaderModule = resolver->Resolve(name);
			if (!newShaderModule)
			{
				NazaraError("failed to retrieve updated shader module {0}", name);
				return;
			}

			try
			{
				m_shaderModule = Validate(*newShaderModule, &m_optionIndexByName);
			}
			catch (const std::exception& e)
			{
				NazaraError("failed to retrieve updated shader module {0}: {1}", name, e.what());
				return;
			}

			// Clear cache
			m_combinations.clear();

			OnShaderUpdated(this);
		});
	}

	UberShader::UberShader(nzsl::ShaderStageTypeFlags shaderStages, nzsl::Ast::ModulePtr shaderModule) :
	m_shaderModule(std::move(shaderModule)),
	m_shaderStages(shaderStages)
	{
		NazaraAssertMsg(m_shaderModule, "invalid shader module");

		try
		{
			m_shaderModule = Validate(*m_shaderModule, &m_optionIndexByName);
		}
		catch (const std::exception& e)
		{
			NazaraError("failed to compile ubershader: {0}", e.what());
			throw;
		}
	}

	const std::shared_ptr<ShaderModule>& UberShader::Get(const Config& config)
	{
		auto it = m_combinations.find(config);
		if (it == m_combinations.end())
		{
			nzsl::ShaderWriter::States states;

			// TODO: Remove this when arrays are accepted as config values
			for (const auto& [optionHash, optionValue] : config.optionValues)
			{
				std::uint32_t hash = optionHash;

				std::visit([&](auto&& arg)
				{
					states.optionValues[hash] = arg;
				}, optionValue);
			}
			states.shaderModuleResolver = Graphics::Instance()->GetShaderModuleResolver();

			std::shared_ptr<ShaderModule> stage;

			try
			{
				stage = Graphics::Instance()->GetRenderDevice()->InstantiateShaderModule(m_shaderStages, *m_shaderModule, std::move(states));
			}
			catch (const std::exception& e)
			{
				NazaraError("failed to instanciate shader: {0}", e.what());
				throw;
			}

			it = m_combinations.emplace(config, std::move(stage)).first;
		}

		return it->second;
	}

	nzsl::Ast::ModulePtr UberShader::Validate(const nzsl::Ast::Module& module, std::unordered_map<std::string, Option, StringHash<>, std::equal_to<>>* options)
	{
		NazaraAssertMsg(m_shaderStages != 0, "there must be at least one shader stage");
		assert(options);

		// Try to partially sanitize shader

		nzsl::Ast::SanitizeVisitor::Options sanitizeOptions;
		sanitizeOptions.partialSanitization = true;
		sanitizeOptions.moduleResolver = Graphics::Instance()->GetShaderModuleResolver();

		nzsl::Ast::ModulePtr sanitizedModule = nzsl::Ast::Sanitize(module, sanitizeOptions);
		m_usedModules.insert(sanitizedModule->metadata->moduleName);
		for (auto&& importedModule : sanitizedModule->importedModules)
			m_usedModules.insert(importedModule.module->metadata->moduleName);

		nzsl::ShaderStageTypeFlags supportedStageType;

		nzsl::Ast::ReflectVisitor::Callbacks callbacks;
		callbacks.onEntryPointDeclaration = [&](nzsl::ShaderStageType stageType, const std::string& /*name*/)
		{
			supportedStageType |= stageType;
		};

		std::unordered_map<std::string, Option, StringHash<>, std::equal_to<>> optionByName;
		callbacks.onOptionDeclaration = [&](const nzsl::Ast::DeclareOptionStatement& option)
		{
			//TODO: Check optionType

			optionByName[option.optName] = Option{
				nzsl::Ast::HashOption(option.optName)
			};
		};

		nzsl::Ast::ReflectVisitor reflect;
		reflect.Reflect(*sanitizedModule, callbacks);

		if ((m_shaderStages & supportedStageType) != m_shaderStages)
			throw std::runtime_error("shader doesn't support all required shader stages");

		*options = std::move(optionByName);

		return sanitizedModule;
	}
}
