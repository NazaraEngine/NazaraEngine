// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <NZSL/Ast/Cloner.hpp>
#include <NZSL/Ast/TransformerExecutor.hpp>
#include <NZSL/Ast/Transformations/BindingResolverTransformer.hpp>
#include <NZSL/Ast/Transformations/ResolveTransformer.hpp>
#include <NZSL/Ast/Transformations/ValidationTransformer.hpp>

namespace Nz
{
	bool MaterialParams::IsValid() const
	{
		return true;
	}


	Material::Material(MaterialSettings settings, const std::string& referenceModuleName) :
	Material(std::move(settings), *Graphics::Instance()->GetShaderModuleResolver()->Resolve(referenceModuleName))
	{
	}

	Material::Material(MaterialSettings settings, const nzsl::Ast::Module& referenceModule) :
	m_settings(std::move(settings))
	{
		using namespace nzsl::Ast::Literals;

		Graphics* graphics = Graphics::Instance();

		const std::shared_ptr<RenderDevice>& renderDevice = graphics->GetRenderDevice();

		nzsl::Ast::TransformerExecutor executor;
		executor.AddPass<nzsl::Ast::ResolveTransformer>({ .moduleResolver = graphics->GetShaderModuleResolver() });
		executor.AddPass<nzsl::Ast::BindingResolverTransformer>({ .forceAutoBindingResolve = true });
		executor.AddPass<nzsl::Ast::ValidationTransformer>();

		nzsl::Ast::TransformerContext context;
		context.partialCompilation = true;
		context.optionValues["MaxLightCount"_opt] = SafeCast<UInt32>(PredefinedLightData::MaxLightCount);
		context.optionValues["MaxLightCascadeCount"_opt] = SafeCast<UInt32>(PredefinedDirectionalLightData::MaxLightCascadeCount);
		context.optionValues["MaxJointCount"_opt] = SafeCast<UInt32>(PredefinedSkeletalData::MaxMatricesCount);

		nzsl::Ast::ModulePtr resolvedModule = nzsl::Ast::Clone(referenceModule);
		executor.Transform(*resolvedModule, context);

		m_reflection.Reflect(*resolvedModule);

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

			for (const auto& [tag, storageBlock] : block->storageBlocks)
			{
				std::size_t storageBufferIndex = m_storageBuffers.size();

				auto& storageBuffer = m_storageBuffers.emplace_back();
				storageBuffer.bindingIndex = storageBlock.bindingIndex;
				storageBuffer.bindingSet = storageBlock.bindingSet;
				storageBuffer.structIndex = storageBlock.structIndex;

				m_storageBufferByTag.emplace(tag, storageBufferIndex);
			}

			for (const auto& [tag, shaderBlock] : block->uniformBlocks)
			{
				std::size_t blockIndex = m_uniformBlocks.size();

				const ShaderReflection::StructData* structData = m_reflection.GetStructByIndex(shaderBlock.structIndex);
				assert(structData);

				std::size_t size = structData->fieldOffsets.GetAlignedSize();

				auto& uniformBlock = m_uniformBlocks.emplace_back();
				uniformBlock.bindingIndex = shaderBlock.bindingIndex;
				uniformBlock.bindingSet = shaderBlock.bindingSet;
				uniformBlock.bufferPool = std::make_unique<RenderBufferPool>(renderDevice, BufferType::Uniform, size);
				uniformBlock.structIndex = shaderBlock.structIndex;

				m_uniformBlockByTag.emplace(tag, blockIndex);
			}
		}

		m_engineShaderBindings.fill(InvalidBindingIndex);
		if (const ShaderReflection::ExternalBlockData* block = m_reflection.GetExternalBlockByTag("Engine"))
		{
			// TODO: Ensure structs layout is what's expected

			if (auto it = block->uniformBlocks.find("InstanceData"); it != block->uniformBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::InstanceDataUbo] = it->second.bindingIndex;

			if (auto it = block->uniformBlocks.find("LightData"); it != block->uniformBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::LightDataUbo] = it->second.bindingIndex;

			if (auto it = block->uniformBlocks.find("ViewerData"); it != block->uniformBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::ViewerDataUbo] = it->second.bindingIndex;

			if (auto it = block->samplers.find("ShadowMapsDirectional"); it != block->samplers.end())
				m_engineShaderBindings[EngineShaderBinding::ShadowmapDirectional] = it->second.bindingIndex;

			if (auto it = block->samplers.find("ShadowMapsPoint"); it != block->samplers.end())
				m_engineShaderBindings[EngineShaderBinding::ShadowmapPoint] = it->second.bindingIndex;

			if (auto it = block->samplers.find("ShadowMapsSpot"); it != block->samplers.end())
				m_engineShaderBindings[EngineShaderBinding::ShadowmapSpot] = it->second.bindingIndex;

			if (auto it = block->uniformBlocks.find("SkeletalData"); it != block->uniformBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::SkeletalDataUbo] = it->second.bindingIndex;

			if (auto it = block->samplers.find("TextureOverlay"); it != block->samplers.end())
				m_engineShaderBindings[EngineShaderBinding::OverlayTexture] = it->second.bindingIndex;
		}

		for (const auto& handlerPtr : m_settings.GetPropertyHandlers())
			handlerPtr->Setup(*this, m_reflection);

		for (const auto& passOpt : m_settings.GetPasses())
		{
			if (!passOpt)
				continue;

			for (const auto& uberShader : passOpt->shaders)
			{
				uberShader->UpdateConfigCallback([=](UberShader::Config& config, const std::vector<RenderPipelineInfo::VertexBufferData>& vertexBuffers)
				{
					using namespace nzsl::Ast::Literals;

					if (vertexBuffers.empty())
						return;

					const VertexDeclaration& vertexDeclaration = *vertexBuffers.front().declaration;
					const auto& components = vertexDeclaration.GetComponents();

					UInt32 locationIndex = 0;
					for (const auto& component : components)
					{
						switch (component.component)
						{
							case VertexComponent::Color:
								config.optionValues["VertexColorLoc"_opt] = locationIndex;
								break;

							case VertexComponent::Normal:
								config.optionValues["VertexNormalLoc"_opt] = locationIndex;
								break;

							case VertexComponent::Position:
								config.optionValues["VertexPositionLoc"_opt] = locationIndex;
								break;

							case VertexComponent::SizeSinCos:
								config.optionValues["VertexSizeRotLocation"_opt] = locationIndex;
								break;

							case VertexComponent::Tangent:
								config.optionValues["VertexTangentLoc"_opt] = locationIndex;
								break;

							case VertexComponent::TexCoord:
								config.optionValues["VertexUvLoc"_opt] = locationIndex;
								break;

							case VertexComponent::JointIndices:
								config.optionValues["VertexJointIndicesLoc"_opt] = locationIndex;
								break;

							case VertexComponent::JointWeights:
								config.optionValues["VertexJointWeightsLoc"_opt] = locationIndex;
								break;

							case VertexComponent::Unused:
							case VertexComponent::Userdata:
								break;
						}

						++locationIndex;
					}
				});
			}
		}
	}

	std::shared_ptr<MaterialInstance> Material::GetDefaultInstance() const
	{
		std::shared_ptr<MaterialInstance> instance = m_defaultInstance.lock();
		if (!instance)
		{
			instance = Instantiate();
			m_defaultInstance = std::weak_ptr<MaterialInstance>(instance);
		}

		return instance;
	}

	std::shared_ptr<MaterialInstance> Material::Instantiate() const
	{
		return std::make_shared<MaterialInstance>(shared_from_this());
	}

	std::shared_ptr<Material> Material::Get(MaterialType lightingType)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssertMsg(graphics, "Graphics module has not been initialized");

		return graphics->GetDefaultMaterials().materials[lightingType].material;
	}

	std::shared_ptr<Material> Material::LoadFromFile(const std::filesystem::path& filePath, const MaterialParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssertMsg(graphics, "Graphics module has not been initialized");

		return graphics->GetMaterialLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<Material> Material::LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssertMsg(graphics, "Graphics module has not been initialized");

		return graphics->GetMaterialLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<Material> Material::LoadFromStream(Stream& stream, const MaterialParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssertMsg(graphics, "Graphics module has not been initialized");

		return graphics->GetMaterialLoader().LoadFromStream(stream, params);
	}
}
