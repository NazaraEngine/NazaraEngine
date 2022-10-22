// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <NZSL/Ast/SanitizeVisitor.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	bool MaterialParams::IsValid() const
	{
		return true;
	}


	Material::Material(MaterialSettings settings, const std::string& referenceModuleName) :
	Material(std::move(settings), Graphics::Instance()->GetShaderModuleResolver()->Resolve(referenceModuleName))
	{
	}

	Material::Material(MaterialSettings settings, const nzsl::Ast::ModulePtr& referenceModule) :
	m_settings(std::move(settings))
	{
		NazaraAssert(referenceModule, "invalid module");

		Graphics* graphics = Graphics::Instance();

		const std::shared_ptr<RenderDevice>& renderDevice = graphics->GetRenderDevice();

		nzsl::Ast::SanitizeVisitor::Options options;
		options.allowPartialSanitization = true;
		options.moduleResolver = graphics->GetShaderModuleResolver();

		nzsl::Ast::ModulePtr sanitizedModule = nzsl::Ast::Sanitize(*referenceModule, options);

		m_reflection.Reflect(*sanitizedModule);

		m_renderPipelineLayout = renderDevice->InstantiateRenderPipelineLayout(m_reflection.GetPipelineLayoutInfo());

		if (const ShaderReflection::ExternalBlockData* block = m_reflection.GetExternalBlockByTag("Material"))
		{
			for (const auto& [tag, shaderSampler] : block->samplers)
			{
				std::size_t textureIndex = m_textures.size();

				auto& texture = m_textures.emplace_back();
				texture.bindingIndex = shaderSampler.bindingIndex;
				texture.bindingSet = shaderSampler.bindingSet;
				texture.imageType = ToImageType(shaderSampler.imageType);

				m_textureByTag.emplace(tag, textureIndex);
			}

			assert(block->storageBlocks.empty()); //< TODO

			for (const auto& [tag, shaderBlock] : block->uniformBlocks)
			{
				std::size_t blockIndex = m_uniformBlocks.size();

				const ShaderReflection::StructData* structData = m_reflection.GetStructByIndex(shaderBlock.structIndex);
				assert(structData);

				std::size_t size = structData->fieldOffsets.GetSize();

				auto& uniformBlock = m_uniformBlocks.emplace_back();
				uniformBlock.bindingIndex = shaderBlock.bindingIndex;
				uniformBlock.bindingSet = shaderBlock.bindingSet;
				uniformBlock.bufferPool = std::make_unique<RenderBufferPool>(renderDevice, BufferType::Uniform, size);

				m_uniformBlockByTag.emplace(tag, blockIndex);
			}
		}

		for (const auto& handlerPtr : m_settings.GetPropertyHandlers())
			handlerPtr->Setup(*this, m_reflection);
	}

	void Material::AddPass(std::string passName, std::shared_ptr<MaterialPass> pass)
	{
		auto& registry = Graphics::Instance()->GetMaterialPassRegistry();
		return AddPass(registry.GetPassIndex(passName), std::move(pass));
	}

	const std::shared_ptr<MaterialPass>& Material::FindPass(const std::string& passName) const
	{
		auto& registry = Graphics::Instance()->GetMaterialPassRegistry();
		return GetPass(registry.GetPassIndex(passName));
	}

	void Material::RemovePass(const std::string& passName)
	{
		auto& registry = Graphics::Instance()->GetMaterialPassRegistry();
		return RemovePass(registry.GetPassIndex(passName));
	}


	std::shared_ptr<Material> Material::Build(const ParameterList& materialData)
	{
		return std::shared_ptr<Material>();
	}

	std::shared_ptr<Material> Material::LoadFromFile(const std::filesystem::path& filePath, const MaterialParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics, "Utility module has not been initialized");

		return graphics->GetMaterialLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<Material> Material::LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics, "Utility module has not been initialized");

		return graphics->GetMaterialLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<Material> Material::LoadFromStream(Stream& stream, const MaterialParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics, "Utility module has not been initialized");

		return graphics->GetMaterialLoader().LoadFromStream(stream, params);
	}
}
