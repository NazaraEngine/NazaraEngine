// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/SkeletonInstance.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
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

		const std::shared_ptr<GpuDevice>& renderDevice = graphics->GetRenderDevice();

		nzsl::Ast::TransformerExecutor executor;
		executor.AddPass<nzsl::Ast::ResolveTransformer>({ .moduleResolver = graphics->GetShaderModuleResolver() });
		executor.AddPass<nzsl::Ast::BindingResolverTransformer>({ .forceAutoBindingResolve = true });
		executor.AddPass<nzsl::Ast::ValidationTransformer>();

		nzsl::Ast::TransformerContext context;
		context.partialCompilation = true;
		context.optionValues["MaxLightCascadeCount"_opt] = SafeCast<UInt32>(PredefinedDirectionalShadowAtlasEntryData::MaxLightCascadeCount);
		context.optionValues["MaxJointCount"_opt] = SafeCast<UInt32>(PredefinedSkeletalData::MaxMatricesCount);

		nzsl::Ast::ModulePtr resolvedModule = nzsl::Ast::Clone(referenceModule);
		executor.Transform(*resolvedModule, context);

		m_reflection.Reflect(*resolvedModule);

		struct SetData
		{
			HybridVector<std::size_t, 10> bindingHashes;
		};

		const GpuPipelineLayoutInfo& pipelineLayoutInfo = m_reflection.GetPipelineLayoutInfo();

		HybridVector<SetData, 4> setHashes;
		for (const auto& binding : pipelineLayoutInfo.bindings)
		{
			if (binding.setIndex >= setHashes.size())
				setHashes.resize(binding.setIndex + 1);

			SetData& setData = setHashes[binding.setIndex];
			if (binding.bindingIndex >= setData.bindingHashes.size())
				setData.bindingHashes.resize(binding.bindingIndex + 1);

			setData.bindingHashes[binding.bindingIndex] = HashCombine(binding.arraySize, binding.shaderStageFlags, binding.type);
		}

		std::size_t previousHash = std::hash<UInt32>{}(pipelineLayoutInfo.pushConstantSize);
		for (std::size_t i = 0; i < setHashes.size(); ++i)
		{
			SetData& setData = setHashes[i];

			std::size_t setHash = previousHash;
			for (std::size_t hash : setData.bindingHashes)
				HashCombine(setHash, hash);

			previousHash = setHash;
			m_bindingSetHashes.push_back(setHash);
		}

		m_renderPipelineLayout = renderDevice->InstantiateRenderPipelineLayout(pipelineLayoutInfo);

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
				uniformBlock.bufferPool = std::make_unique<GpuBufferPool>(renderDevice, BufferUsage::UniformBuffer | BufferUsage::DeviceLocal, size);
				uniformBlock.structIndex = shaderBlock.structIndex;

				m_uniformBlockByTag.emplace(tag, blockIndex);
			}
		}

		m_engineShaderBindings.fill(InvalidBindingIndex);
		if (const ShaderReflection::ExternalBlockData* block = m_reflection.GetExternalBlockByTag("Scene"))
		{
			// TODO: Check set index
			// TODO: Ensure structs layout is what's expected

			if (auto it = block->storageBlocks.find("DirectionalLights"); it != block->storageBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::DirectionalLights] = it->second.bindingIndex;

			if (auto it = block->storageBlocks.find("DirectionalShadowAtlasMapping"); it != block->storageBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::DirectionalShadowAtlasMapping] = it->second.bindingIndex;

			if (auto it = block->storageBlocks.find("InstanceBuffer"); it != block->storageBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::InstanceBuffer] = it->second.bindingIndex;

			if (auto it = block->storageBlocks.find("PointLights"); it != block->storageBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::PointLights] = it->second.bindingIndex;

			if (auto it = block->storageBlocks.find("PointShadowAtlasMapping"); it != block->storageBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::PointShadowAtlasMapping] = it->second.bindingIndex;

			if (auto it = block->storageBlocks.find("SpotLights"); it != block->storageBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::SpotLights] = it->second.bindingIndex;

			if (auto it = block->storageBlocks.find("SpotShadowAtlasMapping"); it != block->storageBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::SpotShadowAtlasMapping] = it->second.bindingIndex;

			if (auto it = block->samplers.find("ShadowAtlas"); it != block->samplers.end())
				m_engineShaderBindings[EngineShaderBinding::ShadowAtlas] = it->second.bindingIndex;
		}

		if (const ShaderReflection::ExternalBlockData* block = m_reflection.GetExternalBlockByTag("Viewer"))
		{
			// TODO: Check set index
			// TODO: Ensure structs layout is what's expected

			if (auto it = block->uniformBlocks.find("ViewerData"); it != block->uniformBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::ViewerDataUbo] = it->second.bindingIndex;
		}

		if (const ShaderReflection::ExternalBlockData* block = m_reflection.GetExternalBlockByTag("Instance"))
		{
			// TODO: Check set index
			// TODO: Ensure structs layout is what's expected

			if (auto it = block->uniformBlocks.find("SkeletalData"); it != block->uniformBlocks.end())
				m_engineShaderBindings[EngineShaderBinding::SkeletalDataUbo] = it->second.bindingIndex;
		}

		for (const auto& handlerPtr : m_settings.GetPropertyHandlers())
			handlerPtr->Setup(*this, m_reflection);

		for (const auto& passOpt : m_settings.GetPasses())
		{
			if (!passOpt)
				continue;

			for (const auto& uberShader : passOpt->shaders)
			{
				uberShader->UpdateConfigCallback([=](UberShader::Config& config, const RenderPipelineInfo::VertexInputVector& vertexBuffers)
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

							case VertexComponent::Userdata:
							{
								std::string key = fmt::format("VertexUserdata{}Loc", locationIndex);
								config.optionValues[nzsl::Ast::HashOption(key)] = locationIndex;
								break;
							}

							case VertexComponent::Unused:
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

	void Material::FillSceneBindings(const ElementRenderer::SceneData& sceneData, std::vector<ShaderBinding::Binding>& bindings) const
	{
		if (UInt32 bindingIndex = GetEngineBindingIndex(EngineShaderBinding::DirectionalLights); bindingIndex != Material::InvalidBindingIndex && sceneData.directionalLights)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.directionalLights);
		}

		if (UInt32 bindingIndex = GetEngineBindingIndex(EngineShaderBinding::DirectionalShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && sceneData.directionalLightAtlasMapping)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.directionalLightAtlasMapping);
		}

		if (UInt32 bindingIndex = GetEngineBindingIndex(EngineShaderBinding::InstanceBuffer); bindingIndex != Material::InvalidBindingIndex && sceneData.instanceBuffer)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.instanceBuffer);
		}

		if (UInt32 bindingIndex = GetEngineBindingIndex(EngineShaderBinding::PointLights); bindingIndex != Material::InvalidBindingIndex && sceneData.pointLights)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.pointLights);
		}

		if (UInt32 bindingIndex = GetEngineBindingIndex(EngineShaderBinding::PointShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && sceneData.pointLightAtlasMapping)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.pointLightAtlasMapping);
		}

		if (UInt32 bindingIndex = GetEngineBindingIndex(EngineShaderBinding::ShadowAtlas); bindingIndex != Material::InvalidBindingIndex && sceneData.shadowAtlas)
		{
			const auto& shadowSampler = Graphics::Instance()->GetSamplerCache().Get({ .depthCompare = true });

			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::SampledTextureBinding{
				.texture = sceneData.shadowAtlas.get(),
				.sampler = shadowSampler.get()
			};
		}

		if (UInt32 bindingIndex = GetEngineBindingIndex(EngineShaderBinding::SpotLights); bindingIndex != Material::InvalidBindingIndex && sceneData.spotLights)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.spotLights);
		}

		if (UInt32 bindingIndex = GetEngineBindingIndex(EngineShaderBinding::SpotShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && sceneData.spotLightAtlasMapping)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.spotLightAtlasMapping);
		}
	}

	void Material::FillSkeletonBindings(const SkeletonInstance& skeleton, std::vector<ShaderBinding::Binding>& bindings) const
	{
		if (UInt32 bindingIndex = GetEngineBindingIndex(EngineShaderBinding::SkeletalDataUbo); bindingIndex != Material::InvalidBindingIndex)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::UniformBufferBinding::WholeBuffer(*skeleton.GetSkeletalBuffer());
		}
	}

	void Material::FillViewerBindings(const AbstractViewer& viewer, std::vector<ShaderBinding::Binding>& bindings) const
	{
		if (UInt32 bindingIndex = GetEngineBindingIndex(EngineShaderBinding::ViewerDataUbo); bindingIndex != Material::InvalidBindingIndex)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::UniformBufferBinding::WholeBuffer(*viewer.GetViewerInstance().GetViewerBuffer());
		}
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
